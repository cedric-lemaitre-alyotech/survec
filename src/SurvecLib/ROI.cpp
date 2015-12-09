#include "ROI.h"
#include <QTime>
#include <QDebug>
#include <QDebug>
#include <VideoDeviceManager.h>
using namespace cv;

ROI::ROI() :
ptr_device(NULL)
{
	qDebug() << "=== ROI: constructor default ===";

	// Init
	m_isActive = false;
	m_alarmOnEquals = true;
	m_processStarted = false;
	m_tolerance = 100;
	m_recordROI = false;
	m_filtration = 0;
	m_repetitionCount = 0;

	m_bounds.x = 0;
	m_bounds.y = 0;
	m_bounds.width = 0;
	m_bounds.height = 0;
	m_videoDeviceId = 0;

	if (VideoDeviceManager::getInstance()->getVideoDeviceList().size() > 0)
	{
		ptr_device = VideoDeviceManager::getInstance()->getDevice(m_videoDeviceId);
		if (ptr_device)
		{
			m_videoDeviceId = ptr_device->getId();
			// set bounds
			m_bounds.x = 0;
			m_bounds.y = 0;
			m_bounds.width = ptr_device->getImageWidth();
			m_bounds.height = ptr_device->getImageHeight();
		}
	}

	m_detectionTypeArray[UNDEFINED_DT]		= (new UndefinedDetection());
	m_detectionTypeArray[COLOR_DT]			= (new ColorDetection());
	m_detectionTypeArray[SYMBOL_DT]			= (new SymbolDetection());
	m_detectionTypeArray[TEXT_DT]			= (new TextDetection());
	m_detectionTypeArray[NUMBER_DT]			= (new NumberDetection());

	// Connect all the DetectionType 'changed' signals to Session
	for (int i = 0; i < NB_DETECTION_TYPE; i++)
	{
		connect(m_detectionTypeArray[i], SIGNAL(changed()), SLOT(emitChanged()));
		connect(m_detectionTypeArray[i], SIGNAL(alarmChanged()), SLOT(emitAlarmChanged()));
	}

	setCurrentDetectionType(UNDEFINED_DT);

	qDebug() << "=== End ROI constructor default ===";
}

ROI::ROI(VideoDevice * pVideoDevice)
{
	qDebug() << "=== ROI: constructor ===";

	m_videoDeviceId = 0;
	ptr_device = pVideoDevice; // should be ptr_device because the videoDevice wasn't created by this class
	if (ptr_device)
	{
		m_videoDeviceId = ptr_device->getId();
		// set bounds
		m_bounds.x = 0;
		m_bounds.y = 0;
		m_bounds.width = ptr_device->getImageWidth();
		m_bounds.height = ptr_device->getImageHeight();
	}

	//if (VideoDeviceManager::getInstance()->getVideoDeviceList().size() > 0)
	//{
	//	m_videoDeviceId = VideoDeviceManager::getInstance()->getDevice(0)->getId();
	//}

	m_detectionTypeArray[UNDEFINED_DT]	=		(new UndefinedDetection());
	m_detectionTypeArray[COLOR_DT]		=		(new ColorDetection());
	m_detectionTypeArray[SYMBOL_DT]		=		(new SymbolDetection());
	m_detectionTypeArray[TEXT_DT]		=		(new TextDetection());
	m_detectionTypeArray[NUMBER_DT]		=		(new NumberDetection());

	connect(m_detectionTypeArray[UNDEFINED_DT], SIGNAL(changed()), this, SIGNAL(changed()));
	connect(m_detectionTypeArray[COLOR_DT],		SIGNAL(changed()), this, SIGNAL(changed()));
	connect(m_detectionTypeArray[SYMBOL_DT],	SIGNAL(changed()), this, SIGNAL(changed()));
	connect(m_detectionTypeArray[TEXT_DT],		SIGNAL(changed()), this, SIGNAL(changed()));
	connect(m_detectionTypeArray[NUMBER_DT],	SIGNAL(changed()), this, SIGNAL(changed()));

	setCurrentDetectionType(UNDEFINED_DT);

	qDebug() << "=== End ROI constructor ===";
}

