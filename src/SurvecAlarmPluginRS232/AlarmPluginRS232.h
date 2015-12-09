#ifndef ALARMPLUGINRS232_H
#define ALARMPLUGINRS232_H

#include "AlarmPluginInterface.h"
// Include QT
#include <QtPlugin>
#include <QString>
#include <QSerialPort>
#include <QTimer>

#if defined(WIN32)
#if defined(SurvecAlarmPluginRS232_EXPORTS)
#define  SURVECLIB_PLUGINRS232_EXPORT __declspec(dllexport)
#else
#define  SURVECLIB_PLUGINRS232_EXPORT __declspec(dllimport)
#endif
#else
#define SURVECLIB_PLUGINRS232_EXPORT
#endif

class SURVECLIB_PLUGINRS232_EXPORT AlarmPluginRS232 : public AlarmPluginInterface
{
	Q_OBJECT
	Q_PLUGIN_METADATA(IID AlarmPluginInterface_iid FILE "AlarmPluginRS232.json")
	Q_INTERFACES(AlarmPluginInterface)

public:

	/**
	*	Constructor
	*/
	AlarmPluginRS232();

	/**
	*	Destructor
	*/
	~AlarmPluginRS232();
	
	/**
	*	@brief: used to get the plugin name
	* 	@return: the plugin name
	*/
	QString				name() const;
	
	/**
	*	@brief: init the plugin
	*/
	void				initPlugin();

	/**
	*	@brief: method called when detecting an alarm
	*	@param cv::Mat image: the image containing alarm 
	*/
	void				beginAlarm(Alarm* alarm);

	/**
	*	@brief: method called when ending an alarm
	*	@param cv::Mat image: the image containing alarm
	*/
	void				endAlarm(Alarm* alarm);

protected:
	QSerialPort *		p_port;				//<! RS232 port
};

#endif
