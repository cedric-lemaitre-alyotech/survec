#ifndef ALARMPLUGININTERFACE_H
#define ALARMPLUGININTERFACE_H

#include "Survec_dll.h"
#include "Alarm.h"

#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QObject>

#define AlarmPluginInterface_iid "survec.AlarmPluginInterface"

class SURVECLIB_EXPORT AlarmPluginInterface: public QObject
{
	Q_OBJECT

public:

	/**
	*	Constructor
	*/
	AlarmPluginInterface();

	/**
	*	Destructor
	*/
	~AlarmPluginInterface();
	
	/**
	*	@brief: used to get the plugin name
	* 	@return: the plugin name
	*/
	virtual QString name() const = 0;
	
	/**
	*	@brief: init the plugin
	*/
	virtual void				initPlugin() = 0;

	/**
	*	@brief: method called when detecting an alarm
	*	@param cv::Mat image: the image containing alarm 
	*/
	virtual void				beginAlarm(Alarm* alarm) = 0;

	/**
	*	@brief: method called when detecting an alarm
	*	@param cv::Mat image: the image containing alarm
	*/
	virtual void				endAlarm(Alarm* alarm) = 0;
};

Q_DECLARE_INTERFACE(AlarmPluginInterface, AlarmPluginInterface_iid)

#endif
