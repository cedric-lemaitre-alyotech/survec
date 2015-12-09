#include "AlarmPluginInterface.h"
#include <QDebug>

using namespace cv;

AlarmPluginInterface::AlarmPluginInterface()
{	
	qDebug() << ">CONSTRUCTOR: AlarmPluginInterface";
}

AlarmPluginInterface::~AlarmPluginInterface()
{
}

//void AlarmPluginInterface::setImage(Mat & image)
//{
//	qDebug() << "AlarmPluginInterface: setImage";
//	m_image = image;
//}
