#include "DetectionType.h"
#include <QDebug>

using namespace cv;

DetectionType::DetectionType()
	: m_mutex(QMutex::Recursive)
{
	qDebug() << ">CONSTRUCTOR: DetectionType";

	m_alarm = false;
	m_alarmValid = false;
	m_debug = false;
}

DetectionType::~DetectionType()
{
}

//void DetectionType::setImage(Mat & image)
//{
//	qDebug() << "DetectionType: setImage";
//	m_image = image;
//}

bool DetectionType::getAlarmValid()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_alarmValid;
}

void DetectionType::setAlarmValid(bool val)
{
	QMutexLocker mutexLocker(&m_mutex);
	m_alarmValid = val;
}

bool DetectionType::getAlarm()
{
	QMutexLocker mutexLocker(&m_mutex);
	return m_alarm;
}

void DetectionType::setAlarm(bool val)
{
	QMutexLocker mutexLocker(&m_mutex);
	m_alarm = val;
}

void DetectionType::setDebug(bool val)
{
	m_debug = val;
}