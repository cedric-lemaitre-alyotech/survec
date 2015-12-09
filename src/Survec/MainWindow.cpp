#include "MainWindow.h"
#include "Session.h"
#include "CustomRectItem.h"
#include "Scene.h"
#include "XML.h"
#include <Tools.h>
#include <Process.h>
#include <ColorToDetect.h>
#include <cwDisplayAlarmImg.h>
#include <AlarmPluginManager.h>
#include <XML.h>

#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QTime>
#include <QDateTime>
#include <QDebug>
#include <QDesktopServices>
#include <QMessageBox>
#include <QProcess>
#include <QCloseEvent>
#include <QScrollBar>

MainWindow::MainWindow(QSettings* settings, QWidget * parent, Qt::WindowFlags flags)
: QMainWindow(parent, flags)
, p_settings(settings)
, p_session(NULL)
{
	m_ui.setupUi(this);

	showMaximized();

	// Init the cwVideoPlayer
	m_ui.videoPlayer1->getCwView()->setModeClick(1);
	m_ui.videoPlayer2->getCwView()->setModeClick(1);

	m_ui.videoPlayer1->setReadFullPixmap(false);
	m_ui.videoPlayer2->setReadFullPixmap(false);

	m_ui.videoPlayer1->setButtonVisible(false);
	m_ui.videoPlayer2->setButtonVisible(false);

	// Reads the initial picture
	p_initPixmap = new QPixmap(m_ui.videoPlayer1->getCwView()->getScene()->getItemPixmap()->pixmap());
	qpixmap1 = p_initPixmap;

	// Init working fileName and directory
	m_fileName = "";
	m_workingDir = "";

	// Do not draw any rectangle on the videoPlayer
	m_ui.videoPlayer1->getCwView()->getScene()->setRectangleVisible(false);
	m_ui.videoPlayer2->getCwView()->getScene()->setRectangleVisible(false);
		
	// QTime (for timer)
	p_time = new QTime();
	m_nbDays = 0;

	connect(m_ui.cw_ROIResult1,			SIGNAL(roiStateChanged()),	this,	SLOT(updateActiveROI()) );
	connect(m_ui.cw_ROIResult2,			SIGNAL(roiStateChanged()),	this,	SLOT(updateActiveROI()) );
	connect(m_ui.cw_ROIResult1,			SIGNAL(alarmChanged()),		this,	SLOT(updateRoiAlarm()));
	connect(m_ui.cw_ROIResult2,			SIGNAL(alarmChanged()),		this,	SLOT(updateRoiAlarm()));

	connect(m_ui.cw_ROIResult1,			SIGNAL(changed()),			this,	SLOT(updateROIResult())	);
	connect(m_ui.cw_ROIResult2,			SIGNAL(changed()),			this,	SLOT(updateROIResult())	);

	// create new session
	actionNew();

	m_ui.cw_ROIResult1->setROI(p_session->getROI(0), 1);
	m_ui.cw_ROIResult2->setROI(p_session->getROI(1), 2);

	// Init start process
	isProcessStarted = false;
			
	// == Connect ==
	// Open dialog
	connect(m_ui.actionEditColors,		SIGNAL(triggered()),				SLOT(actionEditColor())		);

	// Action buttons
	connect(m_ui.actionNew,				SIGNAL(triggered()),				SLOT(actionNew())			);
	connect(m_ui.actionOpen,			SIGNAL(triggered()),				SLOT(actionOpen())			);
	connect(m_ui.actionSave,			SIGNAL(triggered()),				SLOT(actionSave())			);
	connect(m_ui.actionSaveAs,			SIGNAL(triggered()),				SLOT(actionSaveAs())		);
	connect(m_ui.actionHelp,			SIGNAL(triggered()),				SLOT(actionHelp())			);
	connect(m_ui.actionInfo,			SIGNAL(triggered()),				SLOT(actionInfo())			);
	connect(m_ui.actionAlarm,			SIGNAL(triggered()),				SLOT(actionAlarmLog())		);
	connect(m_ui.actionExit,			SIGNAL(triggered()),				SLOT(actionExit())			);
	connect(m_ui.actionEnglish,			SIGNAL(triggered()),				SLOT(actionEnglish()));
	connect(m_ui.actionFrench,			SIGNAL(triggered()),				SLOT(actionFrench()));

	// Update during detection
	p_timerDisplay = new QTimer();
	p_timerDisplay->setInterval(50);
	p_timerDisplay->start();
	// UpdateGUI every 40ms
	connect(p_timerDisplay,				SIGNAL(timeout()),			this,	SLOT(updateDisplayTime()));

	connect(m_ui.cw_ROIResult1,			SIGNAL(stopPlaying(bool)), this,	SLOT(pauseVideo(bool)));
	connect(m_ui.cw_ROIResult2,			SIGNAL(stopPlaying(bool)), this,	SLOT(pauseVideo(bool)));
	
	// Update both webcam feed
	connect(m_ui.cw_ROIResult1,			SIGNAL(webcamChanged()), this,		SLOT(updateWebcamFeedROI1()));
	connect(m_ui.cw_ROIResult2,			SIGNAL(webcamChanged()), this,		SLOT(updateWebcamFeedROI2()));
	
	connect(m_ui.pbResetHisto,			SIGNAL(clicked()),					SLOT(resetHisto())			);
	connect(m_ui.pbOpenDir,				SIGNAL(clicked()),					SLOT(openDir())				);
	connect(m_ui.tbStartProcess,		SIGNAL(clicked()),					SLOT(startProcess())		);

	// FPS display
	connect(VideoDeviceManager::getInstance()->getDevice(0), SIGNAL(frameRate(int)), this,	SLOT(changeFPS(int)));

	// Double clic on tabwidget to display image
	connect(m_ui.tableWidget,			SIGNAL(cellDoubleClicked(int, int)), this,			SLOT(displayImage(int, int)));

	// Calls a one shot timer that calls fitWebcam() to properly display the view once its content is loaded
	timerFitView();
	
	m_ui.videoPlayer1->setEnabled(true);
	m_ui.videoPlayer2->setEnabled(true);
	
	m_ui.cw_ROIResult1->getGroupBox()->setTitle("ROI 1");
	m_ui.cw_ROIResult2->getGroupBox()->setTitle("ROI 2");

	// load plugins
	AlarmPluginManager::getInstance()->loadPlugins();
	m_ui.comboBoxOutput->blockSignals(true);
	m_ui.comboBoxOutput->clear();
	m_ui.comboBoxOutput->addItem(tr("None"));
	foreach(AlarmPluginInterface* plugin, AlarmPluginManager::getInstance()->getPlugins())
	{
		m_ui.comboBoxOutput->addItem(plugin->name());
	}
	if (m_ui.comboBoxOutput->count() > 0)
	{
		m_ui.comboBoxOutput->setCurrentIndex(0);
		p_session->setAlarmPluginName(m_ui.comboBoxOutput->currentText());
	}
	m_ui.comboBoxOutput->blockSignals(false);

	m_ui.lbTime->setText("00:00:00");
	m_ui.lbAvgTime->setText("");
	m_ui.lbVideoRecording->setText(tr("Video Recording"));
	m_ui.lbVideoRecording->setVisible(false);

	p_session->setDirty(false);

	updateGUI();


	// Traduction des messages de la lib
	XML::ERROR_WEBCAM_CONNEXION = tr("Error : missing webcam - %1 detected and %2 needed by the session");
	XML::ERROR_FILE_FORMAT = tr("Wrong XML format");
	XML::ERROR_FILE_LOADING = tr("Error file loading");

}

MainWindow::~MainWindow()
{
	qDebug() << "MAINWINDOW DESTRUCTOR";

	m_ui.videoPlayer1->detachVideoDevice();
	m_ui.videoPlayer2->detachVideoDevice();

	if (p_initPixmap)
	{
		delete p_initPixmap;
		p_initPixmap = NULL;
	}
	
	if (p_time)
	{
		delete p_time;
		p_time = NULL;
	}
	
	if (p_timerDisplay)
	{
		delete p_timerDisplay;
		p_timerDisplay = NULL;
	}
	
	if (p_session)
	{
		delete p_session;
		p_session = NULL;
	}

	//if (p_alarmImgDialog)
	//{
	//	delete p_alarmImgDialog;
	//	p_alarmImgDialog = NULL;
	//}
	
	VideoDeviceManager::resetInstance();
	AlarmPluginManager::destroyInstance();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
	if (p_session != NULL && p_session->getDirty())
	{
		QMessageBox msgBox(this);
		msgBox.setWindowTitle("Survec");
		msgBox.setText(tr("Do you want to save before closing ?"));
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Close | QMessageBox::Cancel);
		msgBox.setDefaultButton(QMessageBox::Close);
		int result = msgBox.exec();

		switch (result)
		{
		case QMessageBox::Save:
			if (isProcessStarted)
			{
				startProcess();
			}
			actionSave();
			close();
			break;
		case QMessageBox::Close:
			if (isProcessStarted)
			{
				startProcess();
			}
			close();
			break;
		default:
			event->ignore();
			break;
		}
	}
}

void MainWindow::timerFitView()
{
	// Full screen display of webcam
	QTimer::singleShot(100, this, SLOT(fitWebcam()));
}

void MainWindow::fitWebcam()
{
	m_ui.videoPlayer1->fitView();
	m_ui.videoPlayer2->fitView();
}

void MainWindow::setActionEnable(bool value)
{
	m_ui.actionNew->setEnabled(value);
	m_ui.actionOpen->setEnabled(value);
	m_ui.actionSave->setEnabled(value);
	m_ui.actionSaveAs->setEnabled(value);
	m_ui.actionEditColors->setEnabled(value);
}

void MainWindow::displayAlarms()
{
	int startIndex = m_ui.tableWidget->rowCount() - 1;
	if (startIndex < 0) startIndex = 0;

	startIndex = 0;

	// == Save alarms values in tabAlarm
	QVector<QString> colorList;
	QVector<QVector<QString>> tabAlarm;

	for (int i = startIndex; i < p_session->getAlarmTab().size(); i++)
	{
		QVector<QString> rowData;
		for (int j = 0; j < NB_ALARM_COLS; j++)
		{
			QString temp(p_session->getAlarmTab(i).getValueAt(j));
			if (j == 3)
			{
				if (temp == "Running")
				{
					colorList.push_back("#dddd88");
				}
				else if (temp == "Completed")
				{
					colorList.push_back("#88dddd");
				}
			}

			if (j == 4 || j == 5 || j == 6 || j == 8 || j == 9 || j == 10)
			{
				if (j == 4)
				{
					colorList.push_back(p_session->getAlarmTab(i).getBackgroundColor1());
				}
				else if (j == 5)
				{
					if (p_session->getROI(0)->getCurrentDetectionType() == ROI::COLOR_DT)
						colorList.push_back(p_session->getAlarmTab(i).getCurrent(1));
					else
						colorList.push_back(QString(p_session->getAlarmTab(i).getBackgroundColor1()));
				}
				else if (j == 6)
				{
					if (p_session->getROI(0)->getCurrentDetectionType() == ROI::COLOR_DT)
						colorList.push_back(p_session->getAlarmTab(i).getTarget(1));
					else
						colorList.push_back(QString(p_session->getAlarmTab(i).getBackgroundColor1()));
				}
				else if (j == 8)
				{	
					colorList.push_back(p_session->getAlarmTab(i).getBackgroundColor2());
				}
				else if (j == 9)
				{
					if (p_session->getROI(1)->getCurrentDetectionType() == ROI::COLOR_DT)
						colorList.push_back(p_session->getAlarmTab(i).getCurrent(2));
					else
						colorList.push_back(QString(p_session->getAlarmTab(i).getBackgroundColor2()));
				}
				else if (j == 10)
				{
					if (p_session->getROI(1)->getCurrentDetectionType() == ROI::COLOR_DT)
						colorList.push_back(p_session->getAlarmTab(i).getTarget(2));
					else
						colorList.push_back(QString(p_session->getAlarmTab(i).getBackgroundColor2()));
				}
										
				if (temp.startsWith("#"))
				{
					rowData.push_back("");
				}
				else
				{
					rowData.push_back(temp);
				}
					
			}
			else
			{
				rowData.push_back(temp);
			}
		}
		tabAlarm.push_back(rowData);
	}
	
	int max_slider_pos = m_ui.tableWidget->verticalScrollBar()->maximum();
	bool move_slider_to_bottom = false;

	if (m_ui.tableWidget->verticalScrollBar()->sliderPosition() == max_slider_pos)
	{
		move_slider_to_bottom = true;
	}

	// == Uses tabAlarm to place values in the qtablewidget
	m_ui.tableWidget->setRowCount(p_session->getAlarmTab().size());
	int k = 0;
	for (int i = 0; i < tabAlarm.size(); i++)
	{
		for (int j = 0; j < tabAlarm[i].size(); j++)
		{
			//qDebug() << "i :" << i << ":" << *tabAlarm[i][j];
			m_ui.tableWidget->setItem(startIndex + i, j, new QTableWidgetItem(tabAlarm[i][j]));

			if (j == 3 || j == 4 || j == 5 || j == 6 || j == 8 || j == 9 || j == 10)
			{
				if (colorList.size() > k)
				{
					m_ui.tableWidget->item(startIndex + i, j)->setBackgroundColor(QColor(colorList.at(k)));
					k++;
				}
			}
		}
	}

	if (move_slider_to_bottom)
	{
		m_ui.tableWidget->scrollToBottom();
	}

}

// slot
void MainWindow::displayImage(int row, int col)
{
	if (p_session->getWriteImages())
	{
		qDebug() << "row: " << row << "/" << " col: " << col;
		//cwDisplayAlarmImg dialog(this);
		p_alarmImgDialog = new cwDisplayAlarmImg(this);

		p_alarmImgDialog->setWindowTitle("Alarm " + QString::number(row+1));
		p_alarmImgDialog->setModal(Qt::NonModal);
		QString symbolBaseFileName = m_workingDir + "/" + QFileInfo(m_fileName).baseName() + "_symbol_roi%1.png";
		if (!p_session->getAlarmTab().at(row).getImagePath1().isEmpty())
		{
			//Mat img1, symbol1;
			QString fileName1 = p_session->getAlarmTab().at(row).getImagePath1().section('/', -1);
		
			p_alarmImgDialog->setImgROI1(p_session->getAlarmTab().at(row).getImagePath1());
			p_alarmImgDialog->setImgSymbol1(symbolBaseFileName.arg(1));
			p_alarmImgDialog->setTitleROI1(fileName1);
			p_alarmImgDialog->setTitleSymbol1("Symbol ROI1");
		}
		if (!p_session->getAlarmTab().at(row).getImagePath2().isEmpty())
		{
			//Mat img2, symbol2;
			QString fileName2 = p_session->getAlarmTab().at(row).getImagePath2().section('/', -1);

			p_alarmImgDialog->setImgROI2(p_session->getAlarmTab().at(row).getImagePath2());
			p_alarmImgDialog->setImgSymbol2(symbolBaseFileName.arg(2));
			p_alarmImgDialog->setTitleROI2(fileName2);
			p_alarmImgDialog->setTitleSymbol2("Symbol ROI2");
		}

		p_alarmImgDialog->setContent();
		p_alarmImgDialog->exec();
	}
}

// slot
void MainWindow::updateROIResult()
{
	qDebug() << ">INFO: MainWindow::updateROIResult";
	int bounds[4];

	m_ui.videoPlayer1->setReadFullPixmap(false);
	m_ui.videoPlayer2->setReadFullPixmap(false);
	
	//----------------------
	// Update cwROIResult1
	//----------------------
	m_ui.cw_ROIResult1->setROI(p_session->getROI(0), 1);

	// Update gbROI1
	p_session->getROI(0)->getBounds(bounds);	
	
	m_ui.videoPlayer1->setBounds(bounds[0], bounds[1], bounds[2], bounds[3]);

	//----------------------
	// Update cwROIResult2
	//----------------------
	m_ui.cw_ROIResult2->setROI(p_session->getROI(1), 2);
	// Update gbROI2
	p_session->getROI(1)->getBounds(bounds);

	m_ui.videoPlayer2->setBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
}

void MainWindow::cwROIResultEnableCurrent(bool val)
{
	// Color of currentValue changes in cwROIResult
	// cwROIResult1
	m_ui.cw_ROIResult1->getValueCurrent()->setEnabled(val);
	m_ui.cw_ROIResult1->getLabelOperator()->setEnabled(val);
	m_ui.cw_ROIResult1->getLabelCurrent()->setEnabled(val);

	// cwROIResult2
	m_ui.cw_ROIResult2->getValueCurrent()->setEnabled(val);
	m_ui.cw_ROIResult2->getLabelOperator()->setEnabled(val);
	m_ui.cw_ROIResult2->getLabelCurrent()->setEnabled(val);
}

// slot
void MainWindow::displayAvgTime()
{
	// Display average time
	m_ui.lbAvgTime->setText(tr("Avg time (ms): ") + QString::number(p_session->getProcess()->getAvgTime()));
}

void MainWindow::updateTitle()
{
	QString title = QString("Survec v%1 - ").arg(SURVEC_VERSION);
	if (p_session)
	{
		title += (m_workingDir + "/" + m_fileName);
		if (p_session->getDirty())
		{
			title += "*";
		}
		if (m_fileName == "")
		{
			title = QString("Survec v%1").arg(SURVEC_VERSION);
		}
	}
	setWindowTitle(title);
}

// slot
void MainWindow::updateGUI()
{
	qDebug() << "MainWindow::updateGUI()";
	updateTitle();

	// Update cwROIResult
	updateROIResult();
		
	bool activeROI1 = p_session->getROI(0)->getActive();
	bool activeROI2 = p_session->getROI(1)->getActive();
	
	/****************************
	*			ROI 1
	*****************************/
	// Checks ROI 1 if active
	m_ui.cbRecord1->setEnabled(activeROI1);
	m_ui.dteMaxDuration->setEnabled(activeROI1);
	m_ui.cw_ROIResult1->getAlarmLabel()->setEnabled(activeROI1);
	if (!isProcessStarted && activeROI1)
		m_ui.cw_ROIResult1->getROIConfigPushButton()->setEnabled(true);
	else if (isProcessStarted)
		m_ui.cw_ROIResult1->getROIConfigPushButton()->setEnabled(false);

	if (!isProcessStarted && activeROI1 && p_session->getROI(0)->getCurrentDetectionType() != ROI::UNDEFINED_DT)
		m_ui.cw_ROIResult1->getPbTest()->setEnabled(true);
	else
		m_ui.cw_ROIResult1->getPbTest()->setEnabled(false);


	m_ui.cw_ROIResult1->getPbScreenshot()->setEnabled(activeROI1 && !isProcessStarted);

//	m_ui.cw_ROIResult1->enableAllContent(activeROI1);

	/****************************
	*			ROI 2
	*****************************/
	m_ui.cw_ROIResult2->setEnabled(activeROI1);			// allow to use ROI2 only if ROI1 is already active
	// Checks ROI 2 if active
	m_ui.gbVideoROI2->setVisible(activeROI2);			// set the video visible

	m_ui.cbRecord2->setEnabled(activeROI2);
	m_ui.cw_ROIResult2->getAlarmLabel()->setEnabled(activeROI2);
	//m_ui.cw_ROIResult2->enableAllContent(activeROI2);

	if (!isProcessStarted)
	{
		m_ui.cw_ROIResult2->getGroupBox()->setEnabled(true);
		m_ui.cw_ROIResult2->getROIConfigPushButton()->setEnabled(activeROI2);
	}
	if (isProcessStarted)
	{
		m_ui.cw_ROIResult2->getROIConfigPushButton()->setEnabled(false);
		if (!activeROI2)
			m_ui.cw_ROIResult2->getGroupBox()->setEnabled(false);
	}
	
	if (!isProcessStarted && activeROI2 && p_session->getROI(0)->getCurrentDetectionType() != ROI::UNDEFINED_DT)
		m_ui.cw_ROIResult2->getPbTest()->setEnabled(true);
	else
		m_ui.cw_ROIResult2->getPbTest()->setEnabled(false);

	m_ui.cw_ROIResult2->getPbScreenshot()->setEnabled(activeROI2 && !isProcessStarted);

	m_ui.videoPlayer2->pause(activeROI2);				// pause videoPlayer2 if ROI2 is not active

	/****************************
	*		ROI 1 && || 2
	*****************************/
	// Logical combination groupBox
	// If both ROI are active and different from Undefined_DT, allow the user to pick a logical combination (AND or OR)
	if (activeROI1 && activeROI2 && p_session->getROI(0)->getCurrentDetectionType() != ROI::UNDEFINED_DT && p_session->getROI(1)->getCurrentDetectionType() != ROI::UNDEFINED_DT)
	{
		m_ui.gbResultOp->setVisible(true);
		//m_ui.gbResultOp->setEnabled(true);
	}
	else
	{
		m_ui.gbResultOp->setVisible(false);
		//m_ui.gbResultOp->setEnabled(false);
	}

	// Clear alarms button
	m_ui.pbResetHisto->setEnabled(!isProcessStarted);

	/*
	***** END ROI ******/


	
	/***************************************************
	*		Group boxes (update from opening XML file)
	***************************************************/
	// Alarm log group box (write result / write images)
	m_ui.checkBoxWriteImages->setChecked(p_session->getWriteImages());
	m_ui.checkBoxWriteResults->setChecked(p_session->getWriteResults());

	// Output interface groupbox (RS232)
	m_ui.comboBoxOutput->setCurrentText(p_session->getAlarmPluginName());

	// Logical combination groupBox (AND, OR)
	m_ui.radioButtonOR->blockSignals(true);
	m_ui.radioButtonAND->blockSignals(true);
	if (p_session->getLogic() == "AND")
		m_ui.radioButtonAND->setChecked(true);
	else
		m_ui.radioButtonOR->setChecked(true);
	m_ui.radioButtonOR->blockSignals(false);
	m_ui.radioButtonAND->blockSignals(false);

	// Video recording groupbox (ROI1, ROI2, maximum duration)
	m_ui.cbRecord1->blockSignals(true);
	m_ui.cbRecord2->blockSignals(true);
	m_ui.dteMaxDuration->blockSignals(true);
	m_ui.cbRecord1->setChecked(p_session->getRecordROI1());
	m_ui.cbRecord2->setChecked(p_session->getRecordROI2());
	m_ui.dteMaxDuration->setTime(p_session->getRecordDuration());
	m_ui.cbRecord1->blockSignals(false);
	m_ui.cbRecord2->blockSignals(false);
	m_ui.dteMaxDuration->blockSignals(false);


	/********************************************
	*  Alarm tab (update from opening XML file)
	********************************************/
	if (isProcessStarted)
		displayAlarms();

	// Fit view to scene
	timerFitView();
}

// slot
void MainWindow::updateWebcamFeedROI1()
{
	if (p_session->getROI(0) && p_session->getROI(0)->getActive())
	{
		int deviceId1 = p_session->getROI(0)->getVideoDeviceId();
		int deviceId2 = p_session->getROI(1)->getVideoDeviceId();
		m_ui.videoPlayer1->setDeviceId(deviceId1);
		int feedId = VideoDeviceManager::getInstance()->getDevice(deviceId1)->getId();

		m_ui.videoPlayer1->setFeedId(feedId);		
		m_ui.videoPlayer1->readWebcamFeed();

		// Fit the view
		int bounds[4];
		p_session->getROI(0)->getBounds(bounds);
		m_ui.videoPlayer1->setBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
		m_ui.videoPlayer1->updateGUI();
		m_ui.videoPlayer1->fitView();
	}
}

// slot
void MainWindow::updateWebcamFeedROI2()
{	
	if (p_session->getROI(1) && p_session->getROI(1)->getActive())
	{
		int deviceId1 = p_session->getROI(0)->getVideoDeviceId();
		int deviceId2 = p_session->getROI(1)->getVideoDeviceId();
		m_ui.videoPlayer2->setDeviceId(deviceId2);
		int feedId = VideoDeviceManager::getInstance()->getDevice(deviceId2)->getId();

		m_ui.videoPlayer2->setFeedId(feedId);		
		m_ui.videoPlayer2->readWebcamFeed();
		
		// Fit the view
		int bounds[4];
		p_session->getROI(1)->getBounds(bounds);
		m_ui.videoPlayer2->setBounds(bounds[0], bounds[1], bounds[2], bounds[3]);
		m_ui.videoPlayer2->updateGUI();
		m_ui.videoPlayer2->fitView();

		// Closes the device if not being used
		for (int i = 0; i < VideoDeviceManager::getInstance()->getNbDevice(); i++)
		{
			int id = VideoDeviceManager::getInstance()->getDevice(i)->getId();
			if (id != deviceId1 && id != deviceId2)
			{
				VideoDevice * oldDevice = VideoDeviceManager::getInstance()->getDevice(i);
				oldDevice->getCap().open(id);
			}
		}
	}
}

// slot
void MainWindow::updateDisplayTime()
{
	// Starts timer when process starts
	if (!p_time->isNull() && isProcessStarted)
	{

		int secs = p_time->elapsed() / 1000;
		int mins = (secs / 60) % 60;
		int hours = (secs / 3600) + m_nbDays * 24;
		if (p_time->elapsed() < m_lastTime)
			m_nbDays++;
		m_lastTime = p_time->elapsed() -20;
		
		if (p_session->getProcess()->getVideoRecorder(0).isActive() || p_session->getProcess()->getVideoRecorder(1).isActive())
		{
			if (secs - lastVideoRecordingDisplayTime > 0)
			{
				m_ui.lbVideoRecording->setVisible(!m_ui.lbVideoRecording->isVisible());
				lastVideoRecordingDisplayTime = secs;
			}
		}
		else
		{
			m_ui.lbVideoRecording->setVisible(false);
		}

		m_ui.cw_ROIResult1->updateAlarm();
		m_ui.cw_ROIResult2->updateAlarm();

		secs = secs % 60;
		m_ui.lbTime->setText(QString("%1:%2:%3")
			.arg(hours, 2, 10, QLatin1Char('0'))
			.arg(mins, 2, 10, QLatin1Char('0'))
			.arg(secs, 2, 10, QLatin1Char('0')));

		m_ui.cw_ROIResult1->updateGUI();
		if (p_session->getROI(1)->getActive())
			m_ui.cw_ROIResult2->updateGUI();

	}
}

// slot
void MainWindow::updateActiveROI()
{
	bool activeROI1 = p_session->getROI(0)->getActive();
	bool activeROI2 = p_session->getROI(1)->getActive();

	// Start process icon
	// Disable startProcess if ROI1 isn't active (ROI1 not active means ROI2 isn't either)
	//qDebug() << " ROI 1 DT: " << p_session->getROI(0)->getCurrentDetectionType();
	if (!activeROI1 || p_session->getROI(0)->getCurrentDetectionType() == ROI::UNDEFINED_DT)
	{
		m_ui.tbStartProcess->setEnabled(false);
	}
	else if (activeROI1 && p_session->getROI(0)->getCurrentDetectionType() != ROI::UNDEFINED_DT)
	{
		if (!activeROI2 || (activeROI2 && p_session->getROI(1)->getCurrentDetectionType() != ROI::UNDEFINED_DT))
			m_ui.tbStartProcess->setEnabled(true);
		else
			m_ui.tbStartProcess->setEnabled(false);
	}

	timerFitView();
}

// slot
void MainWindow::resetHisto()
{
	QMessageBox msgBox(this);
	msgBox.setWindowTitle(tr("Clear log ?"));
	msgBox.setText(tr("Are you sure you want to reset the alarm log?"));
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);

	int ret = msgBox.exec();

	if (ret == QMessageBox::Ok)
	{
		p_session->clearAlarmTab();
		p_session->setDirty(true);
	}
	displayAlarms();
	actionSave();
}

// slot
void MainWindow::openDir()
{
	if (!p_session->getWorkingDir().isEmpty())
		QDesktopServices::openUrl(QUrl("file:///" + p_session->getWorkingDir(), QUrl::TolerantMode));
}

// slot
void MainWindow::pauseVideo(bool val)
{
	qDebug() << "main window video paused: " << val;

	m_ui.videoPlayer1->pause(!val);
	m_ui.videoPlayer2->pause(!val);
}

// slot
void MainWindow::startProcess()
{
	qDebug() << "--> start/stop Process sessionSaved: " << p_session->getDirty();

	m_ui.lbVideoRecording->setVisible(false);
	lastVideoRecordingDisplayTime = 0;

	if (p_session->getROI(0)->getActive() || p_session->getROI(1)->getActive())
	{
		if (!isProcessStarted)
		{
			// Save session configuration before starting 
			qDebug() << "filename: " << m_fileName << " / working dir: " << m_workingDir;

			m_ui.lbTime->setText("00:00:00");

			QMessageBox msgBox(this);
			msgBox.setWindowTitle(tr("Session writing"));
			msgBox.setText(tr("This operation will start the alarm processing. Existing alarm will be removed.\nDo you want to overwrite existing session files ?"));
			msgBox.setIcon(QMessageBox::Question);
			msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);

			int ret = msgBox.exec();
				
			if (ret == QMessageBox::Yes)
			{
				actionSave();
				p_session->clearAlarmTab();

				m_ui.cw_ROIResult1->getGroupBox()->setCheckable(false);
				m_ui.cw_ROIResult2->getGroupBox()->setCheckable(false);
				p_session->clearAlarmTab();
				p_session->getProcess()->reset();
				isProcessStarted = true;
				cwROIResultEnableCurrent(true);
				p_session->getROI(0)->setProcessStarted(true);
				p_session->getROI(1)->setProcessStarted(true);
				p_time->restart();
				m_lastTime = 0;
				setActionEnable(false);
				m_ui.tbStartProcess->setIcon(QIcon(":/resources/resources/icons/stopProcess.png"));
				updateRoiAlarm();
			}
		}
		else
		{
			m_ui.cw_ROIResult1->getGroupBox()->setCheckable(true);
			m_ui.cw_ROIResult2->getGroupBox()->setCheckable(true);

			isProcessStarted = false;
			cwROIResultEnableCurrent(false);
			p_session->getROI(0)->setProcessStarted(false);
			p_session->getROI(1)->setProcessStarted(false);
		
			setActionEnable(true);
			m_ui.tbStartProcess->setIcon(QIcon(":/resources/resources/icons/startProcess.png"));
			updateRoiAlarm();
		}
	}

	// ===== Set the logic combination of alarms
	if (m_ui.radioButtonAND->isChecked())
	{
		p_session->getProcess()->setRoi1Logicroi2(Alarm::L_AND);
	}
	else if (m_ui.radioButtonOR->isChecked())
	{
		p_session->getProcess()->setRoi1Logicroi2(Alarm::L_OR);
	}

	// ===== Start process and handle alarm
	if (isProcessStarted)
	{
		m_ui.outputInterface->setEnabled(false);
		m_ui.gbResultOp->setEnabled(false);
		m_ui.gbVideoRecording->setEnabled(false);
		m_ui.groupBoxJournal->setEnabled(false);
		m_ui.cw_ROIResult1->getPbScreenshot()->setEnabled(false);
		m_ui.cw_ROIResult2->getPbScreenshot()->setEnabled(false);
		p_session->startDetection();
		p_time->restart();
		m_lastTime = 0;
	}
	else
	{
		m_ui.outputInterface->setEnabled(true);
		m_ui.gbResultOp->setEnabled(true);
		m_ui.gbVideoRecording->setEnabled(true);
		m_ui.groupBoxJournal->setEnabled(true);
		m_ui.cw_ROIResult1->getPbScreenshot()->setEnabled(true);
		m_ui.cw_ROIResult2->getPbScreenshot()->setEnabled(true);

		p_session->stopDetection();	

		// Write alarm file once the process is over
		p_session->writeAlarmFile();

		displayAlarms();
		updateRoiAlarm();
	}

	updateGUI();

}

void MainWindow::saveSymbol()
{
	qDebug() << "MainWindow::saveSymbol";
	
	QString baseFileName = m_workingDir + "/" + QFileInfo(m_fileName).baseName() + "_symbol_roi%1.png";
	for (int i = 0; i < 2; i++)
	{
		if (p_session->getROI(i)->getCurrentDetectionType() == ROI::SYMBOL_DT)
		{
			Mat temp1;

			SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(p_session->getROI(i)->getDetectionType(ROI::SYMBOL_DT));
			Mat img = ptrSymbolDetection->getSymbol();
			if (!img.empty())
			{
				img.copyTo(temp1);
				cv::cvtColor(temp1, temp1, CV_BGR2RGB);
				imwrite((baseFileName.arg(i+1)).toStdString(), temp1);
				if (i==0)
					p_session->setSymbolPath1(baseFileName.arg(i+1));
				else if (i == 1)
					p_session->setSymbolPath2(baseFileName.arg(i + 1));
			}
		}
	}
}

// slot
void MainWindow::actionEditColor()
{
	qDebug() << "edit Color ! ";

	ColorEditWidget cew(this);
	pauseVideo(true);
	cew.setSession(p_session);
	
	cew.exec();
	pauseVideo(false);

	m_ui.cw_ROIResult1->updateGUI();
	m_ui.cw_ROIResult2->updateGUI();
}

// slot
void MainWindow::actionAlarmLog()
{
	qDebug() << "Alarm log ";
	if (!m_ui.dwAlarmLog->isVisible())
		m_ui.dwAlarmLog->show();
	else
		m_ui.dwAlarmLog->hide();
}

//
void MainWindow::actionExit()
{
	close();
}

void MainWindow::setLocalization(const QString& localization)
{
	if (p_settings)
	{
		p_settings->setValue("GUI/localization", localization);
		if (QMessageBox::question(this, tr("Survec restart"), tr("Application need to restart to change the language. Do you want to restart now ?")) == QMessageBox::Yes)
		{
			qApp->quit();
			QProcess::startDetached(qApp->arguments()[0], qApp->arguments(), qApp->applicationDirPath());
		}
	}
}

// slot
void MainWindow::actionEnglish()
{
	setLocalization("en");
}

// slot
void MainWindow::actionFrench()
{
	setLocalization("fr");
}

// slot
void MainWindow::actionNew()
{
	if (p_session != NULL && p_session->getDirty())
	{
		QMessageBox msgBox(this);
		msgBox.setWindowTitle(tr("Session is not saved"));
		msgBox.setText(tr("Do you want to save current session modification ?"));
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

		int ret = msgBox.exec();

		if (ret == QMessageBox::Yes)
		{
			actionSave();
		}
		else if (ret == QMessageBox::Cancel)
		{
			return;
		}
	}

	qDebug() << "ROI actionNew !";
	// reset alarm log
	//m_ui.tableWidget->clearContents();
	int temp = m_ui.tableWidget->rowCount();
	for (int i = 0; i < temp; i++)
	{
		m_ui.tableWidget->removeRow(0);
	}

	//p_session->clearAlarmTab();
	//displayAlarms();
	
	// Init working fileName and directory
	m_fileName = "";
	m_workingDir = QDir::homePath() + "/SurvecSessions"; 
	QDir d(m_workingDir);
	if (d.exists() == false)
		d.mkdir(m_workingDir);
	qDebug() << "DataLocation :" << m_workingDir;


	updateTitle();

	if (p_session != NULL)
	{
		// set the session to be saved (*)
		p_session->setDirty(true);

		// create new empty session
		if (p_session)
			delete p_session;
	}

	// Session
	p_session = new Session();
	p_session->setWorkingDir(m_workingDir);
	p_session->setFileName(m_fileName);
	connect(p_session, SIGNAL(changed()), this, SLOT(updateGUI()));
	connect(p_session, SIGNAL(alarmChanged()), this, SLOT(updateAlarm()));
	// Update average time
	connect(p_session->getProcess(), SIGNAL(updateAvg()), SLOT(displayAvgTime()));
	// Update title
	connect(p_session, SIGNAL(titleChanged()), SLOT(updateTitle()));
	connect(m_ui.comboBoxOutput, SIGNAL(currentIndexChanged(const QString&)), p_session, SLOT(setAlarmPluginName(const QString&)));
	connect(m_ui.checkBoxWriteResults, SIGNAL(stateChanged(int)), p_session, SLOT(setWriteResults(int)));
	connect(m_ui.checkBoxWriteImages, SIGNAL(stateChanged(int)), p_session, SLOT(setWriteImages(int)));
	connect(m_ui.radioButtonAND, SIGNAL(toggled(bool)), p_session, SLOT(setLogic(bool)));

	connect(m_ui.cbRecord1, SIGNAL(stateChanged(int)), p_session, SLOT(setRecordROI1(int)));
	connect(m_ui.cbRecord2, SIGNAL(stateChanged(int)), p_session, SLOT(setRecordROI2(int)));
	connect(m_ui.dteMaxDuration, SIGNAL(timeChanged(QTime)), p_session, SLOT(setRecordDuration(QTime)));

	m_ui.cw_ROIResult1->setROI(p_session->getROI(0), 1);
	m_ui.cw_ROIResult2->setROI(p_session->getROI(1), 2);

	// display
	updateGUI();

	timerFitView();

	p_session->setDirty(false);
}

