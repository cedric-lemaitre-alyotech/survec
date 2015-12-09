#include "ROIConfigurationWidget.h"
#include "ROI.h"
#include "DetectionType.h"
#include "ColorDetection.h"
#include "Session.h"
#include <TextDetection.h>
#include "CustomRectItem.h"
#include <Tools.h>
#include <ColorToDetect.h>
//
#include <QDebug>
#include <QKeyEvent>
#include <QMessageBox>
//
ROIConfigurationWidget::ROIConfigurationWidget(QWidget * pParent, Qt::WindowFlags flags) :
QDialog(pParent, flags),
ptr_roi(NULL)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	qDebug() << "=== ROIConfiguration constructor ===";
	// Maximize button
	setWindowFlags(Qt::Window);

	m_ui.setupUi(this);
	// Init video player
	ptr_scene = m_ui.videoPlayer->getCwView()->getScene();
	m_ui.videoPlayer->setButtonVisible(true);


	// Init the color from ColorToDetect
	m_ui.cbColorDT->clear();
	
	// "OK" button
	connect(m_ui.pbOK,				SIGNAL(clicked()),					SLOT(updateModelFromGUI()));
	
	// Type of detection
	connect(m_ui.rbUndefinedDT,		SIGNAL(toggled(bool)),				SLOT(chooseDetectionType(bool)));
	connect(m_ui.rbColorDT,			SIGNAL(toggled(bool)),				SLOT(chooseDetectionType(bool)));
	connect(m_ui.rbTextDT,			SIGNAL(toggled(bool)),				SLOT(chooseDetectionType(bool)));
	connect(m_ui.rbSymbolDT,		SIGNAL(toggled(bool)),				SLOT(chooseDetectionType(bool)));
	connect(m_ui.rbNumberDT,		SIGNAL(toggled(bool)),				SLOT(chooseDetectionType(bool)));
		
	// Edit ROI values
	connect(m_ui.sbROIX,			SIGNAL(valueChanged(int)),			SLOT(setROIValues(int)));
	connect(m_ui.sbROIY,			SIGNAL(valueChanged(int)),			SLOT(setROIValues(int)));
	connect(m_ui.sbROIWidth,		SIGNAL(valueChanged(int)),			SLOT(setROIValues(int)));
	connect(m_ui.sbROIHeight,		SIGNAL(valueChanged(int)),			SLOT(setROIValues(int)));

	// Tolerance slider
	connect(m_ui.sTolerance,		SIGNAL(valueChanged(int)),			SLOT(updateSliderPercentage(int)));

	// Change color
	connect(m_ui.cbColorDT,			SIGNAL(activated(int)),				SLOT(colorModified(int)));

	// Change text value and font
	connect(m_ui.leTextToDetect,	SIGNAL(textChanged(QString)),		SLOT(textModified(QString)));
	connect(m_ui.cbTextFont, 		SIGNAL(currentIndexChanged(int)),	SLOT(textFontModified(int)));

	// Changed number value and font
	connect(m_ui.dspNumberDT,		SIGNAL(valueChanged(double)),		SLOT(numberModified(double)));
	connect(m_ui.cbNumberFont,		SIGNAL(currentIndexChanged(int)),	SLOT(numberFontModified(int)));

	// Update symbol
	connect(m_ui.pbSetSymbol,		SIGNAL(clicked()),					SLOT(updateSymbol()));

	// Maj ROI values when rectangle is moved/resized
	connect(ptr_scene,				SIGNAL(rectItemModified(int, int, int, int)),		this,	SLOT(setROIGUIValues(int, int, int, int)) );

	// Update ROI video feed
	connect(m_ui.videoPlayer,		SIGNAL(newImageWebcam()),			SLOT(updateROI()));
	connect(m_ui.cbVideoDevice,		SIGNAL(currentIndexChanged(int)),	SLOT(webcamChanged(int)));

	// Change of sensibility
	connect(m_ui.sbFiltration,		SIGNAL(valueChanged(int)),			SLOT(setFiltration(int)));

	showMaximized();

	QApplication::restoreOverrideCursor();
}

