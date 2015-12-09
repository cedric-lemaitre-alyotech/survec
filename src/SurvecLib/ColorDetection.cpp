#include "ColorDetection.h"
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <ColorToDetect.h>
#include <QDateTime>

using namespace cv;

ColorDetection::ColorDetection()
{	
	qDebug() << ">CONSTRUCTOR: ColorDetection";
	
	m_indexColor = 0;
	m_tolerance = 10;
	m_alarm = false;
	m_alarmOnEquals = true;
	m_currentColor = QColor(128, 128, 128); 
	m_debug = false;
	m_isColor = true;
}

ColorDetection::~ColorDetection()
{
}

void ColorDetection::process(Mat &image)
{
	QMutexLocker mutexLocker(&m_mutex);
	try
	{
		//Mat t;
		//image.copyTo(t);
		//QColor tt = QColor(t.at<Vec3b>(0, 0)[0], t.at<Vec3b>(0, 0)[1], t.at<Vec3b>(0, 0)[2]);
	
		Scalar avg = mean(image);
		m_currentColor.setRed(avg[0]);
		m_currentColor.setGreen(avg[1]);
		m_currentColor.setBlue(avg[2]);	

		// Set the min and max color
		setInterval();

		// DEBUG value
		//int rt, gt, bt;
		//m_color.getRgb(&rt, &gt, &bt);
		//int ht, st, vt;
		//m_color.getHsv(&ht, &st, &vt);

		//int rt2, gt2, bt2;
		//m_currentColor.getRgb(&rt2, &gt2, &bt2);
		//int ht2, st2, vt2;
		//m_currentColor.getHsv(&ht2, &st2, &vt2);

		//int hmin = m_minColor[0];
		//int hmax = m_maxColor[0];
		//int smin = m_minColor[1];
		//int smax = m_maxColor[1];
		//int vmin = m_minColor[2];
		//int vmax = m_maxColor[2];
		// end DEBUG value


		float colorHue = m_currentColor.hue()*.5;
		float colorSaturation = m_currentColor.saturation();
		float colorValue = m_currentColor.value();

		bool isInInterval = false;
		if (m_debug)
		{
			qDebug() << "is color : " << m_isColor;
			qDebug() << "color hue : " << colorHue;
			qDebug() << "color sat : " << colorSaturation;
			qDebug() << "color val : " << colorValue;
		}

		if (m_isColor)	// if not grayscale
		{
			// test on hue 
			if (m_minColor[0] < 0 || m_maxColor[0] > 180)	// case where the hue interval goes over the range
			{
				if (m_minColor[0] < 0)						// case where minColor < 0
				{
					m_minColor[0] = 180 + m_minColor[0];
					isInInterval = ((colorHue < m_maxColor[0] && colorHue >= 0) || (colorHue <= 180 && colorHue > m_minColor[0]));
				}

				if (m_maxColor[0] > 180)					// case where maxColor < 180
				{
					m_maxColor[0] = m_maxColor[0] - 180;
					isInInterval = ((colorHue >= 0 && colorHue < m_maxColor[0]) || (colorHue > m_minColor[0] && colorHue <= 180));
				}
			}
			else	// case where the hue doesn't go over the range
				isInInterval = (colorHue <= m_maxColor[0] && colorHue >= m_minColor[0]);

			// test on saturation and value
			if (isInInterval)
			{
				if(! (colorSaturation <= m_maxColor[1] && colorSaturation >= m_minColor[1] && colorValue <= m_maxColor[2] && colorValue >= m_minColor[2]))
					isInInterval = false;
			}
		}
		else		// if grayscale
		{
			if (colorSaturation <= m_maxColor[1] && colorSaturation >= m_minColor[1] && colorValue <= m_maxColor[2] && colorValue >= m_minColor[2])
				isInInterval = true;
		}

		if (m_debug)
			qDebug() << "color in interval : " << isInInterval;

		setAlarmValid(true);
		if (isInInterval)
		{
			if (m_alarmOnEquals)
				setAlarm(true);
			else
				setAlarm(false);
		}
		else
		{
			if (m_alarmOnEquals)
				setAlarm(false);
			else
				setAlarm(true);
		}
	}
	catch (Exception e)
	{
		qDebug() << QString("Exception in process ColorDetection : %1").arg(QString::fromStdString(e.msg));
	}

}

