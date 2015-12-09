#include "cwROIResult.h"
#include "ROIConfigurationWidget.h"
#include <QDebug>
#include <Tools.h>
#include <ColorToDetect.h>
#include <TextDetection.h>
#include <QFileDialog>

cwROIResult::cwROIResult(QWidget * pParent, Qt::WindowFlags flags): QWidget(pParent, flags)
{
	qDebug() << "=== cwROIResult constructor ===";

	// Init
	m_ui.setupUi(this);
	m_isTest = false;

	connect(m_ui.gb_ROI,		SIGNAL(clicked(bool)),	this,		SLOT(activeROIModified(bool)));
	connect(m_ui.pb_editROI,	SIGNAL(clicked()),		this,		SLOT(editROIConfig()));
	connect(m_ui.pbTest,		SIGNAL(clicked()),		this,		SLOT(testConfig()));
	connect(m_ui.pbScreenshot,	SIGNAL(clicked()),		this,		SLOT(screenshot()));
	connect(m_ui.cbDebug,		SIGNAL(clicked(bool)),	this,		SLOT(setDebug(bool)));

	setLabelVisibility(false);
	setActiveUndefinedDetection();

#ifndef _DEBUG
	m_ui.cbDebug->setVisible(false);
#endif

	//m_ui.labelCurrent->setEnabled(false);
	//m_ui.valueCurrent->setEnabled(false);
	//m_ui.valueCurrent->setMinimumSize(30, 30);
	//m_ui.valueTarget->setMinimumSize(30, 30);
}

cwROIResult::~cwROIResult()
{
}

void cwROIResult::setDebug(bool val)
{
	ptr_roi->setDebug(val);
}

void cwROIResult::setROI(ROI * p_roi, int roiNumber)
{
	ptr_roi = p_roi;
	m_roiNumber = roiNumber;
	updateGUI();
}

void cwROIResult::screenshot()
{
	QString fileName, workingDir;

	if (ptr_roi->getWorkingDir() != "")
		workingDir = ptr_roi->getWorkingDir();

	fileName = QFileDialog::getSaveFileName(this, tr("Save as..."), workingDir, tr("PNG (*.png)"));
	workingDir = QFileInfo(fileName).absolutePath();
	fileName = fileName.section('/', -1);

	qDebug() << "screenshot : " << workingDir + "/" + fileName;
	Mat m_image;

	int bounds[4];
	ptr_roi->getBounds(bounds);

	VideoDeviceManager::getInstance()->getDevice(ptr_roi->getVideoDeviceId())->getImage(m_image);
	Mat res(m_image, cv::Rect(bounds[0], bounds[1], bounds[2], bounds[3]));
	
	cvtColor(res, res, CV_BGR2RGB);
	if (!fileName.isEmpty())
	{
		imwrite(workingDir.toStdString() + "/" + fileName.toStdString(), res);
	}
}

void cwROIResult::testConfig()
{
	QApplication::setOverrideCursor(Qt::WaitCursor);

	m_isTest = true;
	ROI::detectionType type = ptr_roi->getCurrentDetectionType();
	Mat m_image;

	int bounds[4];
	ptr_roi->getBounds(bounds);
	
	VideoDeviceManager::getInstance()->getDevice(ptr_roi->getVideoDeviceId())->getImage(m_image);
	Mat res(m_image, cv::Rect(bounds[0], bounds[1], bounds[2], bounds[3])); 
	
	qDebug() << "res cwroiresult type/chan/size: " << res.type() << "/" << res.channels() << " / " << res.cols << "x" << res.rows;

	//cvtColor(res, res, CV_BGR2RGB);
	ptr_roi->retrieveImage();
	ptr_roi->getDetectionType(type)->process(res);
	
	emit alarmChanged();		// emit a signal to display the alarm even if the result is the same (DetectionType sends this signal only if a process changes the alarm)
	emit changed();
	m_isTest = false;

	QApplication::restoreOverrideCursor();
}