ROIConfigurationWidget::~ROIConfigurationWidget()
{
	m_ui.videoPlayer->pause(false);

	if (ptr_roi)
	{
		delete ptr_roi;
	}	
}

void ROIConfigurationWidget::setROI(ROI* pROI, int roiNmber)
{
	ptr_roi = new ROI();
	
	*ptr_roi = *pROI;
	m_ui.videoPlayer->getCwView()->setModeClick(1);
	m_ui.videoPlayer->setReadFullPixmap(true);

	if (ptr_roi)
	{
		initColorCombobox();

		m_ui.cbVideoDevice->clear();

		int idx = ptr_roi->getVideoDeviceId();
		qDebug() << "ROIConfig: videoDevice index: " << idx;

		int nbDevice = VideoDeviceManager::getInstance()->getNbDevice();

		for (int i = 0; i < nbDevice; i++)
		{
			m_ui.cbVideoDevice->addItem(QString::number(i + 1), i);
		}
		
		if (idx != -1)
		{
			m_ui.cbVideoDevice->setCurrentIndex(idx);
		}

		setWindowTitle(QString(tr("ROI Configuration") + QString(" - ROI%1")).arg(roiNmber));

		int bounds[4];
		ptr_roi->getBounds(bounds);
		ptr_scene->getRectangle()->setRect(bounds[0], bounds[1], bounds[2], bounds[3]);

		updateGUIFromModel();
		
	//	qDebug() << "m_ui.lbSymbol->pixmap()->isNull(): " << m_ui.lbSymbol->pixmap()->isNull();
	//	if (m_ui.lbSymbol->pixmap()->isNull())	// Called the first time, before the user clicks on "Set Symbol"
			//updateSymbol();

		if (dynamic_cast<SymbolDetection*>(ptr_roi->getDetectionType(ROI::SYMBOL_DT))->getSymbol().empty())		// If no symbol
		{
			QPixmap qp = ptr_scene->getItemPixmap()->pixmap().copy(bounds[0], bounds[1], bounds[2], bounds[3]);	// put whole picture
			qp = qp.scaled(m_ui.lbSymbol->width(), m_ui.lbSymbol->height(), Qt::AspectRatioMode::KeepAspectRatio);
			m_ui.lbSymbol->setPixmap(qp);
		}
		else
		{		
			// otherwise, put the symbol
			Mat temp = Mat(dynamic_cast<SymbolDetection*>(ptr_roi->getDetectionType(ROI::SYMBOL_DT))->getSymbol());
			//cvtColor(temp, temp, CV_BGR2RGB);
			QPixmap qp = Tools::cvMatToQPixmap(temp);
			qp = qp.scaled(m_ui.lbSymbol->width(), m_ui.lbSymbol->height(), Qt::AspectRatioMode::KeepAspectRatio);
			m_ui.lbSymbol->setPixmap(qp);
		}
	}
	m_ui.videoPlayer->delayedFitView();

}

