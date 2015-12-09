#ifndef CWDISPLAYALARMIMG_H
#define CWDISPLAYALARMIMG_H

#include "ui_cwDisplayAlarmImg.h"
#include <QObject>
#include <QDialog>


class cwDisplayAlarmImg: public QDialog
{
	Q_OBJECT

public:
	
	/**
	*	Constructor
	*/
	cwDisplayAlarmImg(QWidget * pParent = 0, Qt::WindowFlags flags = 0);

	/**
	*	Destructor
	*/
	~cwDisplayAlarmImg();

	void setContent();

	/**
	*	@brief: set the path of the image of ROI 1
	*/
	void setImgROI1(QString path);

	/**
	*	@brief: set the path of the image of ROI 2
	*/
	void setImgROI2(QString path);

	/**
	*	@brief: set the title of ROI 1
	*/
	void setTitleROI1(QString title);

	/**
	*	@brief: set the title of ROI 2
	*/
	void setTitleROI2(QString title);

	/**
	*	@brief: set the image of the symbol of ROI 1
	*/
	void setImgSymbol1(QString path);

	/**
	*	@brief: set the image of the symbol of ROI 2
	*/
	void setImgSymbol2(QString path);

	/**
	*	@brief: set the title of the symbol of ROI 1
	*/
	void setTitleSymbol1(QString title);

	/**
	*	@brief: set the title of the symbol of ROI 2
	*/
	void setTitleSymbol2(QString title);

protected:
	Ui::cwDisplayAlarmImg	m_ui;				//!<ui

	QString m_imgROI1;							//!< path of image ROI 1					
	QString m_imgROI2;							//!< path of image ROI 2
	QString m_titleROI1;						//!< title of ROI 1
	QString m_titleROI2;						//!< title of ROI 2
	QString m_imgSymbol1;						//!< path of symbol of image ROI 1
	QString m_imgSymbol2;						//!< path of symbol of image ROI 2
	QString m_titleSymbol1;						//!< title of symbol 1
	QString m_titleSymbol2;						//!< title of symbol 2
};

#endif
