#include "UndefinedDetection.h"
#include <QDebug>
#include <opencv2/opencv.hpp>

using namespace cv;

UndefinedDetection::UndefinedDetection()
{	
	qDebug() << ">CONSTRUCTOR: UndefinedDetection";

}

UndefinedDetection::~UndefinedDetection()
{	

}

void UndefinedDetection::process(Mat &image)
{
	qDebug() << "Undefinied detection: process: image size: " << image.rows << "x" << image.cols;
}