void ROIConfigurationWidget::initColorCombobox()
{
	ColorToDetect cTD = VideoDeviceManager::getInstance()->getDevice(ptr_roi->getVideoDeviceId())->getColorToDetect();

	m_ui.cbColorDT->addItem(tr("Red"));
	m_ui.cbColorDT->addItem(tr("Orange"));
	m_ui.cbColorDT->addItem(tr("Yellow"));
	m_ui.cbColorDT->addItem(tr("Green"));
	m_ui.cbColorDT->addItem(tr("Cyan"));
	m_ui.cbColorDT->addItem(tr("Blue"));
	m_ui.cbColorDT->addItem(tr("Pink"));
	m_ui.cbColorDT->addItem(tr("White"));
	m_ui.cbColorDT->addItem(tr("Grey"));
	m_ui.cbColorDT->addItem(tr("Black"));

	QImage img(16, 16, QImage::Format_RGB32);
	QPainter p(&img);
	QRect rect = img.rect();

	QColor color = cTD.getRed();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(0, QPixmap::fromImage(img), Qt::DecorationRole);

	color = cTD.getOrange();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(1, QPixmap::fromImage(img), Qt::DecorationRole);

	color = cTD.getYellow();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(2, QPixmap::fromImage(img), Qt::DecorationRole);

	color = cTD.getGreen();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(3, QPixmap::fromImage(img), Qt::DecorationRole);

	color = cTD.getCyan();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(4, QPixmap::fromImage(img), Qt::DecorationRole);

	color = cTD.getBlue();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(5, QPixmap::fromImage(img), Qt::DecorationRole);

	color = cTD.getPink();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(6, QPixmap::fromImage(img), Qt::DecorationRole);

	color = cTD.getWhite();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(7, QPixmap::fromImage(img), Qt::DecorationRole);

	color = cTD.getGrey();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(8, QPixmap::fromImage(img), Qt::DecorationRole);

	color = cTD.getBlack();
	p.fillRect(rect, color);
	m_ui.cbColorDT->setItemData(9, QPixmap::fromImage(img), Qt::DecorationRole);

	m_ui.cbColorDT->setCurrentIndex(0);
}

void ROIConfigurationWidget::blockAllSignals(bool value)
{
	m_ui.cbVideoDevice->blockSignals(value);
	m_ui.sbROIX->blockSignals(value);
	m_ui.sbROIY->blockSignals(value);
	m_ui.sbROIWidth->blockSignals(value);
	m_ui.sbROIHeight->blockSignals(value);
	m_ui.rbUndefinedDT->blockSignals(value);
	m_ui.rbColorDT->blockSignals(value);
	m_ui.cbColorDT->blockSignals(value);
	m_ui.rbNumberDT->blockSignals(value);
	m_ui.cbOperator->blockSignals(value);
	m_ui.dspNumberDT->blockSignals(value);
	m_ui.cbNumberFont->blockSignals(value);
	m_ui.rbTextDT->blockSignals(value);
	m_ui.leTextToDetect->blockSignals(value);
	m_ui.cbTextFont->blockSignals(value);
	m_ui.rbSymbolDT->blockSignals(value);	
}

