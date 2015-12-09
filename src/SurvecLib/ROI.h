#ifndef ROI_H
#define ROI_H

#include "Survec_dll.h"
#include <QObject>
#include <stdio.h>

// Detection Type
#include "DetectionType.h"
#include "ColorDetection.h"
#include "SymbolDetection.h"
#include "TextDetection.h"
#include "NumberDetection.h"
#include "UndefinedDetection.h"
#include <QTime>
//#include <QObject.h>

class VideoDevice; // forward declaration 

class SURVECLIB_EXPORT ROI: public QObject
{
	Q_OBJECT

protected:
	static const int NB_DETECTION_TYPE = 5;

public:

	typedef struct
	{
		int x;
		int y;
		int width;
		int height;
	}rectangle;

	typedef enum
	{
		UNDEFINED_DT = 0,
		COLOR_DT,
		SYMBOL_DT,
		TEXT_DT,
		NUMBER_DT
	} detectionType;

public:
	
	/**
	*	@brief Default constructor
	*/
	ROI();

	/**
	*	Constructor
	*/
	ROI(VideoDevice * pVideoDevice);

	/**
	*	Destructor
	*/
	~ROI();

	/**
	*	@brief: change the color to detect
	*/
	void colorToDetectModified();

	/**
	*	@brief video device instanciation
	*/
	void setVideoDevice(VideoDevice * pVideoDevice);

	/**
	*	@brief set the current detection type
	*/
	void setCurrentDetectionType(detectionType type);
	void setCurrentDetectionTypeAsQString(QString type);
	
	/**
	*	@brief get the current detection type
	*/
	detectionType getCurrentDetectionType() const;

	/**
	*	@brief: get the detection type
	*/
	DetectionType * getDetectionType(detectionType type) const;

	/**
	*	@brief: get the video device id
	*/
	int getVideoDeviceId() const;

	/**
	*	@brief: get the current image of the ROI
	*/
	cv::Mat&	getImage();

	/**
	*	@brief: get the current image from Video Device
	*/
	void getImage(cv::Mat & image);

	/**
	*	@brief: set the boundaries of the rectangle
	*/
	void setBounds(int * rectangle);

	/**
	*	@brief: set the boundaries of the rectangle
	*/
	void setBounds(int x, int y, int width, int height);

	/**
	*	@brief: get the boundaries of the rectangle
	*/
	void getBounds(int * bounds) const;

	/**
	*	@brief: get the boundaries of the rectangle
	*/
	void getBounds(rectangle & bounds) const;

	/**
	*	@brief: set if the ROI is active or not
	*/
	void setActive(bool value);

	/**
	*	@brief: get if the ROI is active
	*/
	bool getActive() const;

	/**
	*	@brief: get the alarm (calls getAlarm() from each DetectionType)
	*/
	bool getAlarm();

	bool getAlarmValid();

	/**
	*	@brief: set the alarm
	*/
	void setAlarm(bool val);

	/**
	*	@brief: set the debug for each detectionType
	*/
	void setDebug(bool val);

	/**
	*	@brief: get the debug value
	*/
	bool getDebug();

	/**
	*	@brief: get the current value (calls getCurrent() from each DetectionType)
	*/
	QString getCurrent();

	/**
	*	@brief: get the target value (calls getTarget() from each DetectionType)
	*/
	QString getTarget();

	/**
	*	@brief: set the target (calls setTarget() from each DetectionType but SymbolDetection)
	*/
	void setTarget(QString target);

	/**
	*	@brief: set the font to work with (in Number and TextDetection only)
	*/
	void setFont(QString font);

	/**
	*	@brief: get the AlarmOnEquals bool (calls getAlarmOnEquals() for each DetectionType but Number since it has more operators)
	*/
	bool getAlarmOnEquals() const;

	/**
	*	@brief: set the operator for each DetectionType
	*/
	void setOperator(QString op);
	
	/**
	*	@brief: set if the process has started
	*/
	void setProcessStarted(bool value);

	/**
	*	@brief: get the value if the process is started
	*/
	bool getProcessStarted();

	/**
	*	@brief: get the value of the tolerance
	*/
	float getTolerance() const;

	/**
	*	@brief: set the value of the tolerance
	*/
	void setTolerance(float value);

	/**
	*	@brief: get the filtration value
	*/
	int getFiltration() const;

	/**
	*	@brief: set the filtration value
	*/
	void setFiltration(int value);

	/**
	*	@brief: get the filtration value
	*/
	int getRepetitionCount() const;

	/**
	*	@brief: get the filtration value
	*/
	void setRepetitionCount(int value);

	/**
	*	@brief: copy all of one ROI into another
	*	@param ROI other: the other ROI to copy
	*/
	ROI& operator=(const ROI& other);

	/**
	*	@brief: set the image to work with from video device
	*/
	void retrieveImage();
	
	/**
	*	@brief: set the video device to work with
	*/
	void setVideoDeviceId(int id);

	/**
	*	@brief: get the video device
	*/
	VideoDevice * getVideoDevice();

	/**
	*	@brief: get the working directory
	*/
	QString getWorkingDir();
	
	/**
	*	@brief: set the working directory
	*/
	void setWorkingDir(QString dir);

	/**
	*	@brief: set the duration of the video to record
	*/
	void setRecordDuration(QTime duration);
	
	/**
	*	@brief: get the duration of the video to record
	*/
	QTime & getRecordDuration();

	/**
	*	@brief: set to enable the video recording
	*/
	void setRecordROI(bool val);
	
	/**
	*	@brief: get the boolean to allow the video recording
	*/
	bool getRecordROI();

public slots:
	/** Signal: from each DetectionType, when something changes
	*	@brief: signal is sent to Session which updates the GUI
	*/
	void emitChanged();
	
	/** Signal: from each DetectionType, when the alarm changes
	*	@brief: signal is sent to Session which updates the GUI
	*/
	void emitAlarmChanged();

signals:
	/**
	*  @brief: emitted signal to indicate model change
	*/
	void changed();
	
	/**
	*	@brief: emitted signal to indicate alarm change
	*/
	void alarmChanged();

protected:
	rectangle						m_bounds;					//!<bounds of the rectangle's ROI

	detectionType					m_detectionType;			//!<current detection type
	int								m_videoDeviceId;			//!<id of the current device
	VideoDevice *					ptr_device;					//!<pointer to the VideoDevice

	bool							m_isActive;					//!<bool if ROI is active
	bool							m_alarmOnEquals;			//!<bool for the alarmOnEquals param
	bool							m_processStarted;			//!<bool if process is started

	float							m_tolerance;				//!<value of the tolerance

	int								m_repetitionCount;			//!number of alarm repetition
	int								m_filtration;				//!<value of filtration

	bool							m_debug;					//!<boolean to display data to the user

	Mat								m_roiImage;					//!<the image the ROI is working with
	bool							m_recordROI;				//!<bool to record the video

	QString							m_workingDir;				//!<path towards the working directory
	QTime							m_recordDuration;			//!<duration of the video recording
	
	DetectionType *					p_currentDetection;			//!<Pointer to Current type of detection (ie ColorDetection)
	DetectionType *					m_detectionTypeArray[5];	//!<Array of detection type
};
#endif
