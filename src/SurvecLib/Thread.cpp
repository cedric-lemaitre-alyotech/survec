#include "Thread.h"
#include <QDebug>
#include <ColorDetection.h>
#include <QTime>

Thread::Thread(Mat &image, ROI * roi, QObject * pParent): QThread(pParent)
{	
	qDebug() << "Thread: constructor";

	ptr_roi = roi;
}

Thread::Thread()
{
	qDebug() << "Thread: default constructor";
}

Thread::~Thread()
{
}

void Thread::run()
{
	if (ptr_roi)
	{
		ROI::detectionType type = ptr_roi->getCurrentDetectionType();
		ptr_roi->getDetectionType(type)->process(ptr_roi->getImage());
	}
}

void Thread::setROI(ROI * roi)
{
	ptr_roi = roi;
}