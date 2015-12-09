#ifndef DETECTIONTYPE_H
#define DETECTIONTYPE_H

#include "Survec_dll.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QObject>

class SURVECLIB_EXPORT DetectionType: public QObject
{
	Q_OBJECT

public:

	/**
	*	Constructor
	*/
	DetectionType();

	/**
	*	Destructor
	*/
	~DetectionType();

	/**
	*	@brief: process method detecting an alarm
	*	@param cv::Mat image: the image to work with
	*/
	virtual void				process(cv::Mat &image) = 0;

	/**
	*	@brief: get the alarm value (true means an alarm is thrown, false means there's no alarm)
	*/
	bool				getAlarm();

	void				setAlarm(bool val);

	bool				getAlarmValid();

	void				setAlarmValid(bool val);

	void				setDebug(bool val);

signals:
	/**
	*	@brief: emitted signal to indicate model change
	*/
	void						changed();
	
	/**
	*	@brief: emitted signal to indicate alarm change
	*/
	void						alarmChanged();


protected:
	cv::Mat						m_image;	//!<the image to process

	bool						m_alarm;				//!<boolean set to 'true' when an alarm is thrown
	bool						m_alarmValid;				//!<boolean set to 'true' when an alarm is thrown
	bool						m_debug;				//!<for debug purposes
	QMutex						m_mutex;				//!<mutex to protect members from other threads
};
#endif
