#ifndef COLORDETECTION_H
#define COLORDETECTION_H
//
#include "Survec_dll.h"
#include "DetectionType.h"
//
#include <QObject>
#include <QColor>
#include <ColorToDetect.h>
//
class VideoDevice; // forward declaration 

class SURVECLIB_EXPORT ColorDetection: public DetectionType
{

public:
	/**
	*	Constructor
	*/
	ColorDetection();

	/**
	*	Destructor
	*/
	~ColorDetection();


	/**
	*	@brief: abstract method from DetectionType
	*	process the image and write result
	*	@param cv::Mat image: the dest image
	*/
	void				process(cv::Mat &image);

	void				colorToDetectModified(ColorToDetect colors);


	/**
	*	@brief: get the color to detect
	*	@return: the color to detect
	*/
	QColor				getColor() const;

	/**
	*	@brief: set the color to detect
	*	@param: color: the color to detect
	*/
	void				setColor(QColor color);

	/**
	*	@brief: get the combobox index of the color to detect
	*/
	int					getIndexColor() const;

	/**
	*	@brief: set the combobox index of the color to detect
	*/
	void				setIndexColor(int indexColor);
	
	/**
	*	@brief: set the "is" or "is not" value
	*/
	void				setAlarmOnEquals(bool value);

	/**
	*	@brief: get the "is" or "is not" value
	*/
	bool				getAlarmOnEquals();

	/**
	*	@brief: set the interval of color (minColor and maxColor)
	*/
	void				setInterval();

	/**
	*	@brief: counts the number of white pixels
	*	@param Mat & image: the image to count the white pixels in
	*	@return int: the number of white pixels
	*/
	int					countWhite(cv::Mat & image);

	/**
	*	@brief: set the minColor
	*/
	void				setMinColor(cv::Scalar color);

	/**
	*	@brief: set the maxColor
	*/
	void				setMaxColor(cv::Scalar color);

	/**
	*	@brief: set the tolerance
	*/
	void				setTolerance(float value);

	/**
	*	@brief: get the current color to detect
	*/
	QColor				getCurrentColor();

	/**
	*	@brief: set the current color to detect
	*	@param r, g, b: the RGB color code
	*/
	void				setCurrentColor(int r, int g, int b);

	/**
	*	@brief: set the target color
	*/
	void				setTarget(QString color);

protected:
	QColor				m_color;			//!<the color to detect
	QColor				m_currentColor;		//!<current color to detect
	cv::Scalar			m_minColor;			//!<min color of the interval to detect
	cv::Scalar			m_maxColor;			//!<max color of the interval to detect
	int					m_indexColor;		//!<the index of the combobox corresponding to the color to detect

	bool				m_alarmOnEquals;	//!<boolean to detect whether and alarm should be thrown on the "is" or on the "is not"

	float				m_tolerance;		//!<the value of tolerance


	bool				m_isColor;			//!<1 means the current color is in color (not grayscale), 0 means its grayscale
};
#endif
