/**
 *  @addtogroup SurvecLib
 *  @{
 *  @file Session.h
 */
#ifndef SESSION_H
#define SESSION_H

#include "Survec_dll.h"
#include <opencv2/core/core.hpp>

#include <VideoDeviceManager.h>
#include <Thread.h>
#include <Process.h>
#include <Alarm.h>
#include <AlarmPluginInterface.h>

#include <QSerialPort>

class XML;
class ROI;
class Scene;
class VideoDeviceManager;
/**
 *  
 */
class SURVECLIB_EXPORT Session: public QObject
{
	Q_OBJECT

public:

	/**
	 *  Constructor
	 */
	Session();

	/**
	 *  Destructor
	 */
	~Session();

	/**
	*	@brief: inform each ROI that the color to detect has been modified
	*/
	void colorToDetectModified();

	/**
	*  @bief ROI accessor
	*/
	ROI * getROI(int index) const;

	/**
	*	@brief: get the number of roi
	*/
	int getNumberOfROI();

	/**
	*	@brief: get if the process is runing
	*/
	bool processIsRunning();

	/**
	*	@brief: get the process
	*/
	Process * getProcess();

	/**
	*	@brief: clear the alarm tab and remove the corresponding files (images)
	*/
	void clearAlarmTab();

	/**
	*	@brief: duplicates the alarm tab corresponding files (images)
	*/
	void duplicateAlarmTabFiles(const QString& newDir, const QString& newSessionFilename);

	/**
	*	@brief: get the alarm at index
	*/
	Alarm& getAlarmTab(int index);
	
	/**
	*	@brief: get all of the alarms
	*/
	QVector<Alarm>& getAlarmTab();

	/**
	*	@brief: get the XML
	*/
	XML	* getXML();
	
	/**
	*	@brief: calls XML function to write session info
	*/
	void writeSessionFile();
	
	/**
	*	@brief: calls XML function to read session info
	*/
	bool readSessionFile();

	/**
	*	@brief: calls XML function to read alarms of the matching session
	*/
	void readAlarmFile();
	
	/**
	*	@brief: calls XML function to write alarms of the current session
	*/
	void writeAlarmFile();
	
	/**
	*	@brief: set the current working directory
	*/
	void setWorkingDir(QString dir);
	
	/**
	*	@brief: get the working directory
	*/
	QString getWorkingDir();

	/**
	*	@brief: set the current session filename
	*/
	void setFileName(QString fileName);

	/**
	*	@brief: get thesession filename
	*/
	QString getFileName();

	/**
	*	@brief: get the flag meaning that the session has changed and must be saved
	*/
	bool getDirty();
	
	/**
	*	@brief: set the flag to save the session
	*/
	void setDirty(bool val);

	/**
	*	@brief: get the selected alarm plugin (RS232...)
	*/
	AlarmPluginInterface* getAlarmPlugin();

	/**
	*	@brief: get the physical output interface
	*/
	QString getAlarmPluginName();

	/**
	*	@brief: get the logic ("AND" or "OR")
	*/
	QString getLogic();

	/**
	*	@brief: get if write results
	*/
	int getWriteResults();

	/**
	*	@brief: get if write images
	*/
	int getWriteImages();

	/**
	*	@brief: get if video record ROI1
	*/
	int getRecordROI1();

	/**
	*	@brief: get if video record ROI2
	*/
	int getRecordROI2();

	/**
	*	@brief: get the video record duration
	*/
	QTime getRecordDuration();

	/**
	*	@brief: get the path to the symbol of ROI1
	*/
	QString getSymbolPath1();

	/**
	*	@brief: get the path to the symbol of ROI2
	*/
	QString getSymbolPath2();

	/**
	*	@brief: get the path to the chosen ROI
	*/
	QString getSymbolPath(int indexRoi);

	/**
	*	@brief: set the path of the symbol for ROI1
	*/
	void setSymbolPath1(QString value);

	/**
	*	@brief: set the path of the symbol for ROI2
	*/
	void setSymbolPath2(QString value);

	/**
	*	@brief: set the path of the symbol for the chosen ROI
	*/
	void setSymbolPath(int indexRoi, QString value);


signals:
	/**
	*	@brief: emitted signal to indicate model change
	*/
	void changed();

	/**
	*	@brief: emitted signal to indicate alarm change
	*/
	void alarmChanged();

	/**
	*	@brief: emitted signal to indicate title change
	*/
	void titleChanged();

public slots:
	/**
	*	@brief: start the process
	*/
	void startDetection();

	/**
	*	@brief: stop the process
	*/
	void stopDetection();

	/**
	*	@brief: create a new alarm
	*/
	void setAlarmBegin();
	
	/**
	*	@brief: finish the alarm
	*/
	void setAlarmEnd();

	/**
	*	@brief: send a signal to MainWindow to update the GUI
	*/
	void sessionModified();

	/**
	*	@brief: set the physical interface output (for XML purpose)
	*/
	void setAlarmPluginName(const QString & name);

	/**
	*	@brief: set the logic operator ("AND" or "OR")
	*/
	void setLogic(bool value);

	/**
	*	@brief: enable to write result
	*/
	void setWriteResults(int value);

	/**
	*	@brief: enable to write images
	*/
	void setWriteImages(int value);

	/**
	*	@brief: enable to video record ROI1
	*/
	void setRecordROI1(int value);

	/**
	*	@brief: enable to video record ROI2
	*/
	void setRecordROI2(int value);

	/**
	*	@brief: set the duration of the record
	*/
	void setRecordDuration(QTime value);

	
protected:
	QVector<ROI *>				m_rois;					//!<vector of ROIs
	Process *					p_process;				//!<pointer to Process
	QVector<Alarm>				m_alarmTab;				//!<vector of alarms
		
	XML	*						p_xmlDoc;				//!<pointer to the XML class
	QString						m_workingDir;			//!<working directory
	QString						m_fileName;				//!<session filename

	bool						m_dirty;				//!<flag to indicate if session has to be saved

	// Alarm interface plugin
	QString						m_alarmPluginName;		//!<physical interface read from the XML file
	AlarmPluginInterface*		p_alarmPlugin;			//!<pointer to the alarm plugin interface

	// XML values
	QString						m_logicOperator;		//!<logic operator read from the XML
	bool						m_results;				//!<boolean if the user wants to write results
	bool						m_images;				//!<boolean if the user wants to write images
	QTime						m_recordDuration;		//!<duration of the recording

	// Serial port values
	QString						m_portName;				//!<serial port name
	QSerialPort::BaudRate		m_baudRate;				//!<serial port baud rate
	QSerialPort::DataBits		m_dataBits;				//!<serial port data bits
	QSerialPort::Parity			m_parity;				//!<serial port parity
	QSerialPort::StopBits		m_stopBits;				//!<serial port stop bits

	QString						m_symbolPath1;			//!<path to the symbol for ROI1
	QString						m_symbolPath2;			//!<path to the symbol for ROI2


};

#endif // SESSION_H
// @}
