#include "NumberDetection.h"
#include <QDebug>
#include <opencv2/opencv.hpp>
#include <QDateTime>
#include <QDir>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
using namespace cv;

NumberDetection::NumberDetection()
{	
	qDebug() << ">CONSTRUCTOR: Number";

	m_operator = (NumberDetection::operatorType) 0;
	m_operatorString = "";

	m_number = 0;
	m_currentValue = 0;

	m_fontIndex = 0;
	p_tesseract = NULL;
}

NumberDetection::~NumberDetection()
{
	delete p_tesseract;
}

void NumberDetection::process(Mat &image)
{
	QMutexLocker mutexLocker(&m_mutex);
	try
	{
		bool ok = false;

		if (p_tesseract == NULL)
			initTesseract();
		if (p_tesseract != NULL)
		{
			tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(7); // treat the image as a single text line
			p_tesseract->SetPageSegMode(pagesegmode);

			Mat dst, image_grey, image_grey_blurred, image_grey_res, image_otsu_thresh, image_otsu_thresh_blurred, image_erode;
			Mat element, element2;
			cvtColor(image, image_grey, CV_RGB2GRAY);

			// Case where image is a landscape
			float ratio = (float)image_grey.cols / (float)image_grey.rows;
			if (ratio >= 2)
				resize(image_grey, image_grey, Size(200 * ratio, 200), 0, 0, CV_INTER_CUBIC | CV_WINDOW_KEEPRATIO);

			// Case where image is a square
			else if (ratio < 2)
				resize(image_grey, image_grey, Size(image_grey.cols * ratio * 2, image_grey.rows * ratio * 2), 0, 0, CV_INTER_CUBIC | CV_WINDOW_KEEPRATIO);

			// Case where image is a portrait
			else if (((float)image_grey.rows / (float)image_grey.cols) > 2)
				resize(image_grey, image_grey, Size(image_grey.rows * ratio, 800), 0, 0, CV_INTER_CUBIC | CV_WINDOW_KEEPRATIO);

			cv::GaussianBlur(image_grey, image_grey_blurred, cv::Size(13, 13), 3);

			if (m_fontIndex == 0)		// --> english
			{
				double min, max;
				cv::minMaxIdx(image_grey_blurred, &min, &max);
				if ((max - min) <= 30)	// if only one color in image
					threshold(image_grey_blurred, image_grey_blurred, max, 255, CV_THRESH_BINARY);		// Threshold to make black all black if image is empty (nothing to read)

				threshold(image_grey_blurred, dst, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);		// Threshold to make black and white
			}
			else if (m_fontIndex == 1)	// --> digital 7
			{
				threshold(image_grey_blurred, image_otsu_thresh, 128, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
				int size = 5;
				Mat element = getStructuringElement(MORPH_RECT, Size(size, size));
				erode(image_otsu_thresh, image_otsu_thresh_blurred, element);

				threshold(image_otsu_thresh_blurred, dst, 128, 255, CV_THRESH_BINARY);
			}
			else						// --> PR4G
			{
				image_grey_blurred.convertTo(image_grey_blurred, -1, 0.3, 70);

				threshold(image_grey_blurred, image_otsu_thresh, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);		// Threshold to make black and white

				int size = 5;
				element = getStructuringElement(MORPH_RECT, Size(size, size));
				erode(image_otsu_thresh, image_otsu_thresh, element);

				Mat img_dilate;
				int size2 = 20;
				element2 = getStructuringElement(MORPH_RECT, Size(size2, size2));
				dilate(image_otsu_thresh, dst, element2);
			}

			if (m_debug)
			{
				imshow("res number", dst);
			}

			p_tesseract->SetImage((uchar*)dst.data, dst.size().width, dst.size().height, dst.channels(), dst.step1());


			// Get OCR result
			QLocale locale(QLocale::French);

			QString res = QString::fromUtf8(p_tesseract->GetUTF8Text());

			if(m_debug)
			{
				qDebug() << "res : " << res;
				qDebug() << "m_number : " << m_number;
				qDebug() << "currentValue : " << m_currentValue;
			}

			res = res.simplified();
			res = res.remove(" ");

			if (locale.language() == 37) // French
			{
				if (res.contains("."))
				{
					res = res.remove(",");
					res = res.replace(".", locale.decimalPoint());
				}
			}
			res = res.remove(QRegExp(QString::fromUtf8("[^\\d|\\.|\\,|\\-]")));	// Remove all non-digit, '.' , and ',' from the result

			// If res contains a minus symbol, doesn't recognize text (unless in the first position)
			// ie: -7 is OK but 12-7 gives error
			if (res.contains("-"))
			{
				if (res.count("-") == 1)
				{
					if (res.at(0) == "-")
						m_currentValue = locale.toDouble(res, &ok);
					else
						m_currentValue = std::numeric_limits<double>::quiet_NaN();
				}
				else
					m_currentValue = std::numeric_limits<double>::quiet_NaN();
			}
			else
				m_currentValue = locale.toDouble(res, &ok);

			m_alarmValid = true;
			// If no number is read, alarm (when letters are read, alarm)
			if (res == "" || !ok)
			{
				m_currentValue = std::numeric_limits<double>::quiet_NaN();
				qDebug() << " Number detect NaN";
				m_alarm = false;
				m_alarmValid = false;
			}
			else if (ok)
			{
				qDebug() << "m_operator\t: " << m_operator << "\t currentValue: " << m_currentValue << "\t m_number: " << m_number;
	
				switch (m_operator)
				{
				case 0:
					if (m_currentValue > m_number)	// m_operator = SUP
					{
						setAlarm(true);
						break;
					}
					else
						setAlarm(false);
					break;
				case 1:
					if (m_currentValue < m_number)	// m_operator = INF
					{
						setAlarm(true);
						break;
					}
					else
						setAlarm(false);
					break;
				case 2:
					if (m_currentValue == m_number)	// m_operator = EQUALS
					{
						setAlarm(true);
						break;
					}
					else
						setAlarm(false);
					break;
				case 3:
					if (m_currentValue != m_number)	// m_operator = NEQUALS
					{
						setAlarm(true);
						break;
					}
					else
						setAlarm(false);
					break;
				default:
					break;
				}
			}
		}
	}
	catch (Exception e)
	{
		qDebug() << QString("Exception in process NumberDetection : %1").arg(QString::fromStdString(e.msg));
	}

}

void NumberDetection::initTesseract()
{
	qDebug() << "NumberDetection::initTesseract " << m_fontIndex;
	m_alarm = false;
	p_tesseract = new tesseract::TessBaseAPI();

	switch (m_fontIndex)
	{
	case 0:
		p_tesseract->SetVariable("tessedit_char_whitelist", "0123456789-,.");
		p_tesseract->SetVariable("tessedit_char_blacklist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmopqrstuvwxyz&é\"'(è_çà)=$^*ù£¨µ%¤~#{[|`\^@]}+§/!:;?");
		p_tesseract->Init(NULL, "eng");
		break;
	case 1:
		p_tesseract->SetVariable("tessedit_char_whitelist", "0123456789-");
		p_tesseract->SetVariable("tessedit_char_blacklist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmopqrstuvwxyz&é\"'(è_çà)=$^*ù£¨µ%¤~#{[|`\^@]}+§/!:;?");
		p_tesseract->Init(NULL, "d7erode");
		break;
	default:
		p_tesseract->SetVariable("tessedit_char_whitelist", "0123456789-");
		p_tesseract->SetVariable("tessedit_char_blacklist", "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmopqrstuvwxyz&é\"'(è_çà)=$^*ù£¨µ%¤~#{[|`\^@]}+§/!:;?");
		p_tesseract->Init(NULL, "pr4gerode");
		break;
	}
}

// for when the font is changed, we want to init tesseract again
void NumberDetection::setTesseractNull()
{
	p_tesseract = NULL;
}

void NumberDetection::setNumber(double number)
{
	QMutexLocker mutexLocker(&m_mutex);
	if (m_number != number)
	{
		m_number = number;
		emit changed();
	}	
}

double NumberDetection::getNumber() const
{
	return m_number;
}

double NumberDetection::getCurrent()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_currentValue;
}

