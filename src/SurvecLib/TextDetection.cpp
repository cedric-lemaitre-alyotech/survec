#include "TextDetection.h"
#include <QDebug>
#include <QDateTime>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

using namespace cv;

TextDetection::TextDetection()
{	
	qDebug() << ">CONSTRUCTOR: Text";
	m_text = "";
	m_currentValue = "";
	m_fontIndex = 0;
	m_alarm = false;
	m_alarmOnEquals = true; 
	p_tesseract = NULL;
	m_debug = false;
}

TextDetection::~TextDetection()
{
	delete p_tesseract;
}

void TextDetection::process(Mat &image)
{
	QMutexLocker mutexLocker(&m_mutex);
	try
	{
		if (p_tesseract == NULL)
			initTesseract();

		if (p_tesseract != NULL)
		{
			tesseract::PageSegMode pagesegmode = static_cast<tesseract::PageSegMode>(7); // treat the image as a single text line
			p_tesseract->SetPageSegMode(pagesegmode);

			Mat dst, image_grey, image_grey_blurred, image_grey_res, image_otsu_thresh, image_otsu_thresh_blurred, image_otsu_thresh_smooth;
			Mat element, element2;
			cvtColor(image, image_grey, CV_RGB2GRAY);

			float ratio = (float)image_grey.cols / (float)image_grey.rows;
			// Case where image is a landscape
			if (ratio >= 2)
				resize(image_grey, image_grey, Size(200 * ratio, 200), 0, 0, CV_INTER_CUBIC | CV_WINDOW_KEEPRATIO);

			// Case where image is a square
			else if (ratio < 2)
				resize(image_grey, image_grey, Size(image_grey.cols * ratio * 2, image_grey.rows * ratio * 2), 0, 0, CV_INTER_CUBIC | CV_WINDOW_KEEPRATIO);

			// Case where image is a portrait
			else if (((float)image_grey.rows / (float)image_grey.cols) > 2)
				resize(image_grey, image_grey, Size(image_grey.rows * ratio, 800), 0, 0, CV_INTER_CUBIC | CV_WINDOW_KEEPRATIO);

			cv::GaussianBlur(image_grey, image_grey_res, cv::Size(11, 11), 3);						// Blur to remove noise


			if (m_fontIndex == 0)	// --> English
			{
				// Measure range of greyscale
				double min, max;
				cv::minMaxIdx(image_grey_res, &min, &max);

				if ((max - min) <= 30) // if only one color in image
					threshold(image_grey_res, image_grey_res, max, 255, CV_THRESH_BINARY);		// Threshold to make black all black if image is empty (nothing to read)

				int size2 = 3;
				element2 = getStructuringElement(MORPH_RECT, Size(size2, size2));
				dilate(image_grey_res, image_grey_res, element2);									// Dilate too fill in the edge

				threshold(image_grey_res, dst, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);		// Threshold to make black and white
			}

			else					// --> PR4G
			{
				image_grey_res.convertTo(image_grey_res, -1, 0.3, 70);

				threshold(image_grey_res, image_otsu_thresh, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);		// Threshold to make black and white

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
				imshow("res text", dst);
			}

			p_tesseract->SetImage((uchar*)dst.data, dst.size().width, dst.size().height, dst.channels(), dst.step1());

			// Get OCR result	
			QString res = QString::fromUtf8(p_tesseract->GetUTF8Text());

			res = res.simplified();
			m_currentValue = res;

			res = res.remove(" ");

			if (m_debug)
			{
				qDebug() << "m_currentValue: " << m_currentValue;
				qDebug() << "res txt: " << res;
				qDebug() << "m_text : " << m_text;
			}

			setAlarmValid(true);

			QString temp = m_text;
			if (res.compare(temp.remove(" "), Qt::CaseInsensitive) == 0)
			{
				if (m_alarmOnEquals)
					setAlarm(true);
				else
					setAlarm(false);
			}
			else if (m_alarmOnEquals)
			{
				setAlarm(false);
			}
			else
				setAlarm(true);

		}
	}
	catch (Exception e)
	{
		qDebug() << QString("Exception in process TextDetection : %1").arg(QString::fromStdString(e.msg));
	}

}

void TextDetection::initTesseract()
{
	qDebug() << "TextDetection::initTesseract";
	m_alarm = false;
	p_tesseract = new tesseract::TessBaseAPI();
	switch (m_fontIndex)
	{
	case 0:
		//p_tesseract->SetVariable("tessedit_char_blacklist", "&é\"'(-è_çà)=$^*ù£¨µ%¤~#{[|`\^@]}+°§/!:;");
		p_tesseract->Init(NULL, "eng");
		break;
	default:
		p_tesseract->SetVariable("tessedit_char_whitelist", "0123456789ABCDEFHIJLMNOPRSTUVXYZ°*");
		p_tesseract->SetVariable("textord_max_noise_size", "15");
		//p_tesseract->SetVariable("textord_heavy_nr", "1");
		p_tesseract->Init(NULL, "pr4gerode");
		//p_tesseract->Init(NULL, "eng");
		break;
	}
}

// for when the font is changed, we want to init tesseract again
void TextDetection::setTesseractNull()
{
	p_tesseract = NULL;
}

QString TextDetection::getTarget()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_text;
}

void TextDetection::setTarget(QString value)
{
	QMutexLocker mutexLocker(&m_mutex);
	m_text = value;
}

QString TextDetection::getTextToDetect() const
{
	return m_text;
}

void TextDetection::setTextToDetect(QString text)
{
	if (m_text.compare(text) != 0)
	{
		if (!text.isNull())
		{
			m_text = text;// .remove(" ");
			emit changed();
		}
	}	
}

int TextDetection::getFontIndex()
{
	return m_fontIndex;
}

QString TextDetection::getFontAsQString()
{
	switch (m_fontIndex)
	{
	case 0:
		return "auto";
	default:
		return "pr4g";
	}
}

void TextDetection::setFontIndex(int index)
{
	m_fontIndex = index;
}

void TextDetection::setFont(QString font)
{
	if (font == "auto")
		m_fontIndex = 0;
	else
		m_fontIndex = 1;
}

QString TextDetection::getCurrent()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_currentValue;
}

void TextDetection::setAlarmOnEquals(bool value)
{
	m_alarmOnEquals = value;
}

bool TextDetection::getAlarmOnEquals()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_alarmOnEquals;
}