void cwROIResult::updateGUI()
{
	//qDebug() << "cwROIResult updateGUI";
	if (ptr_roi)
	{
		blockAllSignals(true);

		// clear All
		//m_ui.valueTarget->clear();
		//m_ui.valueCurrent->clear();
		//m_ui.valueDetectionType->clear();
		//m_ui.valueOperator->clear();		

	//	m_ui.pb_editROI->setEnabled(ptr_roi->getActive());
		m_ui.gb_ROI->setChecked(ptr_roi->getActive());

		m_ui.webcamNumber->setText(QString("%1").arg(ptr_roi->getVideoDeviceId() + 1));

		ROI::detectionType type = ptr_roi->getCurrentDetectionType();
		switch (type)
		{
		case ROI::COLOR_DT:
			setActiveColorDetection();
			break;
		case ROI::NUMBER_DT:
			setActiveNumberDetection();
			break;
		case ROI::TEXT_DT:
			setActiveTextDetection();
			break;
		case ROI::SYMBOL_DT:
			setActiveSymbolDetection();
			break;
		default:
			setActiveUndefinedDetection();
			break;
		}

		blockAllSignals(false);

		emit roiStateChanged();
	}
}

void cwROIResult::updateAlarm()
{
	if (m_isTest && ptr_roi->getActive())
	{
		if (m_isTest && ptr_roi->getActive() && ptr_roi->getAlarm())
			m_ui.lbAlarm->setPixmap(QPixmap(":/resources/resources/icons/ledALARM.png"));
		else if (m_isTest && ptr_roi->getActive() && !ptr_roi->getAlarm())
			m_ui.lbAlarm->setPixmap(QPixmap(":/resources/resources/icons/ledOK.png"));
		else if (!m_isTest)
			m_ui.lbAlarm->setPixmap(QPixmap(":/resources/resources/icons/ledNO.png"));

	}
	else if (!m_isTest || ptr_roi->getActive())
	{
		if (ptr_roi->getProcessStarted() && ptr_roi->getActive() && ptr_roi->getAlarm())
			m_ui.lbAlarm->setPixmap(QPixmap(":/resources/resources/icons/ledALARM.png"));
		else if (ptr_roi->getProcessStarted() && ptr_roi->getActive() && !ptr_roi->getAlarm())
			m_ui.lbAlarm->setPixmap(QPixmap(":/resources/resources/icons/ledOK.png"));
		else if (!ptr_roi->getProcessStarted())
			m_ui.lbAlarm->setPixmap(QPixmap(":/resources/resources/icons/ledNO.png"));
	}
}

void cwROIResult::setActiveColorDetection()
{
	//qDebug() << "setActiveColorDetection";
	// Shows all labels
	setLabelVisibility(1);

	m_ui.valueCurrent->setVisible(true);
	m_ui.labelCurrent->setVisible(true);
	m_ui.valueOperator->setVisible(false);
	m_ui.valueCurrent->setMaximumSize(30, 30);
	m_ui.valueTarget->setMaximumSize(30, 30);

	m_ui.valueDetectionType->setText(tr("Color"));

	if (ptr_roi)
	{
		ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(ptr_roi->getDetectionType(ROI::COLOR_DT));

		if (ptrColorDetection)
		{
			m_ui.valueCurrent->setText(" ");
			m_ui.valueTarget->setText(" ");

			// Generate alarm on... (valueAlarm)
			QString tmp;
			if (ptrColorDetection->getAlarmOnEquals())
				tmp = tr("Target is");
			else
				tmp = tr("Target is not");
			m_ui.labelTarget->setText(tmp);

			QColor color = VideoDeviceManager::getInstance()->getDevice(ptr_roi->getVideoDeviceId())->getColorToDetect().getIndexColor(ptrColorDetection->getIndexColor());
			
			// Shows the color in the valueTarget label
			QString param2 = QString("background-color: rgb(%1, %2, %3)").arg(color.red()).arg(color.green()).arg(color.blue());
			m_ui.valueTarget->setStyleSheet(param2);

			// Shows the color in the valueCurrent label
			QColor currentColor = ptrColorDetection->getCurrentColor();
			QString param = QString("background-color: rgb(%1, %2, %3)").arg(currentColor.red()).arg(currentColor.green()).arg(currentColor.blue());
			m_ui.valueCurrent->setStyleSheet(param);
			
			if (!ptr_roi->getProcessStarted() && !m_isTest)
			{
				QString param = QString("background-color: rgb(128, 128, 128)"); 
				m_ui.valueCurrent->setStyleSheet(param);
			}

			// Doesn't display the operator
			m_ui.valueOperator->setEnabled(false);

			setFrameRaised(false);
		}
	}
}

