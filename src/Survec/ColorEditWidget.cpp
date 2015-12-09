#include "ColorEditWidget.h"
#include <VideoDeviceManager.h>
#include <Tools.h>
#include "Scene.h"
#include <QEvent>
#include <QDebug>
#include <QDialog>
#include <QPixmap>
#include <QGraphicsSceneMouseEvent>
#include <ColorToDetect.h>

ColorEditWidget::ColorEditWidget(QWidget * pParent, Qt::WindowFlags flags) :
QDialog(pParent, flags),
ptr_session(NULL)
{
	QApplication::setOverrideCursor(Qt::WaitCursor);
	qDebug() << "=== Color Edit Widget ===";
	m_ui.setupUi(this);

	// Init video player
	m_ui.videoPlayer->getCwView()->setModeClick(0);
	m_ui.videoPlayer->setReadFullPixmap(true);
	m_ui.videoPlayer->setButtonVisible(true);

	// Maximize button
	setWindowFlags(Qt::Window);
	
	// Init
	isAllEnabled = true;
	updateColor = false;
	avg = false;
	m_deviceId = 0;

	// Allows to select a real color
	connect(m_ui.tbResetRed,				SIGNAL(clicked()),									SLOT(resetRealButton()) );
	connect(m_ui.tbResetBlue,				SIGNAL(clicked()),									SLOT(resetRealButton()) );
	connect(m_ui.tbResetGreen,				SIGNAL(clicked()),									SLOT(resetRealButton()) );
	connect(m_ui.tbResetYellow,				SIGNAL(clicked()),									SLOT(resetRealButton()) );
	connect(m_ui.tbResetCyan,				SIGNAL(clicked()),									SLOT(resetRealButton()) );
	connect(m_ui.tbResetOrange,				SIGNAL(clicked()),									SLOT(resetRealButton()) );
	connect(m_ui.tbResetPink,				SIGNAL(clicked()),									SLOT(resetRealButton()) );
	connect(m_ui.tbResetWhite,				SIGNAL(clicked()),									SLOT(resetRealButton()) );
	connect(m_ui.tbResetBlack,				SIGNAL(clicked()),									SLOT(resetRealButton()) );
	connect(m_ui.tbResetGrey,				SIGNAL(clicked()),									SLOT(resetRealButton()) );

	// Detects a real color on click on the scene
	connect(m_ui.videoPlayer->getCwView()->getScene(), SIGNAL(newColorPrevis(int, int, int)), SLOT(updateCurrent(int, int, int)));

	// Ok and Cancel buttons
	connect(m_ui.pb_ok,						SIGNAL(clicked()),									SLOT(okButton()));
	connect(m_ui.pb_cancel,					SIGNAL(clicked()),									SLOT(cancelButton()));
	
	// Updates the real color
	connect( m_ui.tbColorPickerRed,			SIGNAL(clicked()),									SLOT(pipette()) );
	connect( m_ui.tbColorPickerBlue,		SIGNAL(clicked()),									SLOT(pipette()) );
	connect( m_ui.tbColorPickerGreen,		SIGNAL(clicked()),									SLOT(pipette()) );
	connect( m_ui.tbColorPickerYellow,		SIGNAL(clicked()),									SLOT(pipette()) );
	connect( m_ui.tbColorPickerCyan,		SIGNAL(clicked()),									SLOT(pipette()) );
	connect( m_ui.tbColorPickerOrange,		SIGNAL(clicked()),									SLOT(pipette()) );
	connect( m_ui.tbColorPickerPink,		SIGNAL(clicked()),									SLOT(pipette()) );
	connect( m_ui.tbColorPickerWhite,		SIGNAL(clicked()),									SLOT(pipette()) );
	connect( m_ui.tbColorPickerBlack,		SIGNAL(clicked()),									SLOT(pipette()) );
	connect( m_ui.tbColorPickerGrey,		SIGNAL(clicked()),									SLOT(pipette()) );
	
	connect(m_ui.videoPlayer->getCwView()->getScene(), SIGNAL(rectItemModified(int, int, int, int)), SLOT(extractAvgColor(int, int, int, int)));
	//connect(m_ui.videoPlayer->getCwView()->getScene(), SIGNAL(disablePipette(bool)), SLOT(enableAllPipette(bool)));
	//connect(m_ui.videoPlayer->getCwView()->getScene(), SIGNAL(avg(bool)), SLOT(setAvg(bool)));

	// Change webcam
	connect(m_ui.cbWebcam,					SIGNAL(currentIndexChanged(int)),					SLOT(webcamChanged(int)));

	showMaximized();

	m_ui.videoPlayer->delayedFitView();

	QApplication::restoreOverrideCursor();
}

