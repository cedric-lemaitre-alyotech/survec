#ifndef COLORTODETECT_H
#define COLORTODETECT_H

#include "Survec_dll.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QColor>
#include <QVector>

using namespace cv;

class SURVECLIB_EXPORT ColorToDetect
{

public:

	enum colorListToDetect{
		red = 0,
		orange,
		yellow,
		green,
		cyan,
		blue,
		pink,
		white,
		grey,
		black
	};

	/**
	*  Constructor
	*/
	ColorToDetect();

	/**
	 *  Destructor
	 */
	~ColorToDetect();

	/**
	*	@brief: getters and setters of colors.
	*	default is predefined colors
	*/
	QColor						getRed();
	QColor						getOrange();
	QColor						getYellow();
	QColor						getCyan();
	QColor						getGreen();
	QColor						getBlue();
	QColor						getPink();
	QColor						getWhite();
	QColor						getGrey();
	QColor						getBlack();
	QColor						getCurrent();

	void						setRed(QColor red);
	void						setOrange(QColor orange);
	void						setYellow(QColor yelow);
	void						setCyan(QColor cyan);
	void						setGreen(QColor green);
	void						setBlue(QColor blue);
	void						setPink(QColor pink);
	void						setWhite(QColor white);
	void						setGrey(QColor grey);
	void						setBlack(QColor black);
	void						setCurrent(QColor current);

	void						reinit();

	/**
	*	@brief: get the corresponding color at the given index
	*	@param index: the color index
	*/
	QColor						getIndexColor(int index);

	/**
	*	@brief: get the name of the color at the given index
	*/
	QString						getIndexColorName(int index);

	/**
	*	@brief: update the color with new HSV code
	*	@param name: the name of the color to update ("red", "orange", "yellow", etc.)
	*	@param h, s, v: the HSV color code
	*/
	void						setColor(QString name, int h, int s, int v);

private:
	QVector<QColor>					m_arrayColor;	//!<array of the ten colors

	QColor							m_current;	//!<current color
};

#endif // COLORTODETECT_H
// @}
