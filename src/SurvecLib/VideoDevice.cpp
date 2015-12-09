#include "VideoDevice.h"
#include <Tools.h>
#include <QDebug>
#include <QPixmap>
#include <QDebug>
#include <QImage>
#include <QFile>
#include <QTime>
#include <QFileInfo>
#include <VideoDeviceManager.h>

using namespace cv;

VideoDevice::VideoDevice()
{
	QImage im(":/resources/resources/icons/ecran_espionne.png");
	m_image = Tools::QImageToCvMat(im);	

	init(0);
}

VideoDevice::VideoDevice(int id)
{
	init(id);
}

VideoDevice::~VideoDevice()
{
	qDebug() << "VideoDevice::~VideoDevice()";
	m_cap.release();
}

void VideoDevice::init(int id)
{
	m_usedViewCount = 0;
	m_id = id;
	m_countFPS = 0;
	m_imageIndex = 0;
	p_time = new QTime();
	p_time->start();
	qDebug() << "videoDevice constructor: id			= " << m_id;
	//qDebug() << "VideoDevice constructor: pixmap size	: " << m_image.cols << "x" << m_image.rows;

	// Update during detection
	p_timer = new QTimer();
	p_timer->setInterval(50);
	// UpdateGUI every 50ms
	connect(p_timer, SIGNAL(timeout()), this, SLOT(readImage()));
	p_timer->stop();

}

void VideoDevice::connectWebcam()
{
	QMutexLocker mutexLocker(&m_mutex);
	qDebug() << "connect";
	
	m_cap.open(m_id);
	qDebug() << ">INFO: Connect id: " << m_id;
	
	if (!m_cap.isOpened())
	{
		m_cap = VideoCapture(m_id);
	}
	
	m_cap.set(CV_CAP_PROP_FRAME_WIDTH, 1920);
	m_cap.set(CV_CAP_PROP_FRAME_HEIGHT, 1080);
}

void VideoDevice::readImage()
{
	if (m_cap.isOpened())
	{
		//qDebug() << "elapsed " << p_time->elapsed();
		m_cap.read(m_image);
		m_countFPS++;
		m_imageIndex++;

	
		if (m_countFPS == 10)
		{
			m_countFPS = 0;
			countFPS();
		}
	
		if (!m_image.empty())
		{
			cvtColor(m_image, m_image, CV_BGR2RGB);
			//qDebug() << "=== Read image : " << m_imageIndex;
			if (m_imageReady.available() == 0)
				m_imageReady.release();		
			emit imageReaded();
		}
		else
		{
			qDebug() << "--> VideoDevice::readImage: IMAGE NOT VALID";
		}
	}
}

void VideoDevice::countFPS()
{
	int t = p_time->elapsed();
	int fps = (10000) / p_time->elapsed();
	
	p_time = new QTime();
	p_time->start();	

	emit frameRate(fps);
}

int VideoDevice::getId()
{
	return m_id;
}

void VideoDevice::setId(int id)
{
	if (id != m_id)
	{
		qDebug() << "=== VideoDevice setId: " << id;
		m_id = id;
	}
}

VideoCapture VideoDevice::getCap()
{
	return m_cap;
}

void VideoDevice::getImage(Mat & image)
{
	image = m_image;
}

Mat VideoDevice::getImage()
{
	//QMutexLocker mutexLocker(&m_mutex);
	if (m_image.empty())
	{
		qDebug() << "";
		qDebug() << "";
		qDebug() << "---------------------------------";
		qDebug() << "- VideoDevice::getImage() EMPTY -";
		qDebug() << "---------------------------------";
	}
	return m_image;
}

int VideoDevice::getImageWidth()
{
	return m_image.size().width;
}

int VideoDevice::getImageHeight()
{
	return m_image.size().height;
}

ColorToDetect& VideoDevice::getColorToDetect()
{
	return m_colors;
}

QMutex&	VideoDevice::getMutex()
{
	return m_mutex;
}

QSemaphore&	VideoDevice::getImageReady()
{
	return m_imageReady;
}

long VideoDevice::imageIndex()
{
	return m_imageIndex;
}

void VideoDevice::attachView()
{
	m_usedViewCount++;
	if (m_usedViewCount > 0)
	{
		// start the acquisition if there is a view
		if (p_timer->isActive() == false)
			p_timer->start();
	}
}

void VideoDevice::detachView()
{
	if (m_usedViewCount > 0)
		m_usedViewCount--;

	if (m_usedViewCount <= 0)
	{
		if (p_timer->isActive() == true)
			p_timer->stop();
	}
}

