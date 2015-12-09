#include "ROIVideoWriter.h"
#include <Tools.h>
#include <QDebug>
#include <QPixmap>
#include <QDebug>
#include <QImage>
#include <QFile>
#include <QTime>
#include <QFileInfo>

using namespace cv;

ROIVideoWriter::ROIVideoWriter() : 
QObject()
{
	m_isActive = false;
	m_fps = 10;
	m_timer.setInterval(1000 / m_fps);
	connect(&m_timer, SIGNAL(timeout()), this, SLOT(updateVideo()));
	m_timer.start(); // needs to be started in a thread with a loop event (main thread or thread with custom event loop)
}

ROIVideoWriter::~ROIVideoWriter()
{
	m_timer.stop();
	qDebug() << "ROIVideoWriter::~ROIVideoWriter()";
}

void ROIVideoWriter::setImage(Mat & image)
{
	if (!image.empty())
	{
		cvtColor(image, image, CV_BGR2RGB);
		m_mutex.lock();
		image.copyTo(m_image);
		m_mutex.unlock();
	}
	else
	{
		qDebug() << "--> ROIVideoWriter::readImage: IMAGE NOT VALID";
	}
}

void ROIVideoWriter::updateVideo()
{
	if (m_isActive)
	{
		if (m_videoWriter.isOpened())
		{
			m_mutex.lock();
			//imshow("video", m_image);
			m_videoWriter << m_image;
			m_mutex.unlock();
			m_imagesCount++;
			m_currentDuration += m_timer.interval() / 1000.0;
			if ((int)m_currentDuration >= m_maxDuration)
				stop();
		}
	}
}

void ROIVideoWriter::setMaxDuration(int duration)
{
	m_maxDuration = duration;
}

int ROIVideoWriter::getMaxDuration() const
{
	return m_maxDuration;
}

void ROIVideoWriter::start(const QString& fileName, Mat & image)
{
	m_currentDuration = 0.0;
	m_imagesCount = 0;

	bool temp = m_videoWriter.open(fileName.toStdString(), CV_FOURCC('M', 'P', '4', '2'), m_fps, Size(image.cols, image.rows), true);
	setImage(image);
	qDebug() << "record video " << fileName << ": " << temp;

	m_isActive = true;
}

void ROIVideoWriter::stop()
{
	if (m_isActive)
	{
		qDebug() << QString("Write video with %1 images ").arg(m_imagesCount);
		m_isActive = false;
		m_videoWriter.release();
	}
}

bool ROIVideoWriter::isOpened() const
{
	return m_videoWriter.isOpened();
}

bool ROIVideoWriter::isActive() const
{
	return m_isActive;
}