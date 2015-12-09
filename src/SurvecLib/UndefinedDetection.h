#ifndef UNDEFINEDDETECTION_H
#define UNDEFINEDDETECTION_H
//
#include "Survec_dll.h"
#include "DetectionType.h"
//
#include <QColor>
//
class SURVECLIB_EXPORT UndefinedDetection: public DetectionType
{

public:
	/**
	*	Constructor
	*/
	UndefinedDetection();

	/**
	*	Destructor
	*/
	~UndefinedDetection();

	/**
	*	@brief: virtual method of DetectionType
	*	starts the process of detection
	*/
	void				process(cv::Mat &image);

};
#endif