// slot
void ROIConfigurationWidget::chooseDetectionType(bool toggle)
{
	qDebug() << " !!! choose detection type ";
	// update roi1 or roi2
	if (ptr_roi)
	{
		// Undefined
		if (sender()->objectName().compare(m_ui.rbUndefinedDT->objectName()) == 0)
		{
			ptr_roi->setCurrentDetectionType(ROI::UNDEFINED_DT);
		}

		// Color
		else if (sender()->objectName().compare(m_ui.rbColorDT->objectName()) == 0)
		{
			m_ui.cbColorDT->setEnabled(toggle);

			// Tolerance
			m_ui.lbTolerance->setEnabled(toggle);
			m_ui.sTolerance->setEnabled(toggle);
			m_ui.lbTolerancePercentage->setEnabled(toggle);
			m_ui.cbColorIs->setEnabled(toggle);
			m_ui.sbFiltration->setEnabled(toggle);
			m_ui.lbFiltration->setEnabled(toggle);

			ptr_roi->setCurrentDetectionType(ROI::COLOR_DT);
		}

		// Number
		else if (sender()->objectName().compare(m_ui.rbNumberDT->objectName()) == 0)
		{
			m_ui.dspNumberDT->setEnabled(toggle);
			m_ui.cbOperator->setEnabled(toggle);
			m_ui.cbNumberFont->setEnabled(toggle);
			m_ui.lbNumberFont->setEnabled(toggle);
			m_ui.sbFiltration->setEnabled(toggle);
			m_ui.lbFiltration->setEnabled(toggle);
			ptr_roi->setCurrentDetectionType(ROI::NUMBER_DT);
		}

		// Symbol
		else if (sender()->objectName().compare(m_ui.rbSymbolDT->objectName()) == 0)
		{
			m_ui.lbSymbol->setEnabled(toggle);
			m_ui.pbSetSymbol->setEnabled(toggle);
			m_ui.cbSymbolIs->setEnabled(toggle);
			m_ui.sbFiltration->setEnabled(toggle);
			m_ui.lbFiltration->setEnabled(toggle);

			// Tolerance
			m_ui.lbTolerance->setEnabled(toggle);
			m_ui.sTolerance->setEnabled(toggle);
			m_ui.lbTolerancePercentage->setEnabled(toggle);

			ptr_roi->setCurrentDetectionType(ROI::SYMBOL_DT);
		}

		// Text
		else if (sender()->objectName().compare(m_ui.rbTextDT->objectName()) == 0)
		{
			m_ui.leTextToDetect->setEnabled(toggle);
			m_ui.cbTextFont->setEnabled(toggle);
			m_ui.lbTextFont->setEnabled(toggle);
			m_ui.cbTextIs->setEnabled(toggle);
			m_ui.sbFiltration->setEnabled(toggle);
			m_ui.lbFiltration->setEnabled(toggle);
			ptr_roi->setCurrentDetectionType(ROI::TEXT_DT);
		}
	}
}

void ROIConfigurationWidget::updateGUIFromModel()
{
	if (ptr_roi)
	{
		UndefinedDetection* ptrUndefinedDetection = dynamic_cast<UndefinedDetection*>(ptr_roi->getDetectionType(ROI::UNDEFINED_DT));
		if (ptrUndefinedDetection && ptr_roi->getCurrentDetectionType() == 0)
		{
			m_ui.rbUndefinedDT->setChecked(true);
		}

		// enable color
		ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(ptr_roi->getDetectionType(ROI::COLOR_DT));
		if (ptrColorDetection && ptr_roi->getCurrentDetectionType() == 1)
		{
			m_ui.rbColorDT->setChecked(true);
		}

		// enable Symbol
		SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(ptr_roi->getDetectionType(ROI::SYMBOL_DT));
		if (ptrSymbolDetection && ptr_roi->getCurrentDetectionType() == 2)
		{
			m_ui.rbSymbolDT->setChecked(true);
		}

		// enable Text
		TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(ptr_roi->getDetectionType(ROI::TEXT_DT));
		if (ptrTextDetection && ptr_roi->getCurrentDetectionType() == 3)
		{
			m_ui.rbTextDT->setChecked(true);
		}

		// enable number
		NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(ptr_roi->getDetectionType(ROI::NUMBER_DT));
		if (ptrNumberDetection && ptr_roi->getCurrentDetectionType() == 4)
		{
			m_ui.rbNumberDT->setChecked(true);
		}

		blockAllSignals(true);

		// update tolerance slider
		m_ui.sTolerance->setValue(ptr_roi->getTolerance());

		// update sensitivity spinbox
		m_ui.sbFiltration->setValue(ptr_roi->getFiltration());

		// values color
		int indexColor = ptrColorDetection->getIndexColor();
		m_ui.cbColorDT->setCurrentIndex(indexColor);
		int indexIsColor = ptrColorDetection->getAlarmOnEquals();
		m_ui.cbColorIs->setCurrentIndex(indexIsColor==0);

		// values text
		QString text = ptrTextDetection->getTextToDetect();
		if (!text.isNull())
			m_ui.leTextToDetect->setText(text);
		int indexIsText = ptrTextDetection->getAlarmOnEquals();
		m_ui.cbTextIs->setCurrentIndex(indexIsText==0);
		m_ui.cbTextFont->setCurrentIndex(ptrTextDetection->getFontIndex());

		// values number & operator
		double number = ptrNumberDetection->getNumber();
		m_ui.dspNumberDT->setValue(number);
		NumberDetection::operatorType op = (NumberDetection::operatorType) ptrNumberDetection->getOperator();
		m_ui.cbOperator->setCurrentIndex((int)ptrNumberDetection->getOperator());
		m_ui.cbNumberFont->setCurrentIndex(ptrNumberDetection->getFontIndex());
		
		// update rectangles	
		int bounds[4];
		ptr_roi->getBounds(bounds);

		m_ui.sbROIX->setValue(bounds[0]);
		m_ui.sbROIY->setValue(bounds[1]);
		m_ui.sbROIWidth->setValue(bounds[2]);
		m_ui.sbROIHeight->setValue(bounds[3]);

		// values symbol
			// ROI
		updateROI(bounds[0], bounds[1], bounds[2], bounds[3]);
		ptr_scene->getRectangle()->setRect(bounds[0], bounds[1], bounds[2], bounds[3]);		
			// Symbol
		Mat mat;
		mat = ptrSymbolDetection->getSymbol();
		if (!mat.empty())
			cvtColor(mat, mat, CV_8UC3);

		QPixmap qpixmap = Tools::cvMatToQPixmap(mat);
		
		if (!qpixmap.isNull())
		{
			qpixmap = qpixmap.scaled(m_ui.lbSymbol->width(), m_ui.lbSymbol->height(), Qt::AspectRatioMode::KeepAspectRatio);
			m_ui.lbSymbol->setPixmap(qpixmap);
		}
		int indexIsSymbol = ptrSymbolDetection->getAlarmOnEquals();
		m_ui.cbSymbolIs->setCurrentIndex(indexIsSymbol==0);

		blockAllSignals(false);
	}
}

