#ifndef SCENE_H
#define SCENE_H

#include <QGraphicsScene>
#include <QtGui/QImage>
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <QGraphicsPixmapItem>
#include <opencv/highgui.h>

class CustomRectItem;

class Scene: public QGraphicsScene
{
	Q_OBJECT

public:

	/**
	*	Constructor
	*/
	Scene(QObject * pParent = NULL);
	
	/**
	*	Destructor
	*/
	virtual ~Scene();

	/**
	*	@brief: gets the pointer to the QGraphicsPixmapItem displayed in the scene
	*/
	QGraphicsPixmapItem *				getItemPixmap() const;

	/**
	*	@brief: sets the QPixmap to display
	*	@param: the pixmap to display
	*/
	void								setItemPixmap(QPixmap * item);

	/**
	*	@brief: gets the pixmapItem width
	*/
	int									getWidth();

	/**
	*	@brief: gets the pixmapItem height
	*/
	int									getHeight();

	/**
	*	@brief: sets the enable color
	*/
	void								setEnableColor(bool value);

	/**
	*	@brief: gets the enable color
	*/
	bool								getEnableColor();

	/**
	*	@brief: sets the rectangle to visible
	*	@param bool visible: set to true to see the rectangle
	*/
	void								setRectangleVisible(bool visible);

	/**
	*	@brief: gets the rectangle visibility
	*	@return bool: true if the rectangle is visible, false otherwise
	*/
	bool								isRectangleVisible() const;

	/**
	*	@brief: gets the rectangle
	*	@return CustomRectItem: the rectangle
	*/
	CustomRectItem*						getRectangle() const;

	/**
	*	@brief: allow the movement of the rectangle while pressing the CTRL key
	*	@param bool value: set the m_isMoving variable to 'value'
	*/
	void								allowMovement(bool value);

signals:
	/** Signal: send to ColorEditWidget to update the color previsualisation label
	*	@param r,g,b: the color of the pixel the user clicked
	*/
	void								newColorPrevis(int r, int g, int b);

	/** Signal: send to ROIConfigurationWidget to update the ROI's rectangle
	*	@param x, y, w, h: (x,y) (topleft corner of rectangle), w = width and h = height
	*/
	void								rectItemModified(int x, int y, int w, int h);

protected:

	/**
	*	@brief: mouse Move event
	*/
	virtual void						mousePressEvent(QGraphicsSceneMouseEvent * pMouseEvent);

	/**
	*	@brief: mouse release event
	*/
	virtual void						mouseReleaseEvent(QGraphicsSceneMouseEvent * pMouseEvent);

	/**
	*	@brief: mouse press event
	*/
	virtual void						mouseMoveEvent(QGraphicsSceneMouseEvent * pMouseEvent);

	/**
	*	@brief: key press event
	*/
	virtual void						keyPressEvent(QKeyEvent * event);

	/**
	*	@brief: key release event
	*/
	virtual void						keyReleaseEvent(QKeyEvent * event);

private:
	QGraphicsPixmapItem *				p_itemPixmap;			//!<the pixmapItem to display
	CustomRectItem *					p_rect;					//!<Rectangle use for region selection

	bool								boolEnableColor;		//!<allow to pick a color
	int									m_pixmapItemWidth ;		//!<width of the rectangle: sent to CustomRectItem to limit the size of the ROI to the pixmap size
	int									m_pixmapItemHeight;		//!<height of the rectangle: sent to CustomRectItem to limit the size of the ROI to the pixmap size

	QPointF								m_startPos;				//!<First point when mouse clicked
	Qt::MouseButton						m_buttonPressed;		//!<The mouse button to be pressed (left clic or nobutton)
	bool								m_isMoving;				//!<Boolean if the user presses the CTRL key (true if pressed)
	QRectF								m_rectF;				//!<The old rectangle position before moving it

	bool								colorMove;				//!<send to ColorEdit the mouseRelease color or the mouseMove average color
};						  

#endif