// slot
void MainWindow::actionOpen()
{
	if (p_session != NULL && p_session->getDirty())
	{
		QMessageBox msgBox(this);
		msgBox.setWindowTitle(tr("Session is not saved"));
		msgBox.setText(tr("Do you want to save current session modification ?"));
		msgBox.setIcon(QMessageBox::Question);
		msgBox.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);

		int ret = msgBox.exec();

		if (ret == QMessageBox::Yes)
		{
			actionSave();
		}
		else if (ret == QMessageBox::Cancel)
		{
			return;
		}
	}

	qDebug() << "actionOpen !";

	m_fileName = QFileDialog::getOpenFileName(this, tr("Open File"), m_workingDir, tr("XML (*.xml)"));
	m_workingDir = QFileInfo(m_fileName).absolutePath();
	m_fileName = m_fileName.section('/', -1);
	qDebug() << "=== m_fileName: " << m_fileName
		<< "\n=== m_workingDir: " << m_workingDir;
	if (!m_fileName.isEmpty())
	{
		p_session->setWorkingDir(m_workingDir);
		p_session->setFileName(m_fileName);

		if (!p_session->readSessionFile())
		{
			p_session->setDirty(false);
			actionNew();
		}
		else
		{
			// Reset alarm log
			m_ui.tableWidget->clearContents();
			//p_session->clearAlarmTab();
			//displayAlarms();

			// read XML
			p_session->readAlarmFile();
			p_session->setDirty(false);

			// change video device stream for ROI1
			int index1 = p_session->getROI(0)->getVideoDeviceId();
			int feedId1 = VideoDeviceManager::getInstance()->getDevice(index1)->getId();
			m_ui.videoPlayer1->setDeviceId(index1);
			m_ui.videoPlayer1->setFeedId(feedId1);
			m_ui.videoPlayer1->readWebcamFeed();

			// change video device stream for ROI2
			int index2 = p_session->getROI(1)->getVideoDeviceId();
			int feedId2 = VideoDeviceManager::getInstance()->getDevice(index2)->getId();
			m_ui.videoPlayer2->setDeviceId(index2);
			m_ui.videoPlayer2->setFeedId(feedId2);
			m_ui.videoPlayer2->readWebcamFeed();

			// display alarms
			displayAlarms();

			// display title
			updateTitle();

			// update view to fit
			timerFitView();
		}
	}

	qDebug() << "alarmTab.size: " << p_session->getAlarmTab().size();
}

// slot
void MainWindow::actionSave()
{
	qDebug() << "ROI actionSave !";
	qDebug() << "MainWindow::fileName: " << m_fileName << " / workingDir: " << m_workingDir;

	if (m_fileName == "")
		actionSaveAs();

	else if (!m_fileName.isEmpty() && m_workingDir != "")
	{
		p_session->setWorkingDir(m_workingDir);
		p_session->setFileName(m_fileName);
		saveSymbol();
		p_session->writeSessionFile();
		p_session->writeAlarmFile();
		p_session->setDirty(false);
		updateTitle();
	}
	else	// If the user didn't use "Save as" before
		actionSaveAs();
}

// slot
void MainWindow::actionSaveAs()
{
	qDebug() << "ROI actionSaveAs !";
	
	m_fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), m_workingDir, tr("XML (*.xml)"));
	m_workingDir = QFileInfo(m_fileName).absolutePath();
	m_fileName = m_fileName.section('/', -1);
	
	
	qDebug() << "MainWindow::fileName: " << m_fileName << " / workingDir: " << m_workingDir;

	if (!m_fileName.isEmpty())
	{
		p_session->duplicateAlarmTabFiles(m_workingDir, m_fileName);
		p_session->setWorkingDir(m_workingDir);
		p_session->setFileName(m_fileName);
		saveSymbol();
		p_session->writeSessionFile();
		p_session->writeAlarmFile();
		p_session->setDirty(false);
		updateTitle();
	}
}

// slot
void MainWindow::actionHelp()
{
	qDebug() << "ROI actionHelp !";

	QString userManualFilepath = QString("%1/Manuel_utilisateur.pdf").arg(qApp->applicationDirPath());
	if (QFile::exists(userManualFilepath))
	{
		QDesktopServices::openUrl("file:///"+userManualFilepath);
	}
	else
	{
		QMessageBox::about(this, tr("Help"), tr("SURVEC\n\nSee user's guide"));
	}
}

// slot
void MainWindow::actionInfo()
{
	qDebug() << "ROI actionInfo !";
	QMessageBox::about(this, tr("About"), tr("SURVEC\n\nAuthors: Alyotech\nLicense EUPL\nMIP DGA\nCopyright DGA-MI 2015\n\nTesseract3.02\nLeptonica1.71\nOpenCV2.4.10\nQt5.4"));
}

// slot
void MainWindow::changeFPS(int value)
{
	m_ui.lbFPS->setText("FPS: " + QString::number(value));
}

// slot
void MainWindow::updateAlarm()
{
	QTime time_updateGUI;
	time_updateGUI.start();

	updateRoiAlarm();

	/********************************************
	*  Alarm tab (update from opening XML file)
	********************************************/
	if (isProcessStarted)
		displayAlarms();

	// Fit view to scene
	timerFitView();

	qDebug() << " update GUI2 t=" << time_updateGUI.elapsed();
}

// slot
void MainWindow::updateRoiAlarm()
{
	if (p_session->getProcess()->getLogic() && isProcessStarted)
	{
		m_ui.lbMainAlarm->setPixmap(QPixmap(":/resources/resources/icons/ledALARM.png"));
	}
	else if (!p_session->getProcess()->getLogic() && isProcessStarted)
	{
		m_ui.lbMainAlarm->setPixmap(QPixmap(":/resources/resources/icons/ledOK.png"));
	}
	else if (!isProcessStarted)
	{
		m_ui.lbMainAlarm->setPixmap(QPixmap(":/resources/resources/icons/ledNO.png"));
	}

	m_ui.cw_ROIResult1->updateAlarm();
	m_ui.cw_ROIResult2->updateAlarm();
}