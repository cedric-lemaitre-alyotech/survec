#ifndef TEXTDETECTION_H
#define TEXTDETECTION_H
//
#include "Survec_dll.h"
#include "DetectionType.h"
//
#include <QColor>
//
#include <opencv2/opencv.hpp>
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"

namespace tesseract
{
	class TessBaseAPI;
}
//
class SURVECLIB_EXPORT  TextDetection: public DetectionType
{

public:
	/**
	*	Constructor
	*/
	TextDetection();

	/**
	*	Destructor
	*/
	~TextDetection();

	/**
	*	@brief: virtual method of DetectionType
	*	starts the process of detection
	*/
	void				process(cv::Mat &image);

	/**
	*	@brief: gets the text to detect
	*/
	QString				getTextToDetect() const;

	/**
	*	@brief: sets the text to detect
	*/
	void				setTextToDetect(QString text);

	/**
	*	@brief: get the current text to detect
	*/
	QString				getCurrent();

	/**
	*	@brief: set the "is" or "is not" value
	*/
	void				setAlarmOnEquals(bool value);

	/**
	*	@brief: get the "is" or "is not" value
	*/
	bool				getAlarmOnEquals();

	/**
	*	@brief: get the index of the font
	*/
	int					getFontIndex();

	/**
	*	@brief: set the font index
	*	@param index: the combobox index of the font (0 =auto, 1 = digital7, etc)
	*/
	void				setFontIndex(int index);

	/**
	*	@brief: set the font index
	*	@param font: a QString of the font (auto, digital7, etc.)
	*/
	void				setFont(QString font);
	
	/**
	*	@brief: initialize tesseract with a language and different variable (blacklist, whitelist, noise size, etc.)
	*/
	void				initTesseract();

	/**
	*	@brief: set tesseract NULL, forcing to call initTesseract again
	*/
	void				setTesseractNull();

	/**
	*	@brief: get target to detect
	*/
	QString				getTarget();

	/**
	*	@brief: set target to detect
	*/
	void				setTarget(QString value);

	/**
	*	@brief: get font as a QString
	*/
	QString				getFontAsQString();

protected:
	QString						m_text;					//!<the text to detect
	QString						m_currentValue;			//!<currently detected value
	int							m_fontIndex;			//!<index of the font
	bool						m_alarmOnEquals;		//!<boolean to detect whether and alarm should be thrown on the "is" or on the "is not"
	tesseract::TessBaseAPI *	p_tesseract;			//!<pointer to tesseract-ocr

	QString						m_workingDir;			//!<working directory
};
#endif
