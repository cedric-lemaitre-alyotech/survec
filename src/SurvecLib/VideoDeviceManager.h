#ifndef VIDEODEVICEMANAGER_H
#define VIDEODEVICEMANAGER_H

#include "Survec_dll.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QObject>
#include <QVector>
#include <VideoDevice.h>

using namespace cv;

class VideoDevice;

class SURVECLIB_EXPORT VideoDeviceManager
{

public:

	/**
	 *  Destructor
	 */
	~VideoDeviceManager();

	/**
	*	@brief: returns the instance of VideoDeviceManager (singleton)
	*/
	static VideoDeviceManager *		getInstance();

	/**
	*	@brief: deletes the instance
	*/
	static void						resetInstance();
	
	/**
	*	@brief: gets the VideoDevice list
	*/
	QVector<VideoDevice*>			getVideoDeviceList();

	/**
	*	@brief: gets the number of device in the list
	*/
	int								getNbDevice() const;

	/**
	*	@brief: gets the VideoDevice at position 'id'
	*	@param int id: the index of the device in the list
	*/
	VideoDevice*					getDevice(int id) const;

	/**
	*	@brief: display the list of devices
	*/
	void							displayList();
	
private:
	/**
	*  Constructor (singleton)
	*/
	VideoDeviceManager();
	static VideoDeviceManager *		p_instance;				//!<pointer to this current instance
	QVector<VideoDevice*>			m_vectorVideoDevice;	//!<pointer to the list of devices

};

#endif // VIDEODEVICEMANAGER_H
// @}
