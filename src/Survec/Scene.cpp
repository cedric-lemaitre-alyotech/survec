#include "Scene.h"
#include "CustomRectItem.h"
#include <QGraphicsView>
#include <QGraphicsSceneWheelEvent>
#include <QKeyEvent>
#include <QDebug>
#include <QEvent>
#include <QDebug>
#include <QPixmap>
#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QPointer>


Scene::Scene(QObject * pParent): QGraphicsScene(pParent),
m_buttonPressed(Qt::NoButton),
m_startPos(0,0)
{
	qDebug() << "=== Scene ===";
	// Init
	m_isMoving = false;
	colorMove = false;

	QPixmap pixmap = QPixmap();
	p_itemPixmap = this->addPixmap(pixmap);

	m_rectF = QRectF(0, 0, 0, 0);

	// add rect item
	p_rect = new CustomRectItem(this);
	p_rect->setRect(0, 0, 0, 0);
	setRectangleVisible(true);
	addItem(p_rect);

	p_rect->update();
	p_rect->setActivateConstrain(true);
}

Scene::~Scene()	
{	
	qDebug() << "Scene::~Scene()";
}

void Scene::mousePressEvent(QGraphicsSceneMouseEvent * pMouseEvent)
{
	colorMove = false;
	m_buttonPressed = pMouseEvent->button();
	m_startPos = pMouseEvent->scenePos();
	QGraphicsScene::mousePressEvent(pMouseEvent);
}


void Scene::mouseReleaseEvent(QGraphicsSceneMouseEvent * pMouseEvent)
{
	if (getEnableColor())
	{
		p_rect->setVisible(false);

		if (pMouseEvent->button() == Qt::LeftButton)
		{
			int x = pMouseEvent->scenePos().x();
			int y = pMouseEvent->scenePos().y();
			
			QRgb color = getItemPixmap()->pixmap().toImage().pixel(x, y);
			QColor qcolor(color);

			int r = qcolor.red();
			int g = qcolor.green();
			int b = qcolor.blue();

			m_buttonPressed = Qt::NoButton;

			emit newColorPrevis(r, g, b);
		}
	}
	else
	{
		m_buttonPressed = Qt::NoButton;
		QGraphicsScene::mouseReleaseEvent(pMouseEvent);
	}
}

void Scene::mouseMoveEvent(QGraphicsSceneMouseEvent * pMouseEvent)
{
	p_rect->setConstrain(QRectF(0, 0, getWidth(), getHeight()));
	colorMove = true;

	if (getEnableColor() && (m_buttonPressed == Qt::LeftButton))// && !m_isMoving)
	{
		p_rect->setVisible(true);
	}

	// If draw new rectangle
	if (p_rect->isVisible() && (m_buttonPressed == Qt::LeftButton))// && !m_isMoving)
	{
		//qDebug() << " draw new rec";
		QPointF endPos = pMouseEvent->scenePos();
		QRectF rect;
		QPointF topLeft, bottomRight;
		QRectF constrainedRect = p_rect->getConstrain();
		
		if (m_startPos.x() < endPos.x())
		{
			if (m_startPos.y() < endPos.y())
			{
				rect.setTopLeft(m_startPos);
				rect.setBottomRight(endPos);
			}
			else
			{
				rect.setBottomLeft(m_startPos);
				rect.setTopRight(endPos);
			}
		}
		else
		{
			if (m_startPos.y() < endPos.y())
			{
				rect.setBottomLeft(endPos);
				rect.setTopRight(m_startPos);
			}
			else
			{
				rect.setTopLeft(endPos);
				rect.setBottomRight(m_startPos);
			}
		}


		// If the startPos is inside the constrainedRect
		if (m_startPos.x() > constrainedRect.x() &&
			m_startPos.y() > constrainedRect.y() &&
			m_startPos.x() < constrainedRect.width() &&
			m_startPos.y() < constrainedRect.height())
		{
			// if the size of the rect is bigger than the constrainedRect
			if ((rect.x() + rect.width()) > constrainedRect.width())
			{
				rect.setWidth(constrainedRect.width() - rect.x());
			}
			if ((rect.y() + rect.height()) > constrainedRect.height())
			{
				rect.setHeight(constrainedRect.height() - rect.y());
			}
			if (rect.x() < constrainedRect.x())
			{
				rect.setX(constrainedRect.x());
			}
			if (rect.y() < constrainedRect.y())
			{
				rect.setY(constrainedRect.y());
			}
			rect.normalized();
			m_rectF = QRectF(rect);
			p_rect->setRect(rect);
			p_rect->update();
			qDebug() << "rect item modified";
			emit rectItemModified(rect.x(), rect.y(), rect.width(), rect.height());
		}
	}	
	
	// If move rectangle
	else if (p_rect->isVisible() && (m_buttonPressed == Qt::LeftButton) && m_isMoving)
	{
		QGraphicsScene::mouseMoveEvent(pMouseEvent);
	}

}


void Scene::keyPressEvent(QKeyEvent * event)
{
	qDebug() << " keyPressEvent";
	qDebug() << " = p_rect pos: " << p_rect->x() << "/" << p_rect->y();// << "/" << m_rectF.width() << "/" << m_rectF.height();
	if (event->key() == Qt::Key_Control)
	{
		allowMovement(true);
		QGraphicsScene::keyPressEvent(event);
	}
}


void Scene::keyReleaseEvent(QKeyEvent * event)
{
	qDebug() << " keyReleaseEvent";
	allowMovement(false);
	QGraphicsScene::keyReleaseEvent(event);
}


void Scene::allowMovement(bool value)
{
	m_isMoving = value;
}

QGraphicsPixmapItem * Scene::getItemPixmap() const
{
	return p_itemPixmap;
}

void Scene::setItemPixmap(QPixmap * item)
{
	p_itemPixmap->setPixmap(*item);
	m_pixmapItemWidth = item->width();
	m_pixmapItemHeight = item->height();
}

int Scene::getWidth()
{
	return m_pixmapItemWidth;
}

int Scene::getHeight()
{
	return m_pixmapItemHeight;
}

void Scene::setEnableColor(bool value)
{
	boolEnableColor = value;
}

bool Scene::getEnableColor()
{
	return boolEnableColor;
}

void Scene::setRectangleVisible(bool visible)
{
	p_rect->setVisible(visible);
}

bool Scene::isRectangleVisible() const
{
	return p_rect->isVisible();
}

CustomRectItem* Scene::getRectangle() const
{
	return p_rect;
}