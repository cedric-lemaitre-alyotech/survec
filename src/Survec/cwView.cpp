#include "cwView.h"
#include <cmath>
#include <QWheelEvent>
#include <QGraphicsSceneEvent>
#include <QGraphicsPixmapItem>
#include <QDebug>
#include <VideoDeviceManager.h>
#include <QDebug>

#define RATIO 1.25

cwView::cwView(QWidget *pParent): QGraphicsView(pParent)
{
	qDebug() << "=== View ===";
	setMouseTracking(true);
	p_scene = new Scene();
	setScene(p_scene);	
}

cwView::~cwView()
{
	qDebug() << "cwView::~cwView()";
	p_scene->clear();
	delete p_scene;
	p_scene = NULL;
}

void cwView::wheelEvent(QWheelEvent * pWheelEvent)
{
	
	// Ratio calculation for the zoom
	int nbSteps = floor(pWheelEvent->delta() / 8.0 / 15.0 + 0.5);
	double ratio = nbSteps > 0 ? RATIO * nbSteps: 1.0 / RATIO * -nbSteps;

	// Zoom
	scale(ratio, ratio);

	// Do not propagate the event
	pWheelEvent->accept();
	update();
	
}


void cwView::setModeClick(int mode)
{
	qDebug() << "cwView: setModeClick: " << mode;
	switch (mode)
	{
		// 0 is ColorEditWidget
		case 0:
			p_scene->setEnableColor(true);
			setCursor(Qt::CrossCursor);
			break;

		// 1 is ROIConfigurationWidget
		case 1:
			p_scene->setEnableColor(false);
			setCursor(Qt::ArrowCursor);
			break;
		default:
			break;
	}
}

void cwView::setImage(QPixmap * image)
{
	p_scene->setItemPixmap(image);
}

Scene * cwView::getScene()
{
	return p_scene;
}

void cwView::showEvent(QShowEvent *) 
{
	qDebug() << "SHOWEVENT !!";
	fitInView(p_scene->sceneRect(), Qt::KeepAspectRatio);
}