void cwROIResult::setActiveTextDetection()
{
	//qDebug() << "setActiveTextDetection";
	// Shows all labels
	setLabelVisibility(1);
	m_ui.valueCurrent->setMaximumWidth(150);
	m_ui.valueTarget->setMaximumWidth(150);
	m_ui.valueCurrent->setMaximumHeight(25);
	m_ui.labelCurrent->setMaximumHeight(25);
	m_ui.valueTarget->setMaximumHeight(25);
	m_ui.labelTarget->setMaximumHeight(25);

	m_ui.valueCurrent->setVisible(true);
	m_ui.labelCurrent->setVisible(true);	
	m_ui.valueOperator->setVisible(false);
		
	m_ui.valueDetectionType->setText(tr("Text"));
	
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(ptr_roi->getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection)
	{
		QString tmp;
		if (ptrTextDetection->getAlarmOnEquals())
			tmp = tr("Target is");
		else
			tmp = tr("Target is not");
		m_ui.labelTarget->setText(tmp);

		// Set the background color to white
		QString param = QString("background-color: rgb(255, 255, 255)");

		// Apply the color and the text to detect to valueCurrent...
		m_ui.valueCurrent->setStyleSheet(param);
		m_ui.valueCurrent->setText(ptrTextDetection->getCurrent());

		// ... and to valueTarget
		m_ui.valueTarget->setStyleSheet(param);
		m_ui.valueTarget->setText(ptrTextDetection->getTextToDetect());

		// Doesn't display the operator
		m_ui.valueOperator->setEnabled(false);

		setFrameRaised(true);
	}	
}

void cwROIResult::setActiveNumberDetection()
{
	//qDebug() << "setActiveNumberDetection";
	// Shows all labels
	setLabelVisibility(1);
	m_ui.valueCurrent->setMaximumWidth(150);
	m_ui.valueTarget->setMaximumWidth(150);
	m_ui.valueCurrent->setMaximumHeight(25);
	m_ui.labelCurrent->setMaximumHeight(25);
	m_ui.valueTarget->setMaximumHeight(25);
	m_ui.labelTarget->setMaximumHeight(25);

	m_ui.valueCurrent->setVisible(true);
	m_ui.labelCurrent->setVisible(true);
	m_ui.valueOperator->setVisible(true);

	m_ui.valueDetectionType->setText(tr("Number"));
	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(ptr_roi->getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection)
	{
		m_ui.labelTarget->setText(tr("Target is"));

		QString param = QString("background-color: rgb(255, 255, 255)");
		m_ui.valueCurrent->setStyleSheet(param);
		m_ui.valueCurrent->setText(QString::number(ptrNumberDetection->getCurrent(), 'f'));

		// Apply the color and the number to detect to valueCurrent
		m_ui.valueTarget->setStyleSheet(param);
		m_ui.valueTarget->setText(QString::number(ptrNumberDetection->getNumber(), 'f'));
		m_ui.valueOperator->setText(ptrNumberDetection->getOperatorString());

		// Display the operator
		m_ui.valueOperator->setEnabled(true);

		setFrameRaised(true);
	}
}

