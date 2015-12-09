#ifndef VIDEODEVICE_H
#define VIDEODEVICE_H

#include "Survec_dll.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QObject>
#include <ColorToDetect.h>
#include <QSemaphore>
#include <QTimer>

using namespace cv;

class SURVECLIB_EXPORT VideoDevice: public QObject
{
	Q_OBJECT;
public:
	/**
	*	default Constructor
	*/
	VideoDevice();

	/**
	*	Constructor
	*/
	VideoDevice(int id);

	/**
	 *  Destructor
	 */
	~VideoDevice();
	
	/**
	*	@brief: links the device to the webcam port
	*/
	void					connectWebcam();

	/**
	*	@brief: gets the id of the device
	*	@return: the id of the webcam
	*/
	int						getId();

	/**
	*	@brief: sets the id of the device
	*	@param id: the id of the webcam to display
	*/
	void					setId(int id);
	
	/**
	*	@brief: get the VideoCapture of the device
	*/
	VideoCapture			getCap();

	/**
	*	@brief: gets the image read by the device
	*	@param cv::Mat image: the dest image
	*/
	void					getImage(Mat & image);
	/**
	*	@brief: get the image 
	*	@return: the cv::Mat image
	*/
	Mat						getImage();

	/**
	*	@brief: get the width of the image
	*	@return: the width 
	*/
	int						getImageWidth();
	
	/**
	*	@brief: get the image height
	*	@return: the height
	*/
	int						getImageHeight();

	/**
	*	@brief: get the ColorToDetect
	*	@return: a reference to the ColorToDetect
	*/
	ColorToDetect&			getColorToDetect();

	/**
	*	@brief: counts the number of fps
	*/
	void					countFPS();

	QMutex&					getMutex();

	QSemaphore&				getImageReady();

	long					imageIndex();

	void					attachView();

	void					detachView();

protected:
	void					init(int id);

signals:
	/**
	*	@brief: emitted when the user changes the webcam to display
	*/
	void					webcamChanged();
	
	/**
	*	@brief: emitted every 10 frames
	*	@param: int the number of fps
	*/
	void					frameRate(int);

	void					imageReaded();

public slots:
	void					readImage();

protected:
	cv::VideoCapture		m_cap;				//!<the VideoCapture
	int						m_id;				//!<the id of the webcam
	cv::Mat					m_image;			//!<the image read by the webcam
	ColorToDetect			m_colors;			//!<to save  the colors to detect (from ColorEdit and ROIConfig)
	QTime*					p_time;				//!<to measure FPS
	long					m_imageIndex;
	int						m_countFPS;			//!<to measure FPS
	QMutex					m_mutex;			//!< synchro read image and process (getImage)
	QSemaphore				m_imageReady;
	QTimer *				p_timer;		//!<timer to update the webcam image
	int						m_usedViewCount;
	bool					m_pause;
};

#endif // VIDEODEVICE_H
// @}