double NumberDetection::getTarget()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_number;
}

void NumberDetection::setTarget(QString value)
{
	m_number = value.toDouble();
	qDebug() << "NUMBER DETECTION: " << m_number;
}

void NumberDetection::setOperatorString(QString operatorString)
{
	if (m_operatorString.compare(operatorString) != 0)
	{
		if (operatorString == "sup")
			m_operatorString = ">";
		else if (operatorString == "inf")
			m_operatorString = "<";
		else if (operatorString == "equals")
			m_operatorString = "=";
		else
			m_operatorString = "≠";

		m_operatorString = operatorString;
		emit changed();
	}
}

QString NumberDetection::getOperatorXML()
{
	QMutexLocker mutexLocker(&m_mutex);
	switch (m_operator)
	{
	case 0:
		return "sup";
	case 1:
		return "inf";
	case 2:
		return "equals";
	default:
		return "notequals";
	}
}

QString NumberDetection::getOperatorString() const
{
	return m_operatorString;
}

NumberDetection::operatorType NumberDetection::getOperator() const
{
	return m_operator;
}

void NumberDetection::setOperator(NumberDetection::operatorType op)
{
	m_operator = op;
}

int NumberDetection::getFontIndex()
{
	return m_fontIndex;
}

void NumberDetection::setFont(QString font)
{
	if (font == "auto")
		m_fontIndex = 0;
	else if (font == "digital7")
		m_fontIndex = 1;
	else
		m_fontIndex = 2;
}

QString NumberDetection::getFontAsQString()
{
	switch (m_fontIndex)
	{
	case 0:
		return "auto";
	case 1:
		return "digital7";
	default:
		return "pr4g";
	}
}

void NumberDetection::setFontIndex(int index)
{
	m_fontIndex = index;
}

