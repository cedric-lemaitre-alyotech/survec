#include "cwVideoPlayer.h"
#include <QDebug>
#include <CustomRectItem.h>
#include <Tools.h>
#include <QPointer>
#include <QTime>

cwVideoPlayer::cwVideoPlayer(QWidget * pParent, Qt::WindowFlags flags): QWidget(pParent, flags)
{
	qDebug() << "=== cw Video Player ===";
	m_ui.setupUi(this);

	p_videoDevice = NULL;
	
	// Init
	readFullPixmap	= false;
	isPause			= true;
	m_deviceId		= 0;
	m_image			= Mat(1080, 1920, CV_8UC3);

	bounds[0] = 0;
	bounds[1] = 0;
	bounds[2] = m_ui.view->getScene()->getWidth();
	bounds[3] = m_ui.view->getScene()->getHeight();

	readWebcamFeed();
		
	qDebug() << "== end video player constructor ==";

	// === Connect ===
	connect(m_ui.pbPlayPause,	SIGNAL(clicked()),	SLOT(playPause()) );
	connect(m_ui.pbFit,			SIGNAL(clicked()),	SLOT(fitView())   );

	m_ui.pbPlayPause->setVisible(false);

	connect(&m_fitDelayTimer, SIGNAL(timeout()), this, SLOT(fitView())); 
	m_fitDelayTimer.setInterval(200);
	m_fitDelayTimer.stop();

	updateGUI();
	playPause();
}

void cwVideoPlayer::showEvent(QShowEvent * event)
{
	m_ui.view->fitInView(m_ui.view->getScene()->itemsBoundingRect(), Qt::KeepAspectRatio);
}

cwVideoPlayer::~cwVideoPlayer()
{	
	qDebug() << "cwVideoPlayer::~cwVideoPlayer()";
	if (p_videoDevice)
		p_videoDevice->detachView();

	if (p_pixmap)
	{
		delete p_pixmap;
		p_pixmap = NULL;
	}
}

bool cwVideoPlayer::isStopped()
{
	return isPause;
}

cwView * cwVideoPlayer::getCwView()
{
	return m_ui.view;
}

void cwVideoPlayer::setROIInfo(int device /* , rectangle*/) //TODO
{

}

void cwVideoPlayer::setButtonVisible(bool visible)
{
	m_ui.wButtonBar->setVisible(visible);
}

void cwVideoPlayer::setPixmapRes(QPixmap &pixmap, int x, int y, int w, int h)
{
	m_pixmapRes = QPixmap(pixmap.copy(x, y, w, h));
}

void cwVideoPlayer::setBounds(int x, int y, int w, int h)
{
	bounds[0] = x;
	bounds[1] = y;
	bounds[2] = w;
	bounds[3] = h;
}

void cwVideoPlayer::pause(bool value)
{
	isPause = value;
	playPause();
}

void cwVideoPlayer::delayedFitView()
{
	m_fitDelayTimer.start();
}

// slot
void cwVideoPlayer::fitView()
{
	//Calculates and returns the bounding rect of all items on the scene.
	//This function works by iterating over all items, and because if this, it can be slow for large scenes.
	//getCwView()->getScene()->getRectangle()->setRect(getCwView()->getScene()->getItemPixmap()->pixmap().rect());

	QRectF rect = m_ui.view->getScene()->itemsBoundingRect();
	m_ui.view->getScene()->setSceneRect(rect);

	m_ui.view->fitInView(m_ui.view->getScene()->sceneRect(), Qt::AspectRatioMode::KeepAspectRatio);

	m_fitDelayTimer.stop();
}

// slot
void cwVideoPlayer::playPause()
{
	// if isPause -> play
	if (isPause)
	{
		//qDebug() << "play";
		isPause = false;
		m_ui.pbPlayPause->setStyleSheet("qproperty-icon: url(:/resources/resources/icons/pause.png);");
	}
	// if !isPause -> pause
	else
	{
		//qDebug() << "pause";
		isPause = true;

		m_ui.pbPlayPause->setStyleSheet("qproperty-icon: url(:/resources/resources/icons/play.png);");
	}
}

// slot
void cwVideoPlayer::updateGUI()
{	
	//QTime timeUpdate;
	//timeUpdate.start();
	// Reads the default image
	//m_ui.view->setImage(new QPixmap(1920, 1080));
	
	//qDebug() << " -- read image device=" << p_videoDevice->getId();
	if (width() > 1 && p_videoDevice)
	{
		m_image = p_videoDevice->getImage();
		setPixmapRes(Tools::cvMatToQPixmap(m_image), bounds[0], bounds[1], bounds[2], bounds[3]);

		m_ui.view->setImage(&m_pixmapRes);

		emit newImageWebcam();

		m_image.release();

		//qDebug() << " -- display image t=" << timeUpdate.elapsed();

	}
}

// slot
void cwVideoPlayer::setReadFullPixmap(bool value)
{
	readFullPixmap = value;
}

void cwVideoPlayer::readWebcamFeed()
{
	qDebug() << "--> readWebcamFeed - m_deviceId: " << m_deviceId;
	// Reads the webcam feed
	if (!VideoDeviceManager::getInstance()->getVideoDeviceList().isEmpty() && VideoDeviceManager::getInstance()->getVideoDeviceList().at(m_deviceId) != NULL)
	{
		if (p_videoDevice != NULL)
		{
			p_videoDevice->detachView();
			disconnect(p_videoDevice, SIGNAL(imageReaded()), this, SLOT(updateGUI()));
		}

		p_videoDevice = VideoDeviceManager::getInstance()->getDevice(m_deviceId);
		qDebug() << "--> readWebcamFeed - connect(): " << p_videoDevice->getId();
		
		p_videoDevice->readImage();
		m_image = p_videoDevice->getImage();
		p_pixmap = new QPixmap(Tools::cvMatToQPixmap(m_image));
		m_pixmapRes = QPixmap(p_pixmap->copy(bounds[0], bounds[1], bounds[2], bounds[3]));

		p_videoDevice->attachView();
		connect(p_videoDevice, SIGNAL(imageReaded()), this, SLOT(updateGUI()));

		if (p_pixmap)
		{
			delete p_pixmap;
			p_pixmap = NULL;
		}
		
		m_image.release();
	}
}

void cwVideoPlayer::setFeedId(int id)
{
	VideoDeviceManager::getInstance()->getDevice(m_deviceId)->setId(id);
	emit webcamChanged();
}

void cwVideoPlayer::setDeviceId(int id)
{
	m_deviceId = id;
}

int cwVideoPlayer::getDeviceId()
{
	return m_deviceId;
}

void cwVideoPlayer::detachVideoDevice()
{
	p_videoDevice->detachView();
	p_videoDevice = NULL;
}