ColorEditWidget::~ColorEditWidget()
{
	m_ui.videoPlayer->pause(false);
}

void ColorEditWidget::setSession(Session* pSession)
{
	ptr_session = pSession;
	
	if (ptr_session)
	{
		m_ui.cbWebcam->clear();

		int idx = 0;
		qDebug() << "ColorEditWidget: videoDevice index: " << idx;

		int nbDevice = VideoDeviceManager::getInstance()->getNbDevice();

		m_ui.cbWebcam->blockSignals(true);
		for (int i = 0; i < nbDevice; i++)
		{
			m_ui.cbWebcam->addItem(QString::number(i + 1), i);
		}

		if (idx != -1)
		{
			m_ui.cbWebcam->setCurrentIndex(idx);
		}
		m_ui.cbWebcam->blockSignals(false);

		
		initCTD();

		setRealPickingColors();

		saveCurrentColor();
	}
}

void ColorEditWidget::initCTD()
{
	qDebug() << "coloreditwidget::initctd: " << VideoDeviceManager::getInstance()->getDevice(m_deviceId);
	currentRed		= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getRed();
	currentOrange	= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getOrange();
	currentYellow	= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getYellow();
	currentGreen	= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getGreen();
	currentCyan		= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getCyan();
	currentBlue		= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getBlue();
	currentPink		= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getPink();
	currentWhite	= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getWhite();
	currentGrey		= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getGrey();
	currentBlack	= VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().getBlack();
}

void ColorEditWidget::setRealPickingColors()
{
	ColorToDetect cTD = VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect();

	QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getRed().red()).arg(cTD.getRed().green()).arg(cTD.getRed().blue());
	m_ui.realPickingRed->setStyleSheet(param);

	param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getOrange().red()).arg(cTD.getOrange().green()).arg(cTD.getOrange().blue());
	m_ui.realPickingOrange->setStyleSheet(param);

	param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getYellow().red()).arg(cTD.getYellow().green()).arg(cTD.getYellow().blue());
	m_ui.realPickingYellow->setStyleSheet(param);

	param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getGreen().red()).arg(cTD.getGreen().green()).arg(cTD.getGreen().blue());
	m_ui.realPickingGreen->setStyleSheet(param);

	param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getCyan().red()).arg(cTD.getCyan().green()).arg(cTD.getCyan().blue());
	m_ui.realPickingCyan->setStyleSheet(param);

	param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getBlue().red()).arg(cTD.getBlue().green()).arg(cTD.getBlue().blue());
	m_ui.realPickingBlue->setStyleSheet(param);

	param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getPink().red()).arg(cTD.getPink().green()).arg(cTD.getPink().blue());
	m_ui.realPickingPink->setStyleSheet(param);

	param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getWhite().red()).arg(cTD.getWhite().green()).arg(cTD.getWhite().blue());
	m_ui.realPickingWhite->setStyleSheet(param);

	param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getGrey().red()).arg(cTD.getGrey().green()).arg(cTD.getGrey().blue());
	m_ui.realPickingGrey->setStyleSheet(param);

	param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getBlack().red()).arg(cTD.getBlack().green()).arg(cTD.getBlack().blue());
	m_ui.realPickingBlack->setStyleSheet(param);

	QColor CTDCurrent = QColor(255, 255, 255);
	param = QString("background-color: rgb(%1, %2, %3)").arg(CTDCurrent.red()).arg(CTDCurrent.green()).arg(CTDCurrent.blue());
	m_ui.lbPrevis->setStyleSheet(param);
	m_ui.lbRed->setText(QString("R: %1").arg(CTDCurrent.red()));
	m_ui.lbGreen->setText(QString("G: %1").arg(CTDCurrent.green()));
	m_ui.lbBlue->setText(QString("B: %1").arg(CTDCurrent.blue()));
}