// slot
void ROIConfigurationWidget::updateModelFromGUI()
{
	qDebug() << ">INFO: Update values";
	
	updateModel(ptr_roi);
}

void ROIConfigurationWidget::updateModel(ROI * p_roi)
{
	if (ptr_roi)
	{
		// Webcam
		int indexWebcam = m_ui.cbVideoDevice->currentIndex();
		qDebug() << "Index webCam updatemodel: " << indexWebcam;
		ptr_roi->setVideoDeviceId(indexWebcam);

		// Bounds
		int x = m_ui.sbROIX->value();
		int y = m_ui.sbROIY->value();
		int w = m_ui.sbROIWidth->value();
		int h = m_ui.sbROIHeight->value();
		ptr_roi->setBounds(x, y, w, h);

		// Throw alarm on...
		// Color
		int indexColorIs = m_ui.cbColorIs->currentIndex();
		dynamic_cast<ColorDetection*>(p_roi->getDetectionType(ROI::COLOR_DT))->setAlarmOnEquals(indexColorIs==0);
		// Text
		int indexTextIs = m_ui.cbTextIs->currentIndex();
		dynamic_cast<TextDetection*>(p_roi->getDetectionType(ROI::TEXT_DT))->setAlarmOnEquals(indexTextIs==0);
		// Symbol
		int indexSymbolIs = m_ui.cbSymbolIs->currentIndex();
		dynamic_cast<SymbolDetection*>(p_roi->getDetectionType(ROI::SYMBOL_DT))->setAlarmOnEquals(indexSymbolIs==0);
		// Number
		NumberDetection::operatorType op = (NumberDetection::operatorType)  m_ui.cbOperator->currentIndex();
		dynamic_cast<NumberDetection*>(p_roi->getDetectionType(ROI::NUMBER_DT))->setOperator(op);
		dynamic_cast<NumberDetection*>(p_roi->getDetectionType(ROI::NUMBER_DT))->setOperatorString(m_ui.cbOperator->currentText());
		
		// Update model tolerance
		int tol = m_ui.sTolerance->value();
		ptr_roi->setTolerance(tol);

		// Update model color (if the user pick red, the slot isn't called)
		colorModified(m_ui.cbColorDT->currentIndex());
	}
}