void ColorDetection::colorToDetectModified(ColorToDetect colors)
{
	m_color = colors.getIndexColor(m_indexColor);
}

int ColorDetection::countWhite(Mat & image)
{
	Mat img = image.clone();
	int cnt = 0;
	for (int y = 0; y < img.rows; y++)
	{
		for (int x = 0; x < img.cols; x++)
		{
			if (img.at<uchar>(y, x) == 255)
			{
				cnt++;
			}
		}
	}
	return cnt;
}

void ColorDetection::setInterval()
{
	int h = m_color.hue()*.5;		//hsv.val[0];
	int s = m_color.saturation();	//hsv.val[1];
	int v = m_color.value();		//hsv.val[2];
	
	if (m_debug)
		qDebug() << "m_color hsv : " << h << "/" << s << "/" << v;

	// if grey scale (grey, white or black): H must be ignored
	if (v < 40)							//black
	{
		m_isColor = false;
		setMinColor(Scalar(0,	0,		v - m_tolerance * 2));
		setMaxColor(Scalar(255, 255,	v + m_tolerance * 2));
	}
	else if (s < 75)					// grey or white
	{
		m_isColor = false;

		setMinColor(Scalar(0,	 0,		v - m_tolerance * 2));
		setMaxColor(Scalar(255, 75,		v + m_tolerance * 2));
	}
	// if color
	else if (v > 40 && s > 75)
	{
		m_isColor = true;
		setMinColor(Scalar(h - m_tolerance, s - m_tolerance * 10, v - m_tolerance * 10));
		setMaxColor(Scalar(h + m_tolerance, s + m_tolerance * 10, v + m_tolerance * 10));
	}
	else
	{
		qDebug() << "error";
	}

	if (m_debug)
	{
		qDebug() << "mincolor : " << m_minColor[0] << "/" << m_minColor[1] << "/" << m_minColor[2];
		qDebug() << "maxcolor : " << m_maxColor[0] << "/" << m_maxColor[1] << "/" << m_maxColor[2];
	}
}

void ColorDetection::setTolerance(float value)
{
	m_tolerance = 10 * (value/100);
}

void ColorDetection::setMinColor(Scalar color)
{
	m_minColor[0] = color.val[0];
	m_minColor[1] = color.val[1];
	m_minColor[2] = color.val[2];
}

void ColorDetection::setMaxColor(Scalar color)
{
	m_maxColor[0] = color.val[0];
	m_maxColor[1] = color.val[1];
	m_maxColor[2] = color.val[2];
}

QColor ColorDetection::getColor() const
{
	return m_color;
}

void ColorDetection::setColor(QColor color)
{
	QMutexLocker mutexLocker(&m_mutex);
	if (m_color != color)
	{
		m_color = color;
	}	
}

void ColorDetection::setCurrentColor(int r, int g, int b)
{
	QMutexLocker mutexLocker(&m_mutex);
	m_currentColor.setRed(r);
	m_currentColor.setGreen(g);
	m_currentColor.setBlue(b);
}

QColor ColorDetection::getCurrentColor()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_currentColor;
}

int ColorDetection::getIndexColor() const
{
	return m_indexColor;
}

void ColorDetection::setIndexColor(int indexColor)
{
	m_indexColor = indexColor;
}

void ColorDetection::setAlarmOnEquals(bool value)
{
	QMutexLocker mutexLocker(&m_mutex);
	m_alarmOnEquals = value;
}

bool ColorDetection::getAlarmOnEquals()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_alarmOnEquals;
}

void ColorDetection::setTarget(QString color)
{
	QMutexLocker mutexLocker(&m_mutex);
	m_color = QColor(color);
}
