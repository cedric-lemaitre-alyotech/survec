#ifndef _SURVECLIB_TOOLS_H
#define _SURVECLIB_TOOLS_H
//
#include "Survec_dll.h"
#include <QImage>
#include <QPixmap>
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/types_c.h"
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

namespace Tools
{
	/**
	 *	@brief: Convert a cv::Mat to a QImage
	 *	@param inMat: the cv::Mat to convert
	 */
	SURVECLIB_EXPORT QImage cvMatToQImage(const cv::Mat &inMat);

	/**
	*	@brief: Convert a cv::Mat to a QPixmap
	*	@param inMat: the cv::Mat to convert
	*/
	SURVECLIB_EXPORT QPixmap cvMatToQPixmap(const cv::Mat &inMat);

	/**
	*	@brief: Convert a QImage to a QPixmap
	*	@param inMat: the QImage to convert
	*/
	SURVECLIB_EXPORT cv::Mat QImageToCvMat(const QImage &inImage, bool inCloneImageData = true);

	/**
	*	@brief: Convert a QPixmap to a cv::Mat
	*	@param inMat: the QPixmap to convert
	*/
	SURVECLIB_EXPORT cv::Mat QPixmapToCvMat(const QPixmap &inPixmap, bool inCloneImageData = true);	
}
#endif // _SURVECLIB_TOOLS_H