void ROIConfigurationWidget::updateROI(int x, int y, int w, int h)
{
	//qDebug() << " ROIConfigurationWidget update symbol";
	QPixmap qpixmap(ptr_scene->getItemPixmap()->pixmap().copy(x, y, w, h));

	qpixmap = qpixmap.scaled(m_ui.lbROI->width(), m_ui.lbROI->height(), Qt::AspectRatioMode::KeepAspectRatio);

	// Sets the qpixmap as the symbol (GUI)
	m_ui.lbROI->setPixmap(qpixmap);
}

void ROIConfigurationWidget::updateROI()
{
	int bounds[4];
	ptr_roi->getBounds(bounds);

	// ROI
	updateROI(bounds[0], bounds[1], bounds[2], bounds[3]);
}

// slot
void ROIConfigurationWidget::updateSymbol()
{
	qDebug() << "ROIConfig: updateSymbol";

	int bounds[4]; 
	ptr_roi->getBounds(bounds);

	QPixmap qp = ptr_scene->getItemPixmap()->pixmap().copy(bounds[0], bounds[1], bounds[2], bounds[3]);

	// Conversion from qpixmap to mat
	if (ptr_roi->getCurrentDetectionType() == 2)
	{
		Mat mat = Tools::QPixmapToCvMat(qp);
		cvtColor(mat, mat, CV_BGRA2RGB);
		
		// Sets the mat as the symbol (model)
		dynamic_cast<SymbolDetection*>(ptr_roi->getDetectionType(ROI::SYMBOL_DT))->setSymbol(mat);

		qp = qp.scaled(m_ui.lbSymbol->width(), m_ui.lbSymbol->height(), Qt::AspectRatioMode::KeepAspectRatio);
		m_ui.lbSymbol->setPixmap(qp);
	}	
}

// slot
void ROIConfigurationWidget::setROIValues(int value)
{
	if (ptr_roi)
	{
		// Restrain the rectangle to its maximum size when manually editing
		m_ui.sbROIX->setMaximum(ptr_scene->getWidth() - m_ui.sbROIWidth->value());
		m_ui.sbROIY->setMaximum(ptr_scene->getHeight() - m_ui.sbROIHeight->value());
		m_ui.sbROIWidth->setMaximum(ptr_scene->getWidth() - m_ui.sbROIX->value());
		m_ui.sbROIHeight->setMaximum(ptr_scene->getHeight() - m_ui.sbROIY->value());

		blockAllSignals(true);

		int x = m_ui.sbROIX->value();
		int y = m_ui.sbROIY->value();
		int w = m_ui.sbROIWidth->value();
		int h = m_ui.sbROIHeight->value();

		ptr_roi->setBounds(x, y, w, h);

		updateGUIFromModel();

		blockAllSignals(false);
	}
}

// slot
void ROIConfigurationWidget::setROIGUIValues(int x, int y, int w, int h)
{
	if (ptr_roi)
	{
		ptr_roi->setBounds(x, y, w, h);
		updateGUIFromModel();
	}
}

void ROIConfigurationWidget::colorModified(int index)
{
	ColorToDetect cTD = VideoDeviceManager::getInstance()->getDevice(ptr_roi->getVideoDeviceId())->getColorToDetect();

	ColorDetection* ptrColorDT = dynamic_cast<ColorDetection*>(ptr_roi->getDetectionType(ROI::COLOR_DT));
	if (ptrColorDT)
	{
		ptrColorDT->setIndexColor(index);
		QColor color;

		switch (index)
		{
		case 0:
			color = cTD.getRed();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		case 1:
			color = cTD.getOrange();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		case 2:
			color = cTD.getYellow();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		case 3:
			color = cTD.getGreen();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		case 4:
			color = cTD.getCyan();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		case 5:
			color = cTD.getBlue();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		case 6:
			color = cTD.getPink();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		case 7:
			color = cTD.getWhite();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		case 8:
			color = cTD.getGrey();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		case 9:
			color = cTD.getBlack();
			cTD.setCurrent(color);
			ptrColorDT->setColor(color);
			break;
		default:
			break;
		}
	}
}

