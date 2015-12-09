#ifndef THREAD_H
#define THREAD_H

#include "Survec_dll.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <ROI.h>
#include <QObject>
#include <QThread>

using namespace cv;

class SURVECLIB_EXPORT Thread: public QThread
{
	Q_OBJECT

public:
	/**
	*	Constructor
	*/
	Thread(Mat &image, ROI * roi, QObject * pParent = NULL);

	Thread();

	/**
	*	Destructor
	*/
	~Thread();

	/**
	*	@brief: set the ROI
	*	@param ROI: the ROI to work on
	*/
	void						setROI(ROI * roi);

protected:
	virtual void				run();		//!<the starting point of the thread
	ROI *						ptr_roi;	//!<pointer to the ROI to work on
};
#endif
