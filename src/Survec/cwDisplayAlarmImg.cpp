#include "cwDisplayAlarmImg.h"
#include <cmath>
#include <Tools.h>
#include <QDebug>

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

cwDisplayAlarmImg::cwDisplayAlarmImg(QWidget * pParent, Qt::WindowFlags flags) :
QDialog(pParent, flags)
{
	qDebug() << "=== cwDisplayAlarmImg ===";

	m_ui.setupUi(this);

	m_imgROI1 = "";
	m_imgROI2 = "";
	m_titleROI1 = "";
	m_titleROI2 = "";
	m_imgSymbol1 = "";
	m_imgSymbol2 = "";
	m_titleSymbol1 = "";
	m_titleSymbol2 = "";

}	

cwDisplayAlarmImg::~cwDisplayAlarmImg()
{
	qDebug() << "cwDisplayAlarmImg::~cwDisplayAlarmImg()";
}

void cwDisplayAlarmImg::setContent()
{
	// Set all the titles
	m_ui.lbTitle1->setText(m_titleROI1);
	m_ui.lbTitle2->setText(m_titleROI2);
	m_ui.lbTitle3->setText(m_titleSymbol1);
	m_ui.lbTitle4->setText(m_titleSymbol2);

	// Set all the images (2 ROI and 2 symbol)

	if (m_imgROI2 == "")
	{
		m_ui.imgROI2->setVisible(false);
		m_ui.lbTitle2->setVisible(false);
	}

	cv::Mat imgROI1, imgROI2, symbolROI1, symbolROI2;
	if (m_imgROI1 != "")
	{
		imgROI1 = cv::imread(m_imgROI1.toStdString());
		cv::cvtColor(imgROI1, imgROI1, CV_BGR2RGB);
		m_ui.imgROI1->setPixmap(Tools::cvMatToQPixmap(imgROI1));
	}

	if (m_imgROI2 != "")
	{
		imgROI2 = cv::imread(m_imgROI2.toStdString());
		cv::cvtColor(imgROI2, imgROI2, CV_BGR2RGB);
		m_ui.imgROI2->setPixmap(Tools::cvMatToQPixmap(imgROI2));
	}

	if (m_titleSymbol1 != "" && m_imgSymbol1 != "")
	{
		symbolROI1 = cv::imread(m_imgSymbol1.toStdString());
		if (!symbolROI1.empty())
		{
			cv::cvtColor(symbolROI1, symbolROI1, CV_BGR2RGB);
			m_ui.imgSymbol1->setPixmap(Tools::cvMatToQPixmap(symbolROI1));

			m_ui.imgSymbol1->setVisible(true);
			m_ui.lbTitle3->setVisible(true);
		}
		else
		{
			m_ui.imgSymbol1->setVisible(false);
			m_ui.lbTitle3->setVisible(false);
		}
	}

	if (m_titleSymbol2 != "" && m_imgSymbol2 != "")
	{
		symbolROI2 = cv::imread(m_imgSymbol2.toStdString());
		if (!symbolROI2.empty())
		{
			cv::cvtColor(symbolROI2, symbolROI2, CV_BGR2RGB);
			m_ui.imgSymbol2->setPixmap(Tools::cvMatToQPixmap(symbolROI2));

			m_ui.imgSymbol2->setVisible(true);
			m_ui.lbTitle4->setVisible(true);
		}
		else
		{
			m_ui.imgSymbol2->setVisible(false);
			m_ui.lbTitle4->setVisible(false);
		}
	}
}

void cwDisplayAlarmImg::setImgROI1(QString path)
{
	m_imgROI1 = path;
}

void cwDisplayAlarmImg::setImgROI2(QString path)
{
	m_imgROI2 = path;
}

void cwDisplayAlarmImg::setTitleROI1(QString title)
{
	m_titleROI1 = title;
}

void cwDisplayAlarmImg::setTitleROI2(QString title)
{
	m_titleROI2 = title;
}

void cwDisplayAlarmImg::setImgSymbol1(QString path)
{
	m_imgSymbol1 = path;
}

void cwDisplayAlarmImg::setImgSymbol2(QString path)
{
	m_imgSymbol2 = path;
}

void cwDisplayAlarmImg::setTitleSymbol1(QString title)
{
	m_titleSymbol1 = title;
}

void cwDisplayAlarmImg::setTitleSymbol2(QString title)
{
	m_titleSymbol2 = title;
}
