#include "Process.h"
#include <QDebug>
#include <VideoDeviceManager.h>
#include <QFileInfo>

Process::Process() :
m_stop(false), m_mutex(QMutex::Recursive)
{
	qDebug() << "Process: default constructor";
	ptr_roi1 = NULL;
	ptr_roi2 = NULL;

	p_timeAlarm = new QTime();
	p_timeThread = new QTime();
	alarmRunning = false;
	firstTime = true;
	logic = false;
	roi1Logicroi2 = Alarm::L_AND;
	m_avgTime = 0;
	m_workingDir = "";
	m_writeImage = false;	
}

Process::~Process()
{
	stop();

	if (p_timeAlarm)
		delete p_timeAlarm;
	if (p_timeThread)
		delete p_timeThread;
}

void Process::run()
{
	alarmRunning = false;
	firstTime = true;
	bool initRecord = true;

	// init sensitivity vectors
	m_lastRoi1Alarm = false;
	m_lastRoi2Alarm = false;

	ptr_roi1->setRepetitionCount(0);
	if (ptr_roi2)
		ptr_roi2->setRepetitionCount(0);

	while (!m_stop)
	{
		try
		{
			////////////////////////////////////////////////////////////
			// synchronize with video device
			if (ptr_roi1 && ptr_roi1->getActive())
			{
				// Synchro only on acquisition from roi1
				ptr_roi1->getVideoDevice()->getImageReady().acquire();
			}

			////////////////////////////////////////////////////////////
			// Launch ROI process threads & update video recording
			p_timeThread->restart();
			QString videoFileName = QFileInfo(m_fileName).baseName() + "_roi%1_" + QDateTime::currentDateTime().toString("yyMMdd_hhmm") + ".avi";
			if (ptr_roi1 && ptr_roi1->getActive())
			{
				ptr_roi1->retrieveImage(); // copy from the device
				if (ptr_roi1->getRecordROI())
					updateVideoRecord(m_video1, ptr_roi1, initRecord, videoFileName.arg(1));
				m_threadROI1.start();
			}

			if (ptr_roi2 && ptr_roi2->getActive())
			{
				ptr_roi2->retrieveImage();
				if (ptr_roi2->getRecordROI())
					updateVideoRecord(m_video2, ptr_roi2, initRecord, videoFileName.arg(2));
				m_threadROI2.start();
			}
			initRecord = false;

			// Waiting for thread to finish
			m_threadROI1.wait();
			m_threadROI2.wait();


			////////////////////////////////////////////////////////////
			// ==== Below is alarm handling (log, filtration, LED colors) ====
			// Signal sent to update the average time of the last 10 process
			m_arrayAvgTime.push_back(p_timeThread->elapsed());
			if (m_arrayAvgTime.size() == 10)
			{
				for (int i = 0; i < m_arrayAvgTime.size(); i++)
				{
					m_avgTime += m_arrayAvgTime.at(i);
				}
				m_avgTime /= m_arrayAvgTime.size();
				m_arrayAvgTime.clear();
				emit updateAvg();
			}

			////////////////////////////////////////////////////////////
			// handles ROI sensitivity
			bool filterOk = true;
			if (ptr_roi1 && ptr_roi1->getActive())
			{
				bool roiAlarm = ptr_roi1->getAlarm();
				if (ptr_roi1->getAlarmValid())
				{
					if (m_lastRoi1Alarm != roiAlarm)
					{
						ptr_roi1->setRepetitionCount(0);
					}
					else
					{
						ptr_roi1->setRepetitionCount(ptr_roi1->getRepetitionCount() + 1);
					}
					if (ptr_roi1->getRepetitionCount() < ptr_roi1->getFiltration())
						filterOk = false;
					m_lastRoi1Alarm = roiAlarm;
				}
				else
				{
					filterOk = false;
				}
			}
			if (ptr_roi2 && ptr_roi2->getActive())
			{
				bool roiAlarm = ptr_roi2->getAlarm();
				if (ptr_roi2->getAlarmValid())
				{
					if (m_lastRoi2Alarm != roiAlarm)
					{
						ptr_roi2->setRepetitionCount(0);
					}
					else
					{
						ptr_roi2->setRepetitionCount(ptr_roi2->getRepetitionCount() + 1);
					}
					if (ptr_roi2->getRepetitionCount() < ptr_roi2->getFiltration())
						filterOk = false;
					m_lastRoi2Alarm = roiAlarm;
				}
				else
				{
					filterOk = false;
				}
			}


			////////////////////////////////////////////////////////////
			// test logic expression and get alarm result
			if (filterOk)
			{
				if (ptr_roi1 && ptr_roi1->getActive() && (ptr_roi2 == NULL || !ptr_roi2->getActive()))
				{
					logic = ptr_roi1->getAlarm();
				}
				else if (ptr_roi2 && ptr_roi2->getActive())
				{
					if (roi1Logicroi2 == Alarm::L_AND)
					{
						logic = (ptr_roi1->getAlarm() && ptr_roi2->getAlarm());
					}
					else if (roi1Logicroi2 == Alarm::L_OR)
					{
						logic = (ptr_roi1->getAlarm() || ptr_roi2->getAlarm());
					}
				}
				handleAlarm();
			}

			////////////////////////////////////////////////////////////
			// release ROI images
			if (ptr_roi1 && ptr_roi1->getActive())
				ptr_roi1->getImage().release();
			if (ptr_roi2 && ptr_roi2->getActive())
				ptr_roi2->getImage().release();

		}
		catch (Exception e)
		{
			qDebug() << QString("Exception in process : %1").arg(QString::fromStdString(e.msg));
		}
	}
}

void Process::updateVideoRecord(ROIVideoWriter& video, ROI *pROI, bool initRecord, const QString& fileName)
{
	if (initRecord)
	{
		// Initialize the video recording
		video.setMaxDuration(-pROI->getRecordDuration().secsTo(QTime(0, 0)));
		video.start(m_workingDir + "/" + fileName, pROI->getImage());
	}
	else
	{
		Mat temp;
		pROI->getImage(temp);
		video.setImage(temp);
		temp.release();
	}
}

void Process::handleAlarm()
{
	QString startTime = "";

	// Case where the detection starts by an alarm
	if (logic && firstTime)
	{
		firstTime = false;
		newTimeAlarm();
		alarmRunning = true;
	}
	
	// Case where the detection sees an alarm (creates new alarm)
	if (logic && alarmRunning)
	{
		qDebug() << "Begin alarm";
		// Start a new alarm 
		alarmRunning = false;
		p_timeAlarm->start();

		if (m_alarm.getStatus() == "Running")
		{
			// Force end of previous alarm
			QString duration = QString::number(p_timeAlarm->elapsed() / 1000);
			m_alarm.setDuration(duration);
			setStatusString(Alarm::AS_COMPLETED);

			emit alarmEnd();
			emit alarmChanged();
			// Synchro on alarm acquisition
			m_waitForAlarmReceived.acquire();

		}
		m_alarm.reset();

		QDateTime t;
		QDate currentDate = t.date().currentDate();
		QString date = currentDate.toString("dd/MM/yyyy");
		startTime = t.time().currentTime().toString();

		setLogicString(roi1Logicroi2);
		setStatusString(Alarm::AS_RUNNING);
		m_alarm.setDate(date);
		m_alarm.setStartTime(startTime);


		//background color (QTableWidget purpose)
		if (ptr_roi1)
		{
			setTypeROIString(1, ptr_roi1);
			m_alarm.setCurrent1(ptr_roi1->getCurrent());
			m_alarm.setTarget1(ptr_roi1->getTarget());
			if (ptr_roi1->getAlarm())
				m_alarm.setBackgroundColor1("#dd8888");
			else
				m_alarm.setBackgroundColor1("#88dd88");

		}
		if (ptr_roi2 && ptr_roi2->getActive())
		{
			setTypeROIString(2, ptr_roi2);
			m_alarm.setCurrent2(ptr_roi2->getCurrent());
			m_alarm.setTarget2(ptr_roi2->getTarget());
			if (ptr_roi2->getAlarm())
				m_alarm.setBackgroundColor2("#dd8888");
			else
				m_alarm.setBackgroundColor2("#88dd88");		
		}
		
		// Save the image as "roi_number_dateTime.png"
		QString baseFileName = QFileInfo(m_fileName).baseName() + "_roi%1_" + QDateTime::currentDateTime().toString("yyMMdd_hhmmsszzz") + ".png";
		if (ptr_roi1 && m_writeImage)
		{
			cv::Mat image1;
			QString fileName = baseFileName.arg(1);
			qDebug() << "filname: " << fileName;
			cv::cvtColor(ptr_roi1->getImage(), image1, CV_BGR2RGB);
			cv::imwrite(m_workingDir.toStdString() + "/" + fileName.toStdString(), image1);
			m_alarm.setImagePath1(m_workingDir + "/" + fileName);
			image1.release();
		}
		if (ptr_roi2 && m_writeImage && ptr_roi2->getActive())
		{
			cv::Mat image2;
			QString fileName = baseFileName.arg(2);
			cv::cvtColor(ptr_roi2->getImage(), image2, CV_BGR2RGB);
			cv::imwrite(m_workingDir.toStdString() + "/" + fileName.toStdString(), image2);			
			m_alarm.setImagePath2(m_workingDir + "/" + fileName);
			image2.release();
		}
	
		emit alarmBegin();
		emit alarmChanged();
		// Synchro on alarm acquisition
		m_waitForAlarmReceived.acquire();
		qDebug() << "waitForAlarmReceived";
	}
	// Case where the detection stops seeing an alarm (ends the creation of the alarm)
	else if (!logic && !alarmRunning)
	{

		// End current alarm
		alarmRunning = true;
		
		if (!firstTime)
		{
			qDebug() << "End alarm";

			QString duration = QString::number(p_timeAlarm->elapsed()/1000);
			m_alarm.setDuration(duration);
			setStatusString(Alarm::AS_COMPLETED);

			emit alarmEnd();
			emit alarmChanged();			 
			// Synchro on alarm acquisition
			m_waitForAlarmReceived.acquire();
			qDebug() << "waitForAlarmReceived";
		}
		firstTime = false;
		newTimeAlarm();
	}
}

void Process::setLogicString(int value)
{
	QMutexLocker mutexLocker(&m_mutex);
	switch (value)
	{
	case 0:
		m_alarm.setLogic("And");
		break;
	default:
		m_alarm.setLogic("Or");
		break;
	}
}

void Process::setStatusString(int status)
{
	QMutexLocker mutexLocker(&m_mutex);
	switch (status)
	{
	case 0:
		m_alarm.setStatus("Running");
		break;
	default:
		m_alarm.setStatus("Completed");
		break;
	}
}

void Process::setTypeROIString(int numROI, ROI * ptr_roi)
{
	QMutexLocker mutexLocker(&m_mutex);
	int type = ptr_roi->getCurrentDetectionType();

	switch (type)
	{
	case 0:
		m_alarm.setType(numROI, "Undefined", ptr_roi->getAlarmOnEquals());
		break;
	case 1:
		m_alarm.setType(numROI, "Color", ptr_roi->getAlarmOnEquals());
		break;
	case 2:
		m_alarm.setType(numROI, "Symbol", ptr_roi->getAlarmOnEquals());
		break;
	case 3:
		m_alarm.setType(numROI, "Text", ptr_roi->getAlarmOnEquals());
		break;
	default:
		m_alarm.setType(numROI, "Number", ptr_roi->getAlarmOnEquals());
		break;
	}
}

void Process::setAlarmRunning(bool val)
{
	QMutexLocker mutexLocker(&m_mutex);
	alarmRunning = val;
}

void Process::setRoi1Logicroi2(Alarm::logic l)
{
	QMutexLocker mutexLocker(&m_mutex);
	roi1Logicroi2 = l;
}

bool Process::getLogic()
{
	QMutexLocker mutexLocker(&m_mutex);
	return logic;
}

void Process::newTimeAlarm()
{
	p_timeAlarm->restart();
}

QTime * Process::getTimeAlarm()
{
	QMutexLocker mutexLocker(&m_mutex);
	return p_timeAlarm;
}

Alarm& Process::getAlarm()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_alarm;
}

bool Process::getAlarmRunning()
{
	QMutexLocker mutexLocker(&m_mutex);
	return alarmRunning;
}

void Process::reset()
{
	logic = false;
}

void Process::start()
{

	// reset semaphore
	qDebug() << "start process";
	m_waitForAlarmReceived.tryAcquire(m_waitForAlarmReceived.available());

	qDebug() << "start process";
	//QMutexLocker mutexLocker(&m_mutex);
	if ((!m_threadROI1.isRunning()) && (!m_threadROI2.isRunning()))
	{
		m_stop = false;
		QThread::start();
	}
}

void Process::stop()
{
	if (m_alarm.getStatus() == "Running")
	{
		QString duration = QString::number(p_timeAlarm->elapsed() / 1000);
		m_alarm.setDuration(duration);
		setStatusString(Alarm::AS_COMPLETED);
		emit alarmEnd();
		emit alarmChanged();
	}

	//QMutexLocker mutexLocker(&m_mutex);
	m_stop = true;

	// release image acquisition
	if (ptr_roi1 && ptr_roi1->getActive())
	{
		// Synchro only on acquisition from roi1
		ptr_roi1->getVideoDevice()->getImageReady().release();
	}
	setAlarmReceived();

	quit();
	wait();

	m_video1.stop();
	m_video2.stop();
}

void Process::setROI1(ROI * roi)
{
	QMutexLocker mutexLocker(&m_mutex);
	ptr_roi1 = roi;
	m_threadROI1.setROI(ptr_roi1);
}

void Process::setROI2(ROI * roi)
{
	QMutexLocker mutexLocker(&m_mutex);
	ptr_roi2 = roi;
	m_threadROI2.setROI(ptr_roi2);
}

int Process::getAvgTime()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_avgTime;
}

void Process::setWorkingDir(QString path)
{
	QMutexLocker mutexLocker(&m_mutex);
	m_workingDir = path;
}

void Process::setFileName(QString path)
{
	QMutexLocker mutexLocker(&m_mutex);
	m_fileName = path;
}

QString	Process::getWorkingDir()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_workingDir;
}

const ROIVideoWriter& Process::getVideoRecorder(int roi) const
{
	if (roi == 0)
		return m_video1;
	else
		return m_video2;
}

void Process::setWriteImage(int val)
{
	QMutexLocker mutexLocker(&m_mutex);
	m_writeImage = val;
}

void Process::setAlarmReceived()
{
	if (m_waitForAlarmReceived.available() == 0)
		m_waitForAlarmReceived.release();
	qDebug() << "waitForAlarmReceived free";
}