// slot
void ColorEditWidget::pipette()
{
	if (updateColor)
	{
		enableAllPipette(true);
		updateColor = false;
	}
	else
	{
		avg = false;
		updateColor = true;
		enableAllPipette(false);

		if (sender()->objectName().compare(m_ui.tbColorPickerRed->objectName()) == 0)
		{
			m_ui.tbColorPickerRed->setEnabled(true);
			m_ui.tbColorPickerRed->setChecked(true);
		}
		else if (sender()->objectName().compare(m_ui.tbColorPickerOrange->objectName()) == 0)
		{
			m_ui.tbColorPickerOrange->setEnabled(true);
			m_ui.tbColorPickerOrange->setChecked(true);
		}
		else if (sender()->objectName().compare(m_ui.tbColorPickerYellow->objectName()) == 0)
		{
			m_ui.tbColorPickerYellow->setEnabled(true);
			m_ui.tbColorPickerYellow->setChecked(true);
		}
		else if (sender()->objectName().compare(m_ui.tbColorPickerGreen->objectName()) == 0)
		{
			m_ui.tbColorPickerGreen->setEnabled(true);
			m_ui.tbColorPickerGreen->setChecked(true);
		}
		else if (sender()->objectName().compare(m_ui.tbColorPickerCyan->objectName()) == 0)
		{
			m_ui.tbColorPickerCyan->setEnabled(true);
			m_ui.tbColorPickerCyan->setChecked(true);
		}
		else if (sender()->objectName().compare(m_ui.tbColorPickerBlue->objectName()) == 0)
		{
			m_ui.tbColorPickerBlue->setEnabled(true);
			m_ui.tbColorPickerBlue->setChecked(true);
		}
		else if (sender()->objectName().compare(m_ui.tbColorPickerPink->objectName()) == 0)
		{
			m_ui.tbColorPickerPink->setEnabled(true);
			m_ui.tbColorPickerPink->setChecked(true);
		}
		else if (sender()->objectName().compare(m_ui.tbColorPickerWhite->objectName()) == 0)
		{
			m_ui.tbColorPickerWhite->setEnabled(true);
			m_ui.tbColorPickerWhite->setChecked(true);
		}
		else if (sender()->objectName().compare(m_ui.tbColorPickerGrey->objectName()) == 0)
		{
			m_ui.tbColorPickerGrey->setEnabled(true);
			m_ui.tbColorPickerGrey->setChecked(true);
		}
		else if (sender()->objectName().compare(m_ui.tbColorPickerBlack->objectName()) == 0)
		{
			m_ui.tbColorPickerBlack->setEnabled(true);
			m_ui.tbColorPickerBlack->setChecked(true);
		}
	}

}

void ColorEditWidget::enableAllPipette(bool value)
{
	qDebug() << "enable all pipette";
	
	m_ui.tbColorPickerRed->setChecked(false);
	m_ui.tbColorPickerOrange->setChecked(false);
	m_ui.tbColorPickerYellow->setChecked(false);
	m_ui.tbColorPickerGreen->setChecked(false);
	m_ui.tbColorPickerCyan->setChecked(false);
	m_ui.tbColorPickerBlue->setChecked(false);
	m_ui.tbColorPickerPink->setChecked(false);
	m_ui.tbColorPickerWhite->setChecked(false);
	m_ui.tbColorPickerGrey->setChecked(false);
	m_ui.tbColorPickerBlack->setChecked(false);
	

	m_ui.tbColorPickerRed->setEnabled(value);
	m_ui.tbColorPickerOrange->setEnabled(value);
	m_ui.tbColorPickerYellow->setEnabled(value);
	m_ui.tbColorPickerGreen->setEnabled(value);
	m_ui.tbColorPickerCyan->setEnabled(value);
	m_ui.tbColorPickerBlue->setEnabled(value);
	m_ui.tbColorPickerPink->setEnabled(value);
	m_ui.tbColorPickerWhite->setEnabled(value);
	m_ui.tbColorPickerGrey->setEnabled(value);
	m_ui.tbColorPickerBlack->setEnabled(value);
}

void ColorEditWidget::saveCurrentColor()
{
	qDebug() << "SAVE COLORS : m_deviceId : " << m_deviceId;
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setRed(currentRed);
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setOrange(currentOrange);
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setYellow(currentYellow);
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setGreen(currentGreen);
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setCyan(currentCyan);
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setBlue(currentBlue);
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setPink(currentPink);
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setWhite(currentWhite);
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setGrey(currentGrey);
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect().setBlack(currentBlack);
}

