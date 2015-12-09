#include "Session.h"
#include "ROI.h"
#include "Thread.h"
#include "XML.h"
#include "AlarmPluginManager.h"
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

#include <QDebug>
#include <QTime>
#include <QFile>
#include <QFileInfo>


using namespace cv;

Session::Session()
{
	qDebug() << "=== Session constructor ===";

	// Init XML values
	m_dirty = false;
	m_results = true;
	m_images = false;
	m_logicOperator = "OR";
	m_recordDuration = QTime(1, 0, 0);
	m_symbolPath1 = "";
	m_symbolPath2 = "";

	m_workingDir = "";
	m_fileName = "";

	m_alarmPluginName = "None";
	p_alarmPlugin = NULL;

	p_xmlDoc = new XML();

	VideoDeviceManager::getInstance()->displayList();
	for (int i = 0; i < VideoDeviceManager::getInstance()->getNbDevice(); i++)
	{
		VideoDeviceManager::getInstance()->getDevice(i)->getColorToDetect().reinit();
	}

	// Init ROIs
	m_rois.push_back(new ROI());
	m_rois.push_back(new ROI());
	getROI(0)->setRecordDuration(m_recordDuration);
	getROI(1)->setRecordDuration(m_recordDuration);

	
	for (QVector<ROI*>::iterator itr = m_rois.begin(); itr != m_rois.end(); ++itr)
	{
		connect((*itr), SIGNAL(changed()), this, SLOT(sessionModified()));
		//connect((*itr), SIGNAL(alarmChanged()), this, SLOT(alarmModified()));
		connect(p_xmlDoc, SIGNAL(changed()), this, SLOT(sessionModified()));
	}

	// Thread
	p_process = new Process();

	// Alarm
	connect(p_process, SIGNAL(alarmBegin()), this, SLOT(setAlarmBegin()));
	connect(p_process, SIGNAL(alarmEnd()), this, SLOT(setAlarmEnd()));

	qDebug() << "SESSION: m_rois size: " << m_rois.size();


	qDebug() << "=== End Session constructor ===";
}

Session::~Session()
{
	if (p_xmlDoc != NULL)
	{
		delete p_xmlDoc;
		p_xmlDoc = NULL;
	}

	if (p_process != NULL)
	{
		p_process->stop();

		delete p_process;
		p_process = NULL;
	}

	m_alarmTab.clear();
	
	qDeleteAll(m_rois.begin(), m_rois.end());
	m_rois.clear();

	//for (int i = 0; i < m_rois.size(); i++)
	//{
	//	
	//	delete m_rois.at(i);
	//}
}

void Session::colorToDetectModified()
{
	for (int i = 0; i < m_rois.size(); i++)
	{
		getROI(i)->colorToDetectModified();
	}
}

ROI* Session::getROI(int index) const
{
	if (index >= 0 && index <= m_rois.size() - 1)
		return m_rois.at(index);
	else
		return NULL;
}

int Session::getNumberOfROI()
{
	return m_rois.size();
}

void Session::startDetection()
{
	if (getROI(0) && getROI(0)->getActive())
	{
		p_process->setROI1(getROI(0));
	}

	if (getROI(1) && getROI(1)->getActive())
	{
		p_process->setROI2(getROI(1));
	}

	p_process->start();

	emit changed(); // MRA -> Peut être emettre un autre signal que changed (detectionStart);
}

Process * Session::getProcess()
{
	return p_process;
}

void Session::stopDetection()
{
	p_process->stop();
}

bool Session::processIsRunning()
{
	return p_process->isRunning();
}

// slot
void Session::setAlarmBegin()
{
	qDebug() << "alarm begin in session";
	Alarm alarm = p_process->getAlarm();
	p_process->setAlarmReceived();
	
	m_alarmTab.push_back(alarm);

	// call selected alarm plugin 
	if (p_alarmPlugin != NULL)
	{
		p_alarmPlugin->beginAlarm(&alarm);
	}

	emit alarmChanged();
}

// slot
void Session::setAlarmEnd()
{
	qDebug() << "alarm end in session";

	Alarm alarm = p_process->getAlarm();
	p_process->setAlarmReceived();

	if (m_alarmTab.size() > 0)
		m_alarmTab.last() = alarm;

	// call selected alarm plugin 
	if (p_alarmPlugin != NULL)
	{
		p_alarmPlugin->endAlarm(&alarm);
	}

	emit alarmChanged();
}

Alarm& Session::getAlarmTab(int index)
{
	return m_alarmTab[index];
}

QVector<Alarm>& Session::getAlarmTab()
{
	return m_alarmTab;
}

XML * Session::getXML()
{
	return p_xmlDoc;
}

void Session::clearAlarmTab()
{
	qDebug() << "Session::clearAlarmTab";
	for (int i = 0; i < m_alarmTab.size(); i++)
	{
		if (!m_alarmTab.at(i).getImagePath1().isEmpty())
			QFile::remove(m_alarmTab.at(i).getImagePath1());
		if (!m_alarmTab.at(i).getImagePath2().isEmpty())
			QFile::remove(m_alarmTab.at(i).getImagePath2());
	}

	QString fileName = m_workingDir + "/" + m_fileName.remove(".xml") + "_alarm.xml";
	if (QFile::exists(fileName))
		QFile::remove(fileName);
	
	m_alarmTab.clear();
}

