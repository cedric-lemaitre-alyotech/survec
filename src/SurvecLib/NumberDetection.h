#ifndef NUMBERDETECTION_H
#define NUMBERDETECTION_H
//
#include "Survec_dll.h"
#include "DetectionType.h"
//
#include <QObject>
#include <QColor>
#include <opencv2/core/core.hpp>
//
namespace tesseract
{
	class TessBaseAPI;
}
//

using namespace cv;

class SURVECLIB_EXPORT NumberDetection: public DetectionType
{

public:

	typedef enum
	{
		SUP = 0,
		INF,
		EQUALS,
		NEQUALS
	} operatorType;

	/**
	*	Constructor
	*/
	NumberDetection();

	/**
	*	Destructor
	*/
	~NumberDetection();

	/**
	*	@brief: virtual method from DetectionType to implement
	*	@param cv::Mat image: the image to process
	*/
	void				process(cv::Mat &image);

	/**
	*	@brief: set the numbe to detect
	*/
	void				setNumber(double value);

	/**
	*	@brief: get the number to detect
	*/
	double				getNumber() const;

	/**
	*	@brief: get current detected value
	*/
	double				getCurrent();

	/**
	*	@brief: get target to detect
	*/
	double				getTarget();

	/**
	*	@brief: set target to detect
	*/
	void				setTarget(QString value);

	/**
	*	@brief: set the operator (<, >, =, !=, =>, <=)
	*/
	void				setOperator(operatorType op);

	/**
	*	@brief: get the operator
	*/
	operatorType		getOperator() const;

	/**
	*	@brief: set the QString of the operator (for the label)
	*/
	void				setOperatorString(QString operatorString);

	/**
	*	@brief: get the QString of the operator
	*/
	QString				getOperatorString() const;

	/**
	*	@brief: get the operator for the XML file
	*/
	QString				getOperatorXML();

	/**
	*	@brief: get the index of the font
	*/
	int					getFontIndex();

	/**
	*	@brief: set the font index
	*	@param font: a QString of the font (auto, digital7, etc.)
	*/
	void				setFont(QString font);

	/**
	*	@brief: get font as a QString
	*/
	QString				getFontAsQString();

	/**
	*	@brief: set the font index
	*	@param index: the combobox index of the font (0 =auto, 1 = digital7, etc)
	*/
	void				setFontIndex(int index);

	/**
	*	@brief: set tesseract NULL, forcing to call initTesseract again
	*/
	void				setTesseractNull();

protected:

	/**
	*	@brief: initialize tesseract with a language and different variable (blacklist, whitelist, noise size, etc.)
	*/
	void				initTesseract();

protected:
	double						m_number;				//!<number to detect
	double						m_currentValue;			//!<currently detected value
	int							m_fontIndex;			//!<index of the font
	QString						m_operatorString;		//!<string of the operator
	operatorType				m_operator;				//!<current operator
	
	tesseract::TessBaseAPI *	p_tesseract;			//!<pointer to tesseract-ocr

};
#endif
