#ifndef SYMBOLDETECTION_H
#define SYMBOLDETECTION_H
//
#include "Survec_dll.h"
#include "DetectionType.h"
//
#include <QObject>
#include <QColor>
#include <QPixmap>
#include <opencv2/core/core.hpp>

#include <cstdlib>
//
using namespace cv;

class SURVECLIB_EXPORT SymbolDetection: public DetectionType
{
public:
	/**
	*	Constructor
	*/
	SymbolDetection();

	/**
	*	Destructor
	*/
	~SymbolDetection();

	/**
	*	@brief: virtual method of DetectionType
	*	starts the process of detection
	*	@param cv::Mat image: the image to process
	*/
	void				process(Mat &image);

	/**
	*	@brief: record image as the symbol
	*/
	void				recordSymbol();
	/**
	*	@brief: gets the symbol to detect
	*/
	cv::Mat				getSymbol() const;

	/**
	*	@brief: sets the symbol to detect
	*/
	void				setSymbol(cv::Mat & mat);

	/**
	*	@brief: set the "is" or "is not" value
	*/
	void				setAlarmOnEquals(bool value);

	/**
	*	@brief: get the "is" or "is not" value
	*/
	bool				getAlarmOnEquals();

	/**
	*	@brief: measure the PSNR of two images
	*	@param img1, img2: the two images to compare
	*/
	double				getPSNR(const Mat &img1, const Mat &img2);

	/**
	*	@brief: The SSIM returns the MSSIM of the images. This is too a float number between zero and one (higher is better)
	*	however we have one for each channel. Therefore, we return a Scalar OpenCV data structure
	*	@param img1, img2: the two images to compare
	*/
	Scalar				getMSSIM(const Mat& img1, const Mat& img2);

	/**
	*	@brief: measure the difference (in percentage) between the edge of the param image and m_symbol
	*	@param Mat & image: the image to compare to the symbol
	*	@return float: the percentage of difference between the two image (100% means no edge are similar)
	*/
	float				edgeDetection(Mat & image);

	/**
	*	@brief: counts the number of white pixels
	*	@param Mat & image: the image to count the white pixels in
	*	@return int: the number of white pixels
	*/
	int					countWhite(Mat & image);

	/**
	*	@brief: set the tolerance
	*	@param value: the value of the tolerance
	*/
	void				setTolerance(float value);
	
protected:
	cv::Mat				m_symbol;			//!<the source image to detect
	cv::Mat				m_recordedSymbolMask;	//!<the mat symbol mask used for detection
	cv::Mat				m_recordedSymbolMaskInvert;	//!<the mat symbol mask used for detection
	int					m_nbPixelsMask;
	bool				m_alarmOnEquals;	//!<boolean if the alarm should be thrown on "is" or "is not"
	float				m_tolerance;		//!<tolerance in the detection

	QString				m_target;			//!<image path to the targeted symbol
};
#endif