void cwROIResult::setActiveSymbolDetection()
{
	// Shows all labels
	//qDebug() << "setActiveSymbolDetection";
	setLabelVisibility(1);
	m_ui.valueOperator->hide(); // setVisible(false);
	m_ui.valueCurrent->hide(); // setVisible(false);
	m_ui.labelCurrent->hide(); // setVisible(false);

	m_ui.valueTarget->setMaximumSize(80, 80);

	m_ui.valueDetectionType->setText(tr("Symbol"));

	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(ptr_roi->getDetectionType(ROI::SYMBOL_DT));
	
	if (ptrSymbolDetection)
	{
		QString tmp;
		if (ptrSymbolDetection->getAlarmOnEquals())
			tmp = tr("Target is");
		else
			tmp = tr("Target is not");
		m_ui.labelTarget->setText(tmp);

		// Reads the symbol (Mat) and convert it to qpixmap
		//Mat image = Mat(ptrSymbolDetection->getSymbol());
		//ptrSymbolDetection->getSymbol().copyTo(image);

		//cv::cvtColor(image, image, CV_BGR2RGB);
		Mat img = ptrSymbolDetection->getSymbol();
		if (img.empty())
		{
			img = cvCreateImage(cvSize(10, 10), 8, 3);
		}
		QPixmap qpixmap = Tools::cvMatToQPixmap(img);

		// Scales the qpixmap keeping the aspect ratio
		qpixmap = qpixmap.scaled(m_ui.valueTarget->maximumWidth(), m_ui.valueTarget->maximumHeight(), Qt::AspectRatioMode::KeepAspectRatio);

		// Display the qpixmap in the label
		m_ui.valueTarget->setPixmap(qpixmap);

		// Doesn't display the operator
		//m_ui.valueOperator->setEnabled(false);

		setFrameRaised(false);
	}
}

void cwROIResult::setActiveUndefinedDetection()
{
	// Shows no label
	setLabelVisibility(0);
	m_ui.valueDetectionType->setText(tr("Undefined detection"));
}

void cwROIResult::setLabelVisibility(bool visible)
{
	m_ui.labelTarget->setVisible(visible);
	m_ui.valueTarget->setVisible(visible);

	m_ui.valueOperator->hide();
	m_ui.valueCurrent->hide();
	m_ui.labelCurrent->hide();
}

void cwROIResult::setFrameRaised(bool raise)
{
	if (raise)
	{
		m_ui.valueCurrent->setFrameShape(QFrame::Box);
		m_ui.valueCurrent->setFrameShadow(QFrame::Raised);
		m_ui.valueTarget->setFrameShape(QFrame::Box);
		m_ui.valueTarget->setFrameShadow(QFrame::Raised);
	}
	else
	{
		m_ui.valueCurrent->setFrameShape(QFrame::NoFrame);
		m_ui.valueCurrent->setFrameShadow(QFrame::Plain);
		m_ui.valueTarget->setFrameShape(QFrame::NoFrame);
		m_ui.valueTarget->setFrameShadow(QFrame::Plain);
	}
}

void cwROIResult::activeROIModified(bool active)
{
	if (ptr_roi)
	{
		ptr_roi->setActive(active);
	}
}

void cwROIResult::editROIConfig()
{
	qDebug() << "ROI config !";
	if (ptr_roi)
	{
		ROIConfigurationWidget roiConfigurationWidget(this);
		emit stopPlaying(true);
		roiConfigurationWidget.setROI(ptr_roi, m_roiNumber);
		m_isTest = false;
		int res = roiConfigurationWidget.exec();
		if (res == 1)
		{
			*ptr_roi = *roiConfigurationWidget.getROI();

			updateGUI();
			updateAlarm();
			//emit symbolChanged();
		}
		emit stopPlaying(false);
		emit webcamChanged();
	}
}

void cwROIResult::blockAllSignals(bool value)
{
	m_ui.gb_ROI->blockSignals(value);
	m_ui.pb_editROI->blockSignals(value);
	m_ui.valueTarget->blockSignals(value);
	m_ui.valueCurrent->blockSignals(value);
	m_ui.valueOperator->blockSignals(value);
}

QGroupBox * cwROIResult::getGroupBox()
{
	return m_ui.gb_ROI;
}

QPushButton * cwROIResult::getROIConfigPushButton()
{
	return m_ui.pb_editROI;
}

QLabel * cwROIResult::getAlarmLabel()
{
	return m_ui.lbAlarm;
}

QLabel * cwROIResult::getValueCurrent()
{
	return m_ui.valueCurrent;
}

QLabel * cwROIResult::getLabelCurrent()
{
	return m_ui.labelCurrent;
}

QLabel * cwROIResult::getLabelOperator()
{
	return m_ui.valueOperator;
}

QPushButton * cwROIResult::getPbTest()
{
	return m_ui.pbTest;
}

void cwROIResult::setIsTest(bool val)
{
	m_isTest = val;
}

QPushButton * cwROIResult::getPbScreenshot()
{
	return m_ui.pbScreenshot;
}