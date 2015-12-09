#include "Alarm.h"
#include <QDebug>
#include <QDebug>
#include <VideoDeviceManager.h>
using namespace cv;

Alarm::Alarm() : QObject()
{
	qDebug() << "=== Alarm: constructor ===";
	reset();
}

Alarm::Alarm(const Alarm& alarm) : QObject()
{
	m_date = alarm.m_date;
	m_startTime = alarm.m_startTime;
	m_duration = alarm.m_duration;
	m_status = alarm.m_status;
	m_type1 = alarm.m_type1;
	m_current1 = alarm.m_current1;
	m_target1 = alarm.m_target1;
	m_logic = alarm.m_logic;
	m_type2 = alarm.m_type2;
	m_current2 = alarm.m_current2;
	m_target2 = alarm.m_target2;
	m_imagePath1 = alarm.m_imagePath1;
	m_imagePath2 = alarm.m_imagePath2;
	m_backgroundColor1 = alarm.m_backgroundColor1;
	m_backgroundColor2 = alarm.m_backgroundColor2;
}

Alarm& Alarm::operator=(const Alarm& alarm)
{
	m_date = alarm.m_date;
	m_startTime = alarm.m_startTime;
	m_duration = alarm.m_duration;
	m_status = alarm.m_status;
	m_type1 = alarm.m_type1;
	m_current1 = alarm.m_current1;
	m_target1 = alarm.m_target1;
	m_logic = alarm.m_logic;
	m_type2 = alarm.m_type2;
	m_current2 = alarm.m_current2;
	m_target2 = alarm.m_target2;
	m_imagePath1 = alarm.m_imagePath1;
	m_imagePath2 = alarm.m_imagePath2;
	m_backgroundColor1 = alarm.m_backgroundColor1;
	m_backgroundColor2 = alarm.m_backgroundColor2;

	return *this;
}

Alarm::~Alarm()
{	
}

void Alarm::reset()
{
	m_date = "";
	m_startTime = "";
	m_duration = "";
	m_status = Alarm::AS_RUNNING;
	m_type1 = "";
	m_type2 = "";

	m_current1 = "";
	m_target1 = "";

	m_current2 = "";
	m_target2 = "";

	m_logic = L_AND;

	m_imagePath1 = "";
	m_imagePath2 = "";

	m_backgroundColor1 = "#FFFFFF";
	m_backgroundColor2 = "#FFFFFF";
}

QString Alarm::getDate() const
{
	return m_date;
}

QString Alarm::getStartTime() const
{
	return m_startTime;
}

QString Alarm::getDuration() const
{
	return m_duration;
}

QString Alarm::getStatus() const
{
	return m_status;
}

QString Alarm::getTypeROI(int indexRoi) const
{
	if (indexRoi == 1)
		return m_type1;
	else
		return m_type2;
}

QString Alarm::getTypeROI1() const
{
	return m_type1;
}

QString Alarm::getTypeROI2() const
{
	return m_type2;
}

QString Alarm::getLogic() const
{
	return m_logic;
}

QString Alarm::getCurrent(int indexRoi) const
{
	if (indexRoi == 1)
		return m_current1;
	else
		return m_current2;
}

QString Alarm::getTarget(int indexRoi) const
{
	if (indexRoi == 1)
		return m_target1;
	else
		return m_target2;
}

void Alarm::setDate(QString date)
{
	m_date = date;
}

void Alarm::setStartTime(QString startTime)
{
	m_startTime = startTime;
}

void Alarm::setDuration(QString duration)
{
	m_duration = duration;
}

void Alarm::setStatus(QString status)
{
	m_status = status;
}

void Alarm::setType(int numROI, QString type, bool onEquals)
{
	QString temp = "";
	if (onEquals)
		temp = " is";
	else
		temp = " is not";

	if (type.startsWith("Num"))
		temp = "";

	switch (numROI)
	{
	case 1:
		setTypeROI1(type + temp);
		break;
	case 2:
		setTypeROI2(type + temp);
		break;
	default:
		setTypeROI2("Undefined");
		break;
	}
}

void Alarm::setType(int indexRoi, QString type)
{
	switch (indexRoi)
	{
	case 1:
		setTypeROI1(type);
		break;
	case 2:
		setTypeROI2(type);
		break;
	default:
		setTypeROI2("Undefined");
		break;
	}
}

void Alarm::setTypeROI1(QString typeROI1)
{
	m_type1 = typeROI1;
}

void Alarm::setTypeROI2(QString typeROI2)
{
	m_type2 = typeROI2;
}

void Alarm::setLogic(QString logic)
{
	m_logic = logic;
}

void Alarm::setCurrent1(QString value)
{
	m_current1 = value;
}

void Alarm::setCurrent2(QString value)
{
	m_current2 = value;
}

void Alarm::setCurrent(int indexRoi, QString value)
{
	if (indexRoi == 1)
		m_current1 = value;
	else if (indexRoi == 2)
		m_current2 = value;
}

void Alarm::setTarget1(QString value)
{
	m_target1 = value;
}

void Alarm::setTarget2(QString value)
{
	m_target2 = value;
}

QString Alarm::getImagePath1() const
{
	return m_imagePath1;
}

QString Alarm::getImagePath2() const
{
	return m_imagePath2;
}

void Alarm::setImagePath1(QString value)
{
	m_imagePath1 = value;
}

void Alarm::setImagePath2(QString value)
{
	m_imagePath2 = value;
}

QString Alarm::getImagePath(int indexRoi) const
{
	if (indexRoi == 1)
		return m_imagePath1;
	else
		return m_imagePath2;
}

void Alarm::setImagePath(int indexRoi, QString value)
{
	if (indexRoi == 1)
		m_imagePath1 = value;
	else
		m_imagePath2 = value;
}

void Alarm::setTarget(int indexRoi, QString value)
{
	if (indexRoi == 1)
		m_target1 = value;
	else if (indexRoi == 2)
		m_target2 = value;
}

const QString Alarm::getValueAt(int i) const
{
	switch (i)
	{
	case 0:
		return m_date;
	case 1:
		return m_startTime;
	case 2:
		return m_duration;
	case 3:
		return m_status;
	case 4:
		return m_type1;
	case 5:
		return m_current1;
	case 6:
		return m_target1;
	case 7:
		return m_logic;
	case 8:
		return m_type2;
	case 9:
		return m_current2;
	default:
		return m_target2;
	}
}

QString Alarm::getBackgroundColor1() const 
{
	return m_backgroundColor1;
}

void Alarm::setBackgroundColor1(QString color)
{
	m_backgroundColor1 = color;
}

QString Alarm::getBackgroundColor2() const
{
	return m_backgroundColor2;
}

void Alarm::setBackgroundColor2(QString color)
{
	m_backgroundColor2 = color;
}

QString Alarm::getBackgroundColor(int indexROI) const
{
	switch (indexROI)
	{
	case 1:
		return m_backgroundColor1;
	default:
		return m_backgroundColor2;
	}
}

void Alarm::setBackgroundColor(int indexROI, QString color)
{
	switch (indexROI)
	{
	case 1:
		m_backgroundColor1 = color;
	default:
		m_backgroundColor2 = color;
	}
}
