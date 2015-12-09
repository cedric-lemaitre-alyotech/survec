#ifndef ALARM_H
#define ALARM_H

#include "Survec_dll.h"
#include <stdio.h>
#include <QDateTime>
#include <QObject>

class SURVECLIB_EXPORT Alarm : public QObject
{
	Q_OBJECT

public:

	typedef enum
	{
		AS_RUNNING = 0,
		AS_COMPLETED
	} alarmStatus;

	typedef enum
	{
		L_AND = 0,
		L_OR
	} logic;

	/**
	*	@brief Default constructor
	*/
	Alarm();
		
	/**
	*	@brief Copy constructor
	*/
	Alarm(const Alarm& alarm);

	/**
	*	Destructor
	*/
	~Alarm();

	Alarm& operator=(const Alarm& other); // copy assignment

	/**
	*	@brief clear all fields 
	*/
	void reset();
	
	/**
	*	@brief: get the date
	*	@return: the date
	*/
	QString			getDate() const;

	/**
	*	@brief: get start time
	*	@return: the start time
	*/
	QString			getStartTime() const;

	/**
	*	@brief: get duration
	*	@return: the duration
	*/
	QString			getDuration() const;

	/**
	*	@brief: get status
	*	@return: the status
	*/
	QString			getStatus() const;

	/**
	*	@brief: get detection type for ROI1
	*	@return: the detection type
	*/
	QString			getTypeROI1() const;

	/**
	*	@brief: get detection type for ROI2
	*	@return: the detection type
	*/
	QString			getTypeROI2() const;

	/**
	*	@brief: get detection type for ROI
	*	@param: indexROI: the chosen ROI (1 or 2)
	*	@return: the detection type
	*/
	QString			getTypeROI(int indexRoi) const;

	/**
	*	@brief: get logic
	*	@return: the logic
	*/
	QString			getLogic() const;

	/**
	*	@brief: get current value for ROI
	*	@param: indexRoi: the chosen ROI (1 or 2)
	*	@return: the value
	*/
	QString			getCurrent(int indexRoi) const;

	/**
	*	@brief: get target value for ROI
	*	@param: indexRoi: the chosen ROI (1 or 2)
	*	@return: the target
	*/
	QString			getTarget(int indexRoi) const;

	/**
	*	@brief: get the image path to ROI1's image
	*	@return: the image path
	*/
	QString			getImagePath1() const;

	/**
	*	@brief: get the image path to ROI2's image
	*	@return: the image path
	*/
	QString			getImagePath2() const;

	/**
	*	@brief: get the image path to ROI's image
	*	@param: indexRoi: the chosen ROI (1 or 2)
	*	@return: the image path
	*/
	QString			getImagePath(int indexRoi) const;

	/**
	*	@brief: set the date
	*	@param: date: the date
	*/
	void			setDate(QString date);

	/**
	*	@brief: set start time
	*	@param: start time: start time
	*/
	void			setStartTime(QString startTime);

	/**
	*	@brief: set duration
	*	@param: duration: duration
	*/
	void			setDuration(QString duration);

	/**
	*	@brief: set status
	*	@param: status: status
	*/
	void			setStatus(QString status);

	/**
	*	@brief: set the detection type and the "is"/"is not" boolean
	*			result is something like "Color is not"
	*	@param: numROI: the number of the ROI
	*	@param: type: the detection type
	*	@param: onEquals: when to throw the alarm ("is" or "isnot")
	*/
	void			setType(int numROI, QString type, bool onEquals);

	/**
	*	@brief: set the detection type
	*	@param: numROI: the number of the ROI
	*	@param: type: the detection type
	*/
	void			setType(int indexRoi, QString type);

	/**
	*	@brief: set detection type for ROI1
	*	@param: type: the detection type
	*/
	void			setTypeROI1(QString typeROI1);

	/**
	*	@brief: set detection type for ROI1
	*	@param: type: the detection type
	*/
	void			setTypeROI2(QString typeROI2);

	/**
	*	@brief: set current value for ROI1
	*	@param: value: the current value
	*/
	void			setCurrent1(QString value);

	/**
	*	@brief: set current value for ROI2
	*	@param: value: the current value
	*/
	void			setCurrent2(QString value);

	/**
	*	@brief: set current value for chosen ROI
	*	@param: value: the current value
	*/
	void			setCurrent(int indexRoi, QString value);

	/**
	*	@brief: set the target for ROI1
	*	@param: value: the target value
	*/
	void			setTarget1(QString value);

	/**
	*	@brief: set the target for ROI2
	*	@param: value: the target value
	*/
	void			setTarget2(QString value);

	/**
	*	@brief: set the target for chosen ROI
	*	@param: indexRoi: index of ROI
	*	@param: value: the target value
	*/
	void			setTarget(int indexRoi, QString value);

	/**
	*	@brief: set logic
	*	@param: logic: the logic
	*/
	void			setLogic(QString logic);

	/**
	*	@brief: set the image path for ROI1
	*	@param: value: the image path
	*/
	void			setImagePath1(QString value);

	/**
	*	@brief: set the image path for ROI2
	*	@param: value: the image path
	*/
	void			setImagePath2(QString value);

	/**
	*	@brief: set the image path for chosen ROI
	*	@param: indexRoi: index of ROI
	*	@param: value: the image path
	*/
	void			setImagePath(int indexRoi, QString value);

	/**
	*	@brief: get the value 
	*	@param: i: from 0 to 9: date/startTime/duration/status/type1/current1/target1/logic/type2/current2/target2
	*/
	const QString	getValueAt(int i) const;

	/**
	*	@brief: get the background color for ROI1
	*/
	QString			getBackgroundColor1() const;

	/**
	*	@brief: set the background color for ROI1 
	*/
	void			setBackgroundColor1(QString color);

	/**
	*	@brief: get the background color for ROI2
	*/
	QString			getBackgroundColor2() const;

	/**
	*	@brief: set the background color for ROI2
	*/
	void			setBackgroundColor2(QString color);

	/**
	*	@brief: get the background color for target ROI
	*/
	QString			getBackgroundColor(int indexROI) const;

	/**
	*	@brief: set the background color for target ROI
	*/
	void			setBackgroundColor(int indexROI, QString color);
	
protected:
	QString			m_date;				//!<the date the alarm started
	QString			m_startTime;		//!<the time the alarm started
	QString			m_duration;			//!<the duration of the alarm
	QString			m_status;			//!<the status (running or completed)
	QString			m_type1;			//!<the detection type for ROI1
	QString			m_current1;			//!<if TextDetection: the QString. If ColorDetection: RGB code
	QString			m_target1;			//!<the target of ROI1
	QString			m_logic;			//!<the logic between both ROI ("AND" or "OR")
	QString			m_type2;			//!<the detection type for ROI2
	QString			m_current2;			//!<if TextDetection: the QString. If ColorDetection: RGB code
	QString			m_target2;			//!<the target of ROI2

	QString			m_imagePath1;		//!<the image path of ROI1
	QString			m_imagePath2;		//!<the image path of ROI2

	QString			m_backgroundColor1;	//!<the background color for ROI1 inside the log (QTableWidget of MainWindow)
	QString			m_backgroundColor2;	//!<the background color for ROI2 inside the log (QTableWidget of MainWindow)
};

Q_DECLARE_METATYPE(Alarm);

#endif
