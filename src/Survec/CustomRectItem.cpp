#include "CustomRectItem.h"
#include <cmath>
#include <Scene.h>

#include <QDebug>
#include <QPainter>
#include <QPen>
#include <QBrush>
#include <QGraphicsSceneEvent>
#include <QGraphicsPixmapItem>
#include <QGraphicsRectItem>
	

CustomRectItem::CustomRectItem(Scene *pParent): QGraphicsRectItem()
{
	qDebug() << "=== CustomRectItem ===";

	ptr_scene = pParent;
	setFlags(flags() | QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsMovable | QGraphicsItem::ItemIsFocusable);
	setPen(QPen(Qt::red, 2));
}	

CustomRectItem::~CustomRectItem()
{
	qDebug() << "CustomRectItem::~CustomRectItem()";
}

void CustomRectItem::setColor(const QColor & color)
{
	setPen(QPen(color));
}

void CustomRectItem::setConstrain(const QRectF & rect)
{
	m_constrained.setRect(rect.x(), rect.y(), rect.width(), rect.height());
}

QRectF CustomRectItem::getConstrain()
{
	return m_constrained;
}

void CustomRectItem::setActivateConstrain(bool constrained)
{
	m_activateConstrained = constrained;
}

QRectF CustomRectItem::rectFromArea(Qt::Corner corner) const
{
	qreal x, y, width, height;
	double margin;
	QRectF normalizedRect;

	normalizedRect = rect().normalized();
	x = normalizedRect.x();
	y = normalizedRect.y();
	width = normalizedRect.width();
	height = normalizedRect.height();

	margin = 3;

	switch (corner)
	{
		case Qt::TopLeftCorner:
			return QRectF(x - margin, y - margin, 2 * margin, 2 * margin);
		case Qt::TopRightCorner:
			return QRectF(x + width - margin, y - margin, 2 * margin, 2 * margin);
		case Qt::BottomLeftCorner:
			return QRectF(x - margin, y + height - margin, 2 * margin, 2 * margin);
		case Qt::BottomRightCorner:
			return QRectF(x + width - margin, y + height - margin, 2 * margin, 2 * margin);
		default:
			return QRectF();
	};
}

int CustomRectItem::areaFromPos(const QPointF & pos) const
{
	int i;
	int res = -1;

	for (i = 0; i < 4; ++i)
	{
		if (rectFromArea(Qt::Corner(i)).contains(pos))
		{
			res = i;
		}
	}

	return res;
}



void CustomRectItem::paint(QPainter * painter, const QStyleOptionGraphicsItem * /*option*/, QWidget * /*widget*/)
{
	if (isVisible())
	{
		unsigned int i;

		//save the painter
		painter->save();

		//paint the rect (do not call QGraphicsRectItem::paint because it draws the bounding rect when selected)
		painter->setPen(pen());
		painter->fillRect(rect(), brush());
		painter->drawRect(rect());


		//restore the painter
		painter->restore();
	}		
}

QRectF CustomRectItem::boundingRect() const
{
	int i;
	QRectF rect = QGraphicsRectItem::boundingRect();

	if (isSelected())
	{
		//add the four resize handles
		for (i = 0; i<4; ++i)
		{
			rect = rect.united(rectFromArea(Qt::Corner(i)));
		}
	}

	return rect;
}

QPainterPath CustomRectItem::shape() const
{
	QPainterPath res;

	res.addRect(boundingRect());

	return res;
}