ROI::~ROI()
{
	for (int i = 0; i < NB_DETECTION_TYPE; ++i)
	{
		disconnect(m_detectionTypeArray[i], SIGNAL(changed()), this, SIGNAL(changed()));
		delete m_detectionTypeArray[i];
		m_detectionTypeArray[i] = NULL;
	}
}

void ROI::colorToDetectModified()
{
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	ptrColorDetection->colorToDetectModified(ptr_device->getColorToDetect());
}

void ROI::emitChanged()
{
	emit changed();
}

void ROI::emitAlarmChanged()
{
	emit alarmChanged();
}

void ROI::setVideoDevice(VideoDevice * pVideoDevice)
{
	ptr_device = pVideoDevice; // should be ptr_device because the videoDevice wasn't created by this class
	if (ptr_device)
	{
		m_videoDeviceId = pVideoDevice->getId();
		// set bounds
		m_bounds.x = 0;
		m_bounds.y = 0;
		m_bounds.width = ptr_device->getImageWidth();
		m_bounds.height = ptr_device->getImageHeight();
		emit changed();
	}
	// else ?  m_bounds = {0,0,0,0} ?
}

int ROI::getVideoDeviceId() const
{
	return m_videoDeviceId;
}

VideoDevice * ROI::getVideoDevice()
{
	return ptr_device;
}

void ROI::setVideoDeviceId(int id)
{
	m_videoDeviceId = id;

	ptr_device = VideoDeviceManager::getInstance()->getDevice(m_videoDeviceId);
}

void ROI::retrieveImage()
{
	m_roiImage = ptr_device->getImage();
	//qDebug() << "=== Process image : " << ptr_device->imageIndex();
	m_roiImage = m_roiImage(cv::Rect(m_bounds.x, m_bounds.y, m_bounds.width, m_bounds.height));
}


cv::Mat& ROI::getImage()
{
	//qDebug() << "ROI getImage";
	if (m_roiImage.empty())
	{
		qDebug() << "";
		qDebug() << "----------------------";
		qDebug() << "--- getImage empty ---";
		qDebug() << "----------------------";
		qDebug() << "";
	}
	return m_roiImage;
}


QString	ROI::getWorkingDir()
{
	return m_workingDir;
}

void ROI::setWorkingDir(QString dir)
{
	m_workingDir = dir;
}

void ROI::getImage(cv::Mat & image)
{
	m_roiImage.copyTo(image);
}

void ROI::setRecordDuration(QTime duration)
{
	m_recordDuration = duration;
}

QTime & ROI::getRecordDuration()
{
	return m_recordDuration;
}

void ROI::setRecordROI(bool val)
{
	m_recordROI = val;
}

bool ROI::getRecordROI()
{
	return m_recordROI;
}

bool ROI::getAlarm()
{
	UndefinedDetection* ptrUndefinedDetection = dynamic_cast<UndefinedDetection*>(getDetectionType(ROI::UNDEFINED_DT));
	if (ptrUndefinedDetection && getCurrentDetectionType() == 0)
	{
		return false;
	}

	// Color
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		return ptrColorDetection->getAlarm();
	}

	// Symbol
	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(getDetectionType(ROI::SYMBOL_DT));
	if (ptrSymbolDetection && getCurrentDetectionType() == 2)
	{
		return ptrSymbolDetection->getAlarm();
	}

	// Text
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		return ptrTextDetection->getAlarm();
	}

	// Number
	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection && getCurrentDetectionType() == 4)
	{
		return ptrNumberDetection->getAlarm();
	}
	return false;
}