// slot
void ColorEditWidget::resetRealButton()
{
	QString color = sender()->objectName(); // --> "tbResetCOLOR"
	qDebug() << "sender: " << color;
	ColorToDetect cTD = VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect();

	if (color.compare(m_ui.tbResetRed->objectName()) == 0)
	{
		qDebug()<< "reset red";
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getRed().red()).arg(cTD.getRed().green()).arg(cTD.getRed().blue());
		m_ui.realPickingRed->setStyleSheet(param);
		currentRed = cTD.getRed();
	}

	else if (color.compare(m_ui.tbResetOrange->objectName()) == 0)
	{
		qDebug() << "reset orange";
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getOrange().red()).arg(cTD.getOrange().green()).arg(cTD.getOrange().blue());
		m_ui.realPickingOrange->setStyleSheet(param);
		currentOrange = cTD.getOrange();
	}

	else if (color.compare(m_ui.tbResetYellow->objectName()) == 0)
	{
		qDebug() << "reset yellow";
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getYellow().red()).arg(cTD.getYellow().green()).arg(cTD.getYellow().blue());
		m_ui.realPickingYellow->setStyleSheet(param);
		currentYellow = cTD.getYellow();
	}

	else if (color.compare(m_ui.tbResetGreen->objectName()) == 0)
	{
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getGreen().red()).arg(cTD.getGreen().green()).arg(cTD.getGreen().blue());
		m_ui.realPickingGreen->setStyleSheet(param);
		currentGreen = cTD.getGreen();
	}

	else if (color.compare(m_ui.tbResetCyan->objectName()) == 0)
	{
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getCyan().red()).arg(cTD.getCyan().green()).arg(cTD.getCyan().blue());
		m_ui.realPickingCyan->setStyleSheet(param);
		currentCyan = cTD.getCyan();
	}

	else if (color.compare(m_ui.tbResetBlue->objectName()) == 0)
	{
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getBlue().red()).arg(cTD.getBlue().green()).arg(cTD.getBlue().blue());
		m_ui.realPickingBlue->setStyleSheet(param);
		currentBlue = cTD.getBlue();
	}

	else if (color.compare(m_ui.tbResetPink->objectName()) == 0)
	{
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getPink().red()).arg(cTD.getPink().green()).arg(cTD.getPink().blue());
		m_ui.realPickingPink->setStyleSheet(param);
		currentPink = cTD.getPink();
	}

	else if (color.compare(m_ui.tbResetWhite->objectName()) == 0)
	{
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getWhite().red()).arg(cTD.getWhite().green()).arg(cTD.getWhite().blue());
		m_ui.realPickingWhite->setStyleSheet(param);
		currentWhite = cTD.getWhite();
	}

	else if (color.compare(m_ui.tbResetGrey->objectName()) == 0)
	{
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getGrey().red()).arg(cTD.getGrey().green()).arg(cTD.getGrey().blue());
		m_ui.realPickingGrey->setStyleSheet(param);
		currentGrey = cTD.getGrey();
	}

	else if (color.compare(m_ui.tbResetBlack->objectName()) == 0)
	{
		QString param = QString("background-color: rgb(%1, %2, %3)").arg(cTD.getBlack().red()).arg(cTD.getBlack().green()).arg(cTD.getBlack().blue());
		m_ui.realPickingBlack->setStyleSheet(param);
		currentBlack = cTD.getBlack();
	}


}

// slot
void ColorEditWidget::extractAvgColor(int x, int y, int w, int h)
{
	if (w > 1 && h > 1)
	{
		avg = true;
		QPixmap qpixmap(m_ui.videoPlayer->getCwView()->getScene()->getItemPixmap()->pixmap().copy(x, y, w, h));
		Mat mat = Tools::QPixmapToCvMat(qpixmap);

		Scalar avg = mean(mat);
		currentColor = QColor(avg[2], avg[1], avg[0]);
		updatePrevis(avg[2], avg[1], avg[0]);

	}
}

// slot
void ColorEditWidget::updateCurrent(int r, int g, int b)
{
	if (avg)
	{
		r = currentColor.red();
		g = currentColor.green();
		b = currentColor.blue();
	}
	updatePrevis(r, g, b);
	currentColor = QColor(r, g, b);
	
	avg = false;
	setCurrent();
}

void ColorEditWidget::updatePrevis(int r, int g, int b)
{

	QString param = QString("background-color: rgb(%1, %2, %3)").arg(r).arg(g).arg(b);
	m_ui.lbPrevis->setStyleSheet(param);
	// Update RGB labels
	m_ui.lbRed->setText(QString("R: %1").arg(r));
	m_ui.lbGreen->setText(QString("G: %1").arg(g));
	m_ui.lbBlue->setText(QString("B: %1").arg(b));
}

