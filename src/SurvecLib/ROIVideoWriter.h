#ifndef ROIVideoWriter_H
#define ROIVideoWriter_H

#include "Survec_dll.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <QObject>
#include <QTimer>

using namespace cv;

class SURVECLIB_EXPORT ROIVideoWriter: public QObject
{
	Q_OBJECT

public:
	/**
	*	default Constructor
	*/
	ROIVideoWriter();

	/**
	 *  Destructor
	 */
	~ROIVideoWriter();

	/**
	*	@brief: reads the VideoCapture feed and returns the image
	*	@param cv::Mat: the destination image
	*/
	void					setImage(Mat & image);

	/**
	*	@brief: set the max video duration in seconds
	*/
	void					setMaxDuration(int duration);
	
	/**
	*	@brief: returns the max video duration in seconds
	*/
	int						getMaxDuration() const;

	/**
	*	@brief: start the video writer
	*/
	void					start(const QString& fileName, Mat & image);

	/**
	*	@brief: start the video writer
	*/
	void					stop();

	/**
	*   @brief: returns if the video has been open for writing
	*/
	bool					isOpened() const;

	/**
	*  @brief: returns if the video is started recording
	*/
	bool					isActive() const;

public slots:
	void					updateVideo();

protected:
	int						m_imagesCount;
	int						m_fps;
	bool					m_isActive;
	QMutex					m_mutex;
	QTimer					m_timer;
	cv::Mat					m_image;			//!<the image read by the webcam
	double					m_currentDuration;	//!< the current video duration
	int						m_maxDuration;		//!< the max video duration
	VideoWriter				m_videoWriter;		//!< OpenCV video writer encoder
};

#endif // ROIVideoWriter_H
// @}