bool ROI::getAlarmValid()
{
	UndefinedDetection* ptrUndefinedDetection = dynamic_cast<UndefinedDetection*>(getDetectionType(ROI::UNDEFINED_DT));
	if (ptrUndefinedDetection && getCurrentDetectionType() == 0)
	{
		return false;
	}

	// Color
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		return ptrColorDetection->getAlarmValid();
	}

	// Symbol
	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(getDetectionType(ROI::SYMBOL_DT));
	if (ptrSymbolDetection && getCurrentDetectionType() == 2)
	{
		return ptrSymbolDetection->getAlarmValid();
	}

	// Text
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		return ptrTextDetection->getAlarmValid();
	}

	// Number
	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection && getCurrentDetectionType() == 4)
	{
		return ptrNumberDetection->getAlarmValid();
	}
	return false;
}

void ROI::setAlarm(bool val)
{
	// Color
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		ptrColorDetection->setAlarm(val);
	}

	// Symbol
	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(getDetectionType(ROI::SYMBOL_DT));
	if (ptrSymbolDetection && getCurrentDetectionType() == 2)
	{
		ptrSymbolDetection->setAlarm(val);
	}

	// Text
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		ptrTextDetection->setAlarm(val);
	}

	// Number
	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection && getCurrentDetectionType() == 4)
	{
		ptrNumberDetection->setAlarm(val);
	}
}

bool ROI::getAlarmOnEquals() const
{
	// get alarm color
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		return ptrColorDetection->getAlarmOnEquals();
	}

	// get alarm Symbol
	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(getDetectionType(ROI::SYMBOL_DT));
	if (ptrSymbolDetection && getCurrentDetectionType() == 2)
	{
		return ptrSymbolDetection->getAlarmOnEquals();
	}

	// get alarm Text
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		return ptrTextDetection->getAlarmOnEquals();
	}

	return false;
}

void ROI::setOperator(QString op)
{
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		ptrColorDetection->setAlarmOnEquals((op == "1"));
	}

	// get alarm Symbol
	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(getDetectionType(ROI::SYMBOL_DT));
	if (ptrSymbolDetection && getCurrentDetectionType() == 2)
	{
		ptrSymbolDetection->setAlarmOnEquals((op == "1"));
	}

	// get alarm Text
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		ptrTextDetection->setAlarmOnEquals((op == "1"));
	}

	// get alarm number
	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection && getCurrentDetectionType() == 4)
	{
		ptrNumberDetection->setOperatorString(op);
	}
}

void ROI::setCurrentDetectionType(detectionType type)
{
	qDebug() << "ROI: setCurrentDetectionType (" << type << ")";
	if (m_detectionType != type)
	{
		m_detectionType = type;
		emit changed();
	}	
}

void ROI::setCurrentDetectionTypeAsQString(QString type)
{
	if (type == "undefined")
		m_detectionType = ROI::UNDEFINED_DT;
	else if (type == "color")
		m_detectionType = ROI::COLOR_DT;
	else if (type == "number")
		m_detectionType = ROI::NUMBER_DT;
	else if (type == "text")
		m_detectionType = ROI::TEXT_DT;
	else
		m_detectionType = ROI::SYMBOL_DT;
}

ROI::detectionType ROI::getCurrentDetectionType() const
{
	return m_detectionType;
}

DetectionType *	ROI::getDetectionType(detectionType type) const
{
	return m_detectionTypeArray[type];
}

void ROI::setBounds(int *bounds)
{
	if ((m_bounds.x != bounds[0]) || (m_bounds.y != bounds[1]) || (m_bounds.width != bounds[2]) || (m_bounds.height != bounds[3]))
	{
		m_bounds.x = bounds[0];
		m_bounds.y = bounds[1];
		m_bounds.width = bounds[2];
		m_bounds.height = bounds[3];
		emit changed();
	}
}

void ROI::setBounds(int x, int y, int width, int height)
{
	if ((m_bounds.x != x) || (m_bounds.y != y) || (m_bounds.width != width) || (m_bounds.height != height))
	{
		m_bounds.x = x;
		m_bounds.y = y;
		m_bounds.width = width;
		m_bounds.height = height;
		emit changed();
	}
}

void ROI::setDebug(bool val)
{
	m_debug = val;
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		ptrColorDetection->setDebug(val);
	}

	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(getDetectionType(ROI::SYMBOL_DT));
	if (ptrSymbolDetection && getCurrentDetectionType() == 2)
	{
		ptrSymbolDetection->setDebug(val);
	}

	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		ptrTextDetection->setDebug(val);
	}

	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection && getCurrentDetectionType() == 4)
	{
		ptrNumberDetection->setDebug(val);
	}


}

bool ROI::getDebug()
{
	return m_debug;
}

void ROI::getBounds(int * bounds) const
{
	bounds[0] = m_bounds.x;
	bounds[1] = m_bounds.y;
	bounds[2] = m_bounds.width;
	bounds[3] = m_bounds.height;
}

void ROI::getBounds(rectangle & bounds) const
{
	bounds.x = m_bounds.x;
	bounds.y = m_bounds.y;
	bounds.width = m_bounds.width;
	bounds.height = m_bounds.height;
}

void ROI::setActive(bool value)
{
	if (m_isActive != value)
	{
		m_isActive = value;
		emit changed();
	}	
}

bool ROI::getActive() const
{
	return m_isActive;
}

void ROI::setProcessStarted(bool value)
{
	m_processStarted = value;
}

bool ROI::getProcessStarted()
{
	return m_processStarted;
}

float ROI::getTolerance() const
{
	return m_tolerance;
}

void ROI::setTolerance(float value)
{
	m_tolerance = value;

	// Color
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		ptrColorDetection->setTolerance(m_tolerance);
	}

	// Symbol
	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(getDetectionType(ROI::SYMBOL_DT));
	if (ptrSymbolDetection && getCurrentDetectionType() == 2)
	{
		ptrSymbolDetection->setTolerance(m_tolerance);
	}
}

int ROI::getFiltration() const
{
	return m_filtration;
}

void ROI::setFiltration(int value)
{
	m_filtration = value;
}

int ROI::getRepetitionCount() const
{
	return m_repetitionCount;
}

void ROI::setRepetitionCount(int value)
{
	m_repetitionCount = value;
}

QString ROI::getCurrent()
{
	UndefinedDetection* ptrUndefinedDetection = dynamic_cast<UndefinedDetection*>(getDetectionType(ROI::UNDEFINED_DT));
	if (ptrUndefinedDetection && getCurrentDetectionType() == 0)
	{
		return "";
	}

	// get color
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		QColor color = ptrColorDetection->getCurrentColor();
		return color.name();
	}

	// get Symbol
	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(getDetectionType(ROI::SYMBOL_DT));
	if (ptrSymbolDetection && getCurrentDetectionType() == 2)
	{
		//return ptrSymbolDetection->getCurrent();
		return "IMAGE";
	}

	// get Text
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		return ptrTextDetection->getCurrent();
	}

	// get number
	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection && getCurrentDetectionType() == 4)
	{
		return QString::number(ptrNumberDetection->getCurrent());
	}
	return "";
}


QString ROI::getTarget()
{
	UndefinedDetection* ptrUndefinedDetection = dynamic_cast<UndefinedDetection*>(getDetectionType(ROI::UNDEFINED_DT));
	if (ptrUndefinedDetection && getCurrentDetectionType() == 0)
	{
		return "";
	}

	// enable color
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		QColor color = ptrColorDetection->getColor();
		return color.name();
	}

	// enable Symbol
	SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(getDetectionType(ROI::SYMBOL_DT));
	if (ptrSymbolDetection && getCurrentDetectionType() == 2)
	{
		//return ptrSymbolDetection->getTarget();
		return "IMAGE";
	}

	// enable Text
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		return ptrTextDetection->getTarget();
	}

	// enable number
	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection && getCurrentDetectionType() == 4)
	{
		QString res = QString::number(ptrNumberDetection->getTarget());
		QString op = ptrNumberDetection->getOperatorString();
		return op + " " + res;
	}
	return "";
}

