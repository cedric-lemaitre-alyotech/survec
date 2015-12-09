#include "VideoDeviceManager.h"
#include <QDebug>
#include <QApplication>


using namespace cv;

VideoDeviceManager * VideoDeviceManager::p_instance = NULL;


VideoDeviceManager::VideoDeviceManager()
{	
	cv::VideoCapture temp_camera;
	int maxTested = 10;


	for (int i = 0; i < maxTested; i++)
	{
		cv::VideoCapture temp_camera(i);
		bool res = temp_camera.isOpened();
		temp_camera.release();

		if (res)
		{
			VideoDevice * pDevice = new VideoDevice(i);
			pDevice->connectWebcam();
			m_vectorVideoDevice.append(pDevice);
			qDebug() << ">INFO: Camera " << i << " detected";
		}
	}

	qDebug() << ">INFO: number of device: " << getNbDevice();
}

VideoDeviceManager::~VideoDeviceManager()
{
	// delete all the values of the vector
	for (int i = 0; i < m_vectorVideoDevice.size(); ++i)
	{
		delete m_vectorVideoDevice[i];
	}
	m_vectorVideoDevice.clear();
}

VideoDeviceManager * VideoDeviceManager::getInstance()
{
	if (p_instance == NULL)
	{
		p_instance = new VideoDeviceManager();
	}
	return p_instance;
}

void VideoDeviceManager::resetInstance()
{
	if (p_instance != NULL)
	{
		delete p_instance;
		p_instance = NULL;
	}	
}


QVector<VideoDevice*> VideoDeviceManager::getVideoDeviceList()
{
	return m_vectorVideoDevice;
}

int VideoDeviceManager::getNbDevice() const
{
	return m_vectorVideoDevice.size();
}

VideoDevice* VideoDeviceManager::getDevice(int id) const
{
	VideoDevice* temp = NULL;
	if ((id >= 0) && (id < m_vectorVideoDevice.size()))
	{
		temp = m_vectorVideoDevice.at(id);
	}
	return temp;

}

void VideoDeviceManager::displayList()
{
	for (int i = 0; i < p_instance->getNbDevice(); i++)
	{
		qDebug() << "displayList - id: " << p_instance->getDevice(i)->getId() << " / cap: " << p_instance->getDevice(i)->getCap().get(i);
	}
}