void ROIConfigurationWidget::textModified(QString text)
{
	TextDetection* ptrTextDT = dynamic_cast<TextDetection*>(ptr_roi->getDetectionType(ROI::TEXT_DT));
	if (ptrTextDT)
	{
		if (!text.isNull())
			ptrTextDT->setTextToDetect(text);
	}
}

void ROIConfigurationWidget::textFontModified(int index)
{
	TextDetection* ptrTextDT = dynamic_cast<TextDetection*>(ptr_roi->getDetectionType(ROI::TEXT_DT));
	if (ptrTextDT)
	{
		ptrTextDT->setFontIndex(index);
	}
}

void ROIConfigurationWidget::numberModified(double value)
{
	NumberDetection* ptrNumberDT = dynamic_cast<NumberDetection*>(ptr_roi->getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDT)
	{
		ptrNumberDT->setNumber(value);
	}
}

void ROIConfigurationWidget::numberFontModified(int index)
{
	NumberDetection* ptrNumberDT = dynamic_cast<NumberDetection*>(ptr_roi->getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDT)
	{
		ptrNumberDT->setFontIndex(index);
	}
}

void ROIConfigurationWidget::setFiltration(int value)
{
	qDebug() << "sensitivity : " << value;
	ptr_roi->setFiltration(value);
}

void ROIConfigurationWidget::displayBounds()
{
	if (ptr_roi)
	{
		int bounds[4];
		ptr_roi->getBounds(bounds);
		qDebug() << " bounds: " << bounds[0] << "/" << bounds[1] << "/" << bounds[2] << "/" << bounds[3];
	}
}

ROI* ROIConfigurationWidget::getROI() const
{
	return ptr_roi;
}

// slot
void ROIConfigurationWidget::updateSliderPercentage(int value)
{
	// Display the percentage
	m_ui.lbTolerancePercentage->setText(QString("%1%").arg(value));
}

// slot
void ROIConfigurationWidget::webcamChanged(int index)
{
	int tmp = ptr_roi->getVideoDeviceId();
	if (index != tmp)
	{
		qDebug() << "== ROI Config webcamChanged: " << index;

		// Change the webcam stream
		ptr_roi->setVideoDeviceId(index);

		int feedId = VideoDeviceManager::getInstance()->getDevice(index)->getId();
		qDebug() << "feedId: " << feedId;
		m_ui.videoPlayer->setDeviceId(index);
		m_ui.videoPlayer->setFeedId(feedId);
		m_ui.videoPlayer->readWebcamFeed();

		initColorCombobox();

		// Set rectangle bounds to new webcam size
		int bounds[4];
		ptr_roi->getBounds(bounds);
		ptr_scene->getRectangle()->setRect(bounds[0], bounds[1], bounds[2], bounds[3]);


	}
}

void ROIConfigurationWidget::closeEvent(QCloseEvent *event)
{
	QMessageBox msgBox(this);

	msgBox.setWindowTitle("ROI Configuration");
	msgBox.setText(tr("Are you sure you want to close?"));
	msgBox.setIcon(QMessageBox::Question);
	msgBox.setStandardButtons(QMessageBox::Close | QMessageBox::Cancel);
	msgBox.setDefaultButton(QMessageBox::Close);
	int result = msgBox.exec();

	switch (result) 
	{
	case QMessageBox::Close:
		event->accept();
		break;
	case QMessageBox::Cancel:
		event->ignore();
		break;
	default:
		QDialog::closeEvent(event);
		break;
	}
}