void Session::duplicateAlarmTabFiles(const QString& newDir, const QString& newSessionFilename)
{
	if (m_fileName.isEmpty())
		return;

	qDebug() << "Session::copyAlarmTab";
	QString currentSessionName = QFileInfo(m_fileName).baseName();
	QString newSessionName = QFileInfo(newSessionFilename).baseName();

	for (int i = 0; i < m_alarmTab.size(); i++)
	{
		Alarm a = m_alarmTab.at(i);
		QString image1Filename = a.getImagePath1();
		if (!image1Filename.isEmpty())
		{
			QString newImage1Filename = newDir + "/" + QFileInfo(image1Filename).fileName().replace(currentSessionName, newSessionName);
			QFile::copy(image1Filename, newImage1Filename);
			a.setImagePath1(newImage1Filename);
		}

		QString image2Filename = a.getImagePath2();
		if (!image2Filename.isEmpty())
		{
			QString newImage2Filename = newDir + "/" + QFileInfo(image2Filename).fileName().replace(currentSessionName, newSessionName);
			QFile::copy(image2Filename, newImage2Filename);
			a.setImagePath2(newImage2Filename);
		}
		m_alarmTab.replace(i, a);
	}
}

void Session::sessionModified()
{
	if (!getROI(0)->getActive())
	{
		getROI(1)->setActive(false);
	}
	
	setDirty(true);

	emit changed();
}

void Session::setDirty(bool val)
{
	m_dirty = val;
}

bool Session::getDirty()
{
	return m_dirty;
}

void Session::writeSessionFile()
{
	QString fileName = m_workingDir + "/" + m_fileName;
	p_xmlDoc->writeSessionFile(fileName, this);
}

bool Session::readSessionFile()
{
	QString fileName = m_workingDir + "/" + m_fileName;
	return p_xmlDoc->readSessionFile(fileName, this);
}

void Session::readAlarmFile()
{
	QString fileName = m_workingDir + "/" + m_fileName.remove(".xml") + "_alarm.xml";
	p_xmlDoc->readAlarmFile(fileName, m_alarmTab);
}

void Session::writeAlarmFile()
{
	if (m_results)
	{
		QString fileName = m_workingDir + "/" + m_fileName.remove(".xml") + "_alarm.xml";
		p_xmlDoc->writeAlarmFile(fileName, m_alarmTab);
	}
}

void Session::setAlarmPluginName(const QString & name)
{
	p_alarmPlugin = AlarmPluginManager::getInstance()->findPlugin(name);
	//if (name != "None")
	//	p_alarmPlugin->initPlugin();

	m_alarmPluginName = name;
	setDirty(true);
	emit titleChanged();
}

void Session::setLogic(bool value)
{
	if (value == 1)
		m_logicOperator = "AND";
	else
		m_logicOperator = "OR";
	setDirty(true);
	emit titleChanged();
}

void Session::setWriteResults(int value)
{
	m_results = value;
	setDirty(true);
	emit titleChanged();
}

void Session::setWriteImages(int value)
{
	m_images = value;
	p_process->setWriteImage(value);
	setDirty(true);
	emit titleChanged();
}

void Session::setRecordROI1(int value)
{
	getROI(0)->setRecordROI(value);
	setDirty(true);
	emit titleChanged();
}

void Session::setRecordROI2(int value)
{
	getROI(1)->setRecordROI(value);
	setDirty(true);
	emit titleChanged();
}

void Session::setRecordDuration(QTime value)
{
	m_recordDuration = value;
	getROI(0)->setRecordDuration(value);
	getROI(1)->setRecordDuration(value);
	setDirty(true);
	emit titleChanged();
}

void Session::setWorkingDir(QString dir)
{
	qDebug() << "Session::setWorkingDir: " << dir;
	m_workingDir = dir;
	p_process->setWorkingDir(dir);
	getROI(0)->setWorkingDir(dir);
	getROI(1)->setWorkingDir(dir);
}

QString Session::getWorkingDir()
{
	return m_workingDir;
}

void Session::setFileName(QString fileName)
{
	qDebug() << "Session::setFileName: " << m_fileName;
	m_fileName = fileName;
	p_process->setFileName(fileName);
}

QString Session::getFileName()
{
	return m_fileName;
}

AlarmPluginInterface* Session::getAlarmPlugin()
{
	return p_alarmPlugin;
}

QString	Session::getAlarmPluginName()
{
	return m_alarmPluginName;
}

QString Session::getLogic()
{
	return m_logicOperator;
}

int	Session::getWriteResults()
{
	return m_results;
}

int	Session::getWriteImages()
{
	return m_images;
}

int	Session::getRecordROI1()
{
	if (getROI(0))
		return getROI(0)->getRecordROI();
	else
		return 0;
}

int	Session::getRecordROI2()
{
	if (getROI(1))
		return getROI(1)->getRecordROI();
	else
		return 0;
}

QTime Session::getRecordDuration()
{
	return m_recordDuration;
}

QString Session::getSymbolPath1()
{
	return m_symbolPath1;
}

QString Session::getSymbolPath2()
{
	return m_symbolPath2;
}

void Session::setSymbolPath1(QString value)
{
	m_symbolPath1 = value;
}

void Session::setSymbolPath2(QString value)
{
	m_symbolPath2 = value;
}

QString Session::getSymbolPath(int indexRoi)
{
	if (indexRoi == 1)
		return m_symbolPath1;
	else
		return m_symbolPath2;
}

void Session::setSymbolPath(int indexRoi, QString value)
{
	if (indexRoi == 1)
		m_symbolPath1 = value;
	else
		m_symbolPath2 = value;
}