void ColorEditWidget::setCurrent()
{
	QString param = QString("background-color: rgb(%1, %2, %3)").arg(currentColor.red()).arg(currentColor.green()).arg(currentColor.blue());

	qDebug() << "set Current: avg = " << avg;
	if (updateColor)
	{
		// if clicked on Red pipette
		if (m_ui.tbColorPickerRed->isChecked())
		{
			m_ui.realPickingRed->setStyleSheet(param);
			currentRed = currentColor;
		}

		// if clicked on Orange pipette
		else if (m_ui.tbColorPickerOrange->isChecked())
		{
			m_ui.realPickingOrange->setStyleSheet(param);
			currentOrange = currentColor;
		}

		// if clicked on Yellow pipette
		else if (m_ui.tbColorPickerYellow->isChecked())
		{
			m_ui.realPickingYellow->setStyleSheet(param);
			currentYellow = currentColor;
		}

		// if clicked on Green pipette
		else if (m_ui.tbColorPickerGreen->isChecked())
		{
			m_ui.realPickingGreen->setStyleSheet(param);
			currentGreen = currentColor;
		}

		// if clicked on Cyan pipette
		else if (m_ui.tbColorPickerCyan->isChecked())
		{
			m_ui.realPickingCyan->setStyleSheet(param);
			currentCyan = currentColor;
		}

		// if clicked on Blue pipette
		else if (m_ui.tbColorPickerBlue->isChecked())
		{
			m_ui.realPickingBlue->setStyleSheet(param);
			currentBlue = currentColor;
		}

		// if clicked on Pink pipette
		else if (m_ui.tbColorPickerPink->isChecked())
		{
			m_ui.realPickingPink->setStyleSheet(param);
			currentPink = currentColor;
		}

		// if clicked on White pipette
		else if (m_ui.tbColorPickerWhite->isChecked())
		{
			m_ui.realPickingWhite->setStyleSheet(param);
			currentWhite = currentColor;
		}

		// if clicked on Grey pipette
		else if (m_ui.tbColorPickerGrey->isChecked())
		{
			m_ui.realPickingGrey->setStyleSheet(param);
			currentGrey = currentColor;
		}

		// if clicked on Black pipette
		else if (m_ui.tbColorPickerBlack->isChecked())
		{
			m_ui.realPickingBlack->setStyleSheet(param);
			currentBlack = currentColor;
		}
		
		if (!avg)
			enableAllPipette(true);

		updateColor = false;
	}
}

void ColorEditWidget::setAvg(bool value)
{
	qDebug() << "set Avg: " << value;
	avg = value;
}

void ColorEditWidget::displayCurrentColors()
{
	ColorToDetect cTD = VideoDeviceManager::getInstance()->getDevice(m_deviceId)->getColorToDetect();
	qDebug() << "=========== COLOR TO DETECT DEVICE : " << m_deviceId
		<< "\nred : " << cTD.getRed().red() << "/" << cTD.getRed().green() << "/" << cTD.getRed().blue()
		<< "\norange : " << cTD.getOrange().red() << "/" << cTD.getOrange().green() << "/" << cTD.getOrange().blue()
		<< "\nyellow : " << cTD.getYellow().red() << "/" << cTD.getYellow().green() << "/" << cTD.getYellow().blue()
		<< "\ngreen : " << cTD.getGreen().red() << "/" << cTD.getGreen().green() << "/" << cTD.getGreen().blue()
		<< "\ncyan : " << cTD.getCyan().red() << "/" << cTD.getCyan().green() << "/" << cTD.getCyan().blue()
		<< "\nblue : " << cTD.getBlue().red() << "/" << cTD.getBlue().green() << "/" << cTD.getBlue().blue()
		<< "\npink : " << cTD.getPink().red() << "/" << cTD.getPink().green() << "/" << cTD.getPink().blue()
		<< "\nwhite : " << cTD.getWhite().red() << "/" << cTD.getWhite().green() << "/" << cTD.getWhite().blue()
		<< "\ngrey : " << cTD.getGrey().red() << "/" << cTD.getGrey().green() << "/" << cTD.getGrey().blue()
		<< "\nblack : " << cTD.getBlack().red() << "/" << cTD.getBlack().green() << "/" << cTD.getBlack().blue();

}

// slot
void ColorEditWidget::webcamChanged(int value)
{
	qDebug() << "webcam changed: " << value;

	saveCurrentColor();
	displayCurrentColors();

	m_deviceId = value;

	int feedId = VideoDeviceManager::getInstance()->getDevice(value)->getId();

	m_ui.videoPlayer->setDeviceId(value);
	m_ui.videoPlayer->setFeedId(feedId);
	m_ui.videoPlayer->readWebcamFeed();

	initCTD();

	setRealPickingColors();

	displayCurrentColors();
}

// slot
void ColorEditWidget::okButton()
{
	qDebug() << "ok button";

	saveCurrentColor();

	ptr_session->colorToDetectModified();
}

// slot
void ColorEditWidget::cancelButton()
{
	qDebug() << "cancel button";
}