void ROI::setTarget(QString target)
{
	// enable color
	ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(getDetectionType(ROI::COLOR_DT));
	if (ptrColorDetection && getCurrentDetectionType() == 1)
	{
		ptrColorDetection->setTarget(target);
	}

	// enable Text
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		ptrTextDetection->setTarget(target);
	}

	// enable number
	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection && getCurrentDetectionType() == 4)
	{
		ptrNumberDetection->setTarget(target);
	}
}

void ROI::setFont(QString font)
{
	// enable Text
	TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(getDetectionType(ROI::TEXT_DT));
	if (ptrTextDetection && getCurrentDetectionType() == 3)
	{
		ptrTextDetection->setFont(font);
	}

	// enable number
	NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(getDetectionType(ROI::NUMBER_DT));
	if (ptrNumberDetection && getCurrentDetectionType() == 4)
	{
		ptrNumberDetection->setFont(font);
	}
}

ROI& ROI::operator=(const ROI& other)
{	
	other.getBounds(m_bounds);

	m_detectionType = other.getCurrentDetectionType();
	m_videoDeviceId = other.getVideoDeviceId();
	ptr_device = VideoDeviceManager::getInstance()->getDevice(m_videoDeviceId);
	m_isActive = other.getActive();
	m_alarmOnEquals = other.getAlarmOnEquals();
	m_tolerance = other.getTolerance();
	m_filtration = other.getFiltration();

	ColorDetection* pCD = dynamic_cast<ColorDetection*> (m_detectionTypeArray[COLOR_DT]);
	ColorDetection* otherCD = dynamic_cast<ColorDetection*> (other.getDetectionType(COLOR_DT));
	pCD->setColor(otherCD->getColor());
	pCD->setIndexColor(otherCD->getIndexColor());	
	pCD->setAlarmOnEquals(otherCD->getAlarmOnEquals());
	pCD->setTolerance(m_tolerance);
	
	SymbolDetection* pSD = dynamic_cast<SymbolDetection*> (m_detectionTypeArray[SYMBOL_DT]);
	SymbolDetection* otherSD = dynamic_cast<SymbolDetection*> (other.getDetectionType(SYMBOL_DT));
	Mat temp = otherSD->getSymbol();
	if (!temp.empty())
	{
		cvtColor(temp, temp, CV_BGRA2BGR);
		pSD->setSymbol(temp);
	}
	pSD->setAlarmOnEquals(otherSD->getAlarmOnEquals());
	pSD->setTolerance(m_tolerance);
	
	TextDetection* pTD = dynamic_cast<TextDetection*> (m_detectionTypeArray[TEXT_DT]);
	TextDetection* otherTD = dynamic_cast<TextDetection*> (other.getDetectionType(TEXT_DT));
	QString text = otherTD->getTextToDetect();
	if (!text.isNull())
		pTD->setTextToDetect(text);
	pTD->setAlarmOnEquals(otherTD->getAlarmOnEquals());
	pTD->setFontIndex(otherTD->getFontIndex());
	pTD->setTesseractNull();
	
	NumberDetection* pND = dynamic_cast<NumberDetection*> (m_detectionTypeArray[NUMBER_DT]);
	NumberDetection* otherND = dynamic_cast<NumberDetection*> (other.getDetectionType(NUMBER_DT));
	pND->setNumber(otherND->getNumber());
	pND->setOperatorString(otherND->getOperatorString());
	pND->setOperator(otherND->getOperator());
	pND->setFontIndex(otherND->getFontIndex());
	pND->setTesseractNull();
	
	p_currentDetection = m_detectionTypeArray[m_detectionType];
	
	emit changed();
	
	return *this;
}
