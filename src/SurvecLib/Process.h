#ifndef PROCESS_H
#define PROCESS_H

#include "Survec_dll.h"
#include <Thread.h>
#include <ROI.h>
#include <Alarm.h>
#include <ROIVideoWriter.h>

#include <QThread>
#include <QTime>
#include <QSemaphore>


class SURVECLIB_EXPORT Process: public QThread
{
	Q_OBJECT

public:
	/**
	*	Constructor
	*/
	Process();

	/**
	*	Destructor
	*/
	~Process();

	/**
	 *  @brief: reset the alarm state.
	 */
	void reset();
	
	/**
	*	@brief: start process
	*/
	void						start();

	/**
	*	@brief: stop process
	*/
	void						stop();

	/**
	*	@brief: set the ROI to work on
	*	@param ROI: the ROI1
	*/
	void						setROI1(ROI * roi);

	/**
	*	@brief: set the ROI to work on
	*	@param ROI: the ROI2
	*/
	void						setROI2(ROI * roi);

	/**
	*	@brief: set the values of the Alarm
	*/
	void						handleAlarm();

	/**
	*	@brief: create a new QTime to measure the duration of an alarm
	*/
	void						newTimeAlarm();

	/**
	*	@brief: get the alarm
	*/
	Alarm&						getAlarm();

	/**
	*	@brief: get the QTime of the alarm
	*/
	QTime *						getTimeAlarm();

	/**
	*	@brief: get if the alarm is running
	*/
	bool						getAlarmRunning();

	/**
	*	@brief: get the logic of the session ("AND" or "OR")
	*/
	bool						getLogic();

	/**
	*	@brief: set the logic between both ROI
	*/
	void						setRoi1Logicroi2(Alarm::logic l);

	/**
	*	@brief: set if the alarm is running
	*/
	void						setAlarmRunning(bool val);

	/**
	*	@brief: set, as a QString, the type of ROI for target ROI
	*	@param numROI: the index of ROI
	*	@param ptr_roi: the ROI to update
	*/
	void						setTypeROIString(int numROI, ROI * ptr_roi);

	/**
	*	@brief: set the status of the alarm as a QString (XML purpose)
	*/
	void						setStatusString(int status);

	/**
	*	@brief: set the logic of the alarm as a QString (XML purpose)
	*/
	void						setLogicString(int value);

	/**
	*	@brief: get the average time of the last 10 process
	*/
	int							getAvgTime();

	/**
	*	@brief: set the current working directory
	*/
	void						setWorkingDir(QString path);

	/**
	*	@brief: get the working directory
	*/
	QString						getWorkingDir();


	/**
	*	@brief: set the current session filename
	*/
	void						setFileName(QString fileName);

	const ROIVideoWriter&		getVideoRecorder(int roi) const;

	/**
	*	@brief: set writeImage (write on disk the image when an alarm is thrown)
	*/
	void						setWriteImage(int val);

	void						setAlarmReceived();

signals:
	/**
	*	@brief: start a new alarm and a new line in MainWindow's QTableWidget
	*/
	void						alarmBegin();

	/**
	*	@brief: end the alarm, update the duration time and status to "Completed"
	*/
	void						alarmEnd();

	/**
	*	@brief: sent every 10 processes to display in the MainWindow the average time
	*/
	void						updateAvg();

	void						alarmChanged();

protected:
	/**
	*	@brief: setup and start the thread for both ROIs
	*/
	virtual void				run();

	void						updateVideoRecord(ROIVideoWriter& video, ROI *pROI, bool initRecord, const QString& fileName);

public:
	ROI *						ptr_roi1;				//!<pointer to ROI1
	ROI *						ptr_roi2;				//!<pointer to ROI2

	QSemaphore					m_waitForAlarmReceived;

	bool						m_stop;					//!<boolean to start/stop the thread
	bool						alarmRunning;			//!<boolean to handle alarms
	Thread						m_threadROI1;			//!<the thread for processing the ROI1
	Thread						m_threadROI2;			//!<the thread for processing the ROI2

	QTime *						p_timeAlarm;			//!<time to display the duration in the alarm log
	QTime *						p_timeThread;			//!<time to display the duration in the alarm log
	Alarm						m_alarm;

	QVector<int>				m_arrayAvgTime;			//!<array of last 10 elapsed time needed for the processing
	int							m_avgTime;				//!<value of the average last 10 elapsed time
	bool						logic;					//!<boolean to handle the alarm logic between both ROI
	Alarm::logic				roi1Logicroi2;			//!<logic between both ROI ("AND" or "OR")
	bool						firstTime;				//!<boolean for the first iteration where an alarm is thrown

	bool						m_lastRoi1Alarm;
	bool						m_lastRoi2Alarm;

	ROIVideoWriter				m_video1;				//!<tool to write the video of ROI1
	ROIVideoWriter				m_video2;				//!<tool to write the video of ROI2

	QString						m_workingDir;			//!<current working directory
	QString						m_fileName;				//!<current session filename
	int							m_writeImage;			//!<boolean to write images

	QMutex						m_mutex;				//!<mutex to protect members from other threads

};
#endif
