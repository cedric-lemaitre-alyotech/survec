#include "ColorToDetect.h"

using namespace cv;

ColorToDetect::ColorToDetect()
{
	reinit();
}

ColorToDetect::~ColorToDetect()
{
}

void ColorToDetect::reinit()
{
	m_arrayColor.clear();
	m_arrayColor.push_back(QColor(255, 0, 0));
	m_arrayColor.push_back(QColor(255, 160, 0));
	m_arrayColor.push_back(QColor(255, 255, 0));
	m_arrayColor.push_back(QColor(0, 255, 0));
	m_arrayColor.push_back(QColor(0, 255, 255));
	m_arrayColor.push_back(QColor(0, 0, 255));
	m_arrayColor.push_back(QColor(255, 20, 147));
	m_arrayColor.push_back(QColor(255, 255, 255));
	m_arrayColor.push_back(QColor(128, 128, 128));
	m_arrayColor.push_back(QColor(0, 0, 0));
}

QColor ColorToDetect::getRed()
{
	return m_arrayColor[red];
}

QColor ColorToDetect::getOrange()
{
	return m_arrayColor[orange];
}

QColor ColorToDetect::getYellow()
{
	return m_arrayColor[yellow];
}

QColor ColorToDetect::getCyan()
{
	return m_arrayColor[cyan];
}

QColor ColorToDetect::getGreen()
{
	return m_arrayColor[green];
}

QColor ColorToDetect::getBlue()
{
	return m_arrayColor[blue];
}

QColor ColorToDetect::getPink()
{
	return m_arrayColor[pink];
}

QColor ColorToDetect::getWhite()
{
	return m_arrayColor[white];
}

QColor ColorToDetect::getGrey()
{
	return m_arrayColor[grey];
}

QColor ColorToDetect::getBlack()
{
	return m_arrayColor[black];
}

QColor ColorToDetect::getCurrent()
{
	return m_current;
}

void ColorToDetect::setRed(QColor colorRed)
{
	m_arrayColor[red] = colorRed;
}

void ColorToDetect::setOrange(QColor colorOrange)
{
	m_arrayColor[orange] = colorOrange;
}

void ColorToDetect::setYellow(QColor colorYellow)
{
	m_arrayColor[yellow] = colorYellow;
}

void ColorToDetect::setCyan(QColor colorCyan)
{
	m_arrayColor[cyan] = colorCyan;
}

void ColorToDetect::setGreen(QColor colorGreen)
{
	m_arrayColor[green] = colorGreen;
}

void ColorToDetect::setBlue(QColor colorBlue)
{
	m_arrayColor[blue] = colorBlue;
}

void ColorToDetect::setPink(QColor colorPink)
{
	m_arrayColor[pink] = colorPink;
}

void ColorToDetect::setWhite(QColor colorWhite)
{
	m_arrayColor[white] = colorWhite;
}

void ColorToDetect::setGrey(QColor colorGrey)
{
	m_arrayColor[grey] = colorGrey;
}

void ColorToDetect::setBlack(QColor colorBlack)
{
	m_arrayColor[black] = colorBlack;
}

void ColorToDetect::setCurrent(QColor current)
{
	m_current = current;
}

QColor ColorToDetect::getIndexColor(int index)
{
	if (index >= 0 && index < 10)
		return m_arrayColor[index];
	else
		return QColor(128, 128, 128);
}

QString ColorToDetect::getIndexColorName(int index)
{
	switch (index)
	{
	case 0: // red
		return "red";
		break;
	case 1: // orange
		return "orange";
		break;
	case 2: // yellow
		return "yellow";
		break;
	case 3: // green
		return "green";
		break;
	case 4: // cyan
		return "cyan";
		break;
	case 5: // blue
		return "blue";
		break;
	case 6: // pink
		return "pink";
		break;
	case 7: // white
		return "white";
		break;
	case 8: // grey
		return "grey";
		break;
	case 9: // black
		return "black";
		break;
	default:
		return "no_color";
	}
}

void ColorToDetect::setColor(QString name, int h, int s, int v)
{
	QColor color = QColor::fromHsv(h, s, v);

	if (name == "red")
		setRed(color);		
	else if (name == "orange")
		setOrange(color);
	else if (name == "yellow")
		setYellow(color);
	else if (name == "green")
		setGreen(color);
	else if (name == "cyan")
		setCyan(color);
	else if (name == "blue")
		setBlue(color);
	else if (name == "pink")
		setPink(color);
	else if (name == "white")
		setWhite(color);
	else if (name == "grey")
		setGrey(color);
	else
		setBlack(color);
}
