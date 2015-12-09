#ifndef CUSTOMRECTITEM_H
#define CUSTOMRECTITEM_H

#include <QGraphicsRectItem>
#include <QObject>

class Scene;

class CustomRectItem: public QObject, public QGraphicsRectItem
{
	Q_OBJECT

public:
	
	/**
	*	Constructor
	*/
	CustomRectItem(Scene * pParent = NULL);

	/**
	*	Destructor
	*/
	virtual ~CustomRectItem();

	/**
	 * @brief: Set the rectangle color
	 * @param color: The rectangle color
	*/
	void			setColor(const QColor & color);


	/**
	*	@brief: Redefinition of the boundingRect() method
	*/
	QRectF			boundingRect() const;

	/**
	*	@brief: Constrain in a rectangle
	*	@param rect: The constrained rectangle
	*/
	void			setConstrain(const QRectF & rect);

	/**
	*	@brief: get the rectangle in which this CustomRectItem is constrained
	*	@return QRectF: the rectangle
	*/
	QRectF			getConstrain();

	/**
	*	@brief: activates/deactivates the constraint of the rectangle
	*/
	void			setActivateConstrain(bool constrained);

protected:

	/**
	*	@brief: Return the area for a resize handle
	*	@param corner: The corner of the rectangle (0 is top left, 1 is top right, 2 is bottom left and 3 is bottom right)
	*/
	QRectF			rectFromArea(Qt::Corner corner) const;

	/**
	*	Get the corner of the node at a given position
	*	@param pos The point (in local coordinates) on which the mouse is
	*	@return The Qt::Corner (-1 if the mouse is not on a corner)
	*/
	int				areaFromPos(const QPointF & pos) const;

	/**
	*	@brief: Redefinition of the paint() method
	*/
	virtual void	paint(QPainter * painter, const QStyleOptionGraphicsItem * option, QWidget * widget = NULL);

	/**
	*	@brief: Redefinition of the shape() method
	*/
	QPainterPath	shape() const;



public:
	int				m_resizeMode;			//!<The corner currently being moved (-1 if no corner moves)
	Scene *			ptr_scene;				//!<The current scene pointer
	QRectF			m_constrained;			//!<The constrained rectangle
	bool			m_activateConstrained;	//!<Bool to activate the constrain or not
};

#endif
