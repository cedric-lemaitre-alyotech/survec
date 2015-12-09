/**
 *  @addtogroup SurvecLib
 *  @{
 *  @file XML.h
 */
#ifndef XML_H
#define XML_H
#include "Survec_dll.h"
#include <opencv2/core/core.hpp>
#include <VideoDeviceManager.h>
#include <Thread.h>
#include <Process.h>
#include <Alarm.h>
#include <Session.h>
#include <QDomDocument>

/**
 *  
 */

class SURVECLIB_EXPORT XML: public QObject
{
	Q_OBJECT
public:

	static QString			ERROR_FILE_LOADING;
	static QString			ERROR_WEBCAM_CONNEXION;
	static QString			ERROR_FILE_FORMAT;

public:

	/**
	 *  Constructor
	 */
	XML();

	/**
	 *  Destructor
	 */
	~XML();

	/**
	*	@brief: reads the session ROIs parameters
	*	@param fileName: the file to read
	*	@param QString fileName: path towards the configuration xml file
	*/
	bool readSessionFile(QString fileName, Session * p_session);
	
	/**
	*	@brief: writes the session ROIs parameters
	*	@param fileName: the file to write
	*	@param QString fileName: path towards the configuration xml file
	*/
	void writeSessionFile(QString fileName, Session * p_session);

	/**
	*	@brief: read the alarm file
	*	@param fileName: the file to read
	*	@param alarmTab: the alarm tab
	*/
	void readAlarmFile(QString fileName, QVector<Alarm>& alarmTab);
	
	/**
	*	@brief: write the alarm file
	*	@param fileName: the file to write
	*	@param alarmTab: the alarm tab
	*/
	void writeAlarmFile(QString fileName, QVector<Alarm>& alarmTab);


signals:
	/**
	*	@brief: calls during readSession and readAlarm to update GUI
	*/
	void changed();


};

#endif // XML_H
// @}
