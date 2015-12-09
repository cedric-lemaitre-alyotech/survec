#include "SymbolDetection.h"
#include <QDebug>
#include <QDateTime>
#include <opencv2/opencv.hpp>
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/nonfree/features2d.hpp"
#include <VideoDeviceManager.h>

#define C1 (float) (0.01*255*0.01*255)
#define C2 (float) (0.03*255*0.03*255)

using namespace cv;

SymbolDetection::SymbolDetection()
{	
	qDebug() << ">CONSTRUCTOR: Symbol";
	m_alarm = false;
	m_alarmOnEquals = true;
	m_symbol = NULL;
	m_tolerance = 100;
	m_target = "";
	m_debug = false;
}

SymbolDetection::~SymbolDetection()
{	
}

void SymbolDetection::process(Mat &image)
{	
	QMutexLocker mutexLocker(&m_mutex);
	try
	{
		setAlarmValid(true);

		Size size(m_symbol.cols, m_symbol.rows);
		int nbPixelsTotal = size.height * size.width;
		if (nbPixelsTotal == 0 || m_nbPixelsMask == 0)
		{
			qDebug() << "No pixels for symbol detection";
			return;
		}
		resize(image, image, size);

		cv::Mat dst, image_grey, image_erode, image_dilate;

		cvtColor(image, image_grey, CV_RGB2GRAY);
		cv::GaussianBlur(image_grey, image_grey, cv::Size(11, 11), 3);						// Blur to remove noise
		threshold(image_grey, dst, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);		// Threshold to make black and white

		int size2 = 3;
		cv::Mat element2 = getStructuringElement(MORPH_RECT, Size(size2, size2));
		erode(dst, image_erode, element2);
		erode(dst, image_dilate, element2);

		//imshow("symbol1 ", dst);
		//cvWaitKey();

		cv::Mat diffOutMask, diffInMask;
		cv::multiply(image_erode, m_recordedSymbolMask, diffOutMask);
		cv::multiply(image_dilate, m_recordedSymbolMaskInvert, diffInMask);
		if (m_debug)
		{
			cv::Mat displayDebugImage(size.height * 2, size.width * 2, m_recordedSymbolMask.type());
			Mat img1(displayDebugImage, Rect(0, 0, size.width, size.height));
			m_recordedSymbolMask.copyTo(img1);
			Mat img2(displayDebugImage, Rect(size.width, 0, size.width, size.height));
			image_grey.copyTo(img2);
			Mat img3(displayDebugImage, Rect(0, size.height, size.width, size.height));
			diffOutMask.copyTo(img3);
			Mat img4(displayDebugImage, Rect(size.width, size.height, size.width, size.height));
			diffInMask.copyTo(img4);
			imshow("symbol res", displayDebugImage);
		}

		int nbPixelsOut = cv::countNonZero(diffOutMask);
		int nbPixelsIn = cv::countNonZero(diffInMask);

		float percentOut = ((float)nbPixelsOut * 100.0) / m_nbPixelsMask;
		float percentIn = 0;
		if (m_nbPixelsMask < nbPixelsTotal)
			percentIn = ((float)nbPixelsIn * 100.0) / (nbPixelsTotal - m_nbPixelsMask);

		qDebug() << "symbol detect percentOut=" << percentOut;
		qDebug() << "symbol detect percentIn=" << percentIn;


		float valueOut = percentOut * 10.0; // increase difference for tolerance comparison (due to background pixels)
		float valueIn = 100.0 - percentIn; 

		if (valueOut < m_tolerance && valueIn < m_tolerance)
		{
			// symbol is detected
			if (m_alarmOnEquals)
				setAlarm(true);
			else
				setAlarm(false);
		}
		else
		{
			// symbol is not detected
			if (m_alarmOnEquals)
				setAlarm(false);
			else
				setAlarm(true);
		}
	}
	catch (Exception e)
	{
		qDebug() << QString("Exception in process SymbolDetection : %1").arg(QString::fromStdString(e.msg));
	}

}

cv::Mat SymbolDetection::getSymbol() const
{
	return m_symbol;
}

void SymbolDetection::recordSymbol()
{
	cv::Mat dst, image_grey, image_grey_res;

	cvtColor(m_symbol, image_grey, CV_RGB2GRAY);
	cv::GaussianBlur(image_grey, image_grey_res, cv::Size(11, 11), 3);						// Blur to remove noise

	// Measure range of greyscale
	double min, max;
	cv::minMaxIdx(image_grey_res, &min, &max);

	if ((max - min) <= 30) // if only one color in image
		threshold(image_grey_res, image_grey_res, max, 255, CV_THRESH_BINARY);		// Threshold to make black all black if image is empty (nothing to read)

	int size2 = 3;
	cv::Mat element2 = getStructuringElement(MORPH_RECT, Size(size2, size2));
	dilate(image_grey_res, image_grey_res, element2);									// Dilate to fill in the edge

	threshold(image_grey_res, m_recordedSymbolMask, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);		// Threshold to make black and white

	threshold(image_grey_res, m_recordedSymbolMaskInvert, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);

	m_nbPixelsMask = cv::countNonZero(m_recordedSymbolMask);

}

void SymbolDetection::setSymbol(cv::Mat & mat)
{
	m_symbol = mat;
	recordSymbol();
	emit changed();
}

void SymbolDetection::setAlarmOnEquals(bool value)
{
	m_alarmOnEquals = value;
}

bool SymbolDetection::getAlarmOnEquals()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_alarmOnEquals;
}

void SymbolDetection::setTolerance(float value)
{
	m_tolerance = value - (value/2);
}
