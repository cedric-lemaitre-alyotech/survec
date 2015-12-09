#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_MainWindow.h"

#include <Scene.h>
#include <ColorEditWidget.h>
#include <ROIConfigurationWidget.h>
#include <cwVideoPlayer.h>
#include <Alarm.h>
#include <Thread.h>
#include <Version.h>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

//Qt
#include <QTimer>
#include <QDir>
#include <QSerialPort>
#include <QSettings>

class Survec;
class cwDisplayAlarmImg;

class MainWindow: public QMainWindow
{
	Q_OBJECT

protected:
	static const int NB_ALARM_COLS = 11;
public:

	/**
	*	Constructor
	*/
	MainWindow(QSettings* settings, QWidget *parent = 0, Qt::WindowFlags flags = 0);

	/**
	*	Destructor
	*/
	~MainWindow();

	/**
	*	@brief: disable actionButton when the processing has started
	*	@param bool value: enable or not
	*/
	void						setActionEnable(bool value);
	
	/**
	*	@brief: display the alarms in the QTableWidget alarmlog
	*/
	void						displayAlarms();

	/**
	*	@brief:	enables qlabel
	*/
	void						cwROIResultEnableCurrent(bool val);

	/**
	*	 @brief: write the symbol to the working dir
	*/
	void						saveSymbol();

	/**
	*	@brief: set the language (french or english)
	*/
	void						setLocalization(const QString& localization);

	/**
	*	@brief: start a one-shot timer to fit the view just after Qt displays the GUI
	*/
	void						timerFitView();

	/**
	*	@brief: ask the user confirmation before closing the window
	*/
	void						closeEvent(QCloseEvent * event);
	
protected slots:

	/** Slot: called by the on-shot timer to fit the webcam
	*	@brief: fit the webcam
	*/
	void	fitWebcam();

	/** Slot: called when ROIConfigurationWidget emit UpdateROIResult (on "OK" button)
	*	@brief: set cwROIResult's ROI which then update itself
	*/
	void						updateROIResult();

	/** Slot: called on timer's tick (every 40ms)
	*	@brief: disable the checkboxes "Active ROI1/ROI2" when one ROI is set as Undefined Detection
	*/
	void						updateGUI();

	/**	Slot: called from cwROIResult 
	*	@brief: used to allow the user to start the process depending on ROIs state (active or not, undefined or not)
	*/
	void						updateActiveROI();

	/**
	*	@brief: update the video & display
	*/
	void						updateDisplayTime();
	
	/**
	*	@brief: action opening the ColorEditWidget dialog
	*/
	void						actionEditColor();

	/**
	*	@brief: action opening the New dialog
	*/
	void						actionNew();

	/**
	*	@brief: action opening the OpenFile dialog
	*/
	void						actionOpen();

	/**
	*	@brief: action opening the SaveFile dialog
	*/
	void						actionSave();

	/**
	*	@brief: action opening the SaveAsFile dialog
	*/
	void						actionSaveAs();

	/**
	*	@brief: action opening the Help dialog
	*/
	void						actionHelp();

	/**
	*	@brief: action opening the Info dialog
	*/
	void						actionInfo();

	/**
	*	@brief: action opening the Alarm log dock widget
	*/
	void						actionAlarmLog();

	/**
	*	@brief: action for the exit
	*/
	void						actionExit();

	/**
	*	@brief: action for languages change
	*/
	void						actionEnglish();
	void						actionFrench();

	/** Slot: on click on the startProcess button
	*	@brief: calls setActionEnable
	*/
	void						startProcess();

	/**
	*	@brief: update the title of the window
	*/
	void						updateTitle();

	/**
	*	@brief: clear the alarm log
	*/
	void						resetHisto();

	/**
	*	@brief: open the current directory
	*/
	void						openDir();

	/**
	*	@brief: pause the video feed while dialogs are open
	*/
	void						pauseVideo(bool val);

	/**
	*	@brief: display the average time of the last 10 image detection
	*/
	void						displayAvgTime();

	/**
	*	@brief: called to change the webcam feed for ROI1
	*/
	void						updateWebcamFeedROI1();

	/**
	*	@brief: called to change the webcam feed for ROI2
	*/
	void						updateWebcamFeedROI2();

	/**
	*	@brief: measure of FPS
	*/
	void						changeFPS(int value);

	/**
	*	@brief: called on double click in a cell of the alarm log to display the images that started the alarm
	*/
	void						displayImage(int, int);

	/**
	*	@brief: update the alarm
	*/
	void						updateAlarm();
		
	/**
	*	@brief: update the alarm
	*/
	void						updateRoiAlarm(); 

protected:
	Ui::MainWindow				m_ui;							//!<ui

	QSettings*					p_settings;						//!<Settings de l'application
	Session *					p_session;						//!<pointer to Session
	QTimer *					p_timerDisplay;					//!<timer to update cwROIResult & the display of the webcam
	int							bounds[4];						//!<boundaries of the ROI's rectangle

	int							lastVideoRecordingDisplayTime;
	
	QTime *						p_time;							//!<time to display the elapsed time since the process started
	int							m_lastTime;						
	int							m_nbDays;
	QPixmap *					p_initPixmap;					//!<the initial pixmap
	
	bool						isProcessStarted;				//!<the boolean if the process is started
	
	QPixmap *					qpixmap1;

	QString						m_fileName;						//!<working session fileName
	QString						m_workingDir;					//!<where the session files are saved
	bool						m_sessionSaved;					//!<check if the session has been saved or not
	
	cwDisplayAlarmImg *			p_alarmImgDialog;				//!<dialog to display the image that started the alarm
};

#endif
