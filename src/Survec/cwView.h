#ifndef CWVIEW_H
#define CWVIEW_H

#include <QGraphicsView>
#include <Scene.h>

class Scene;

class cwView: public QGraphicsView
{
	Q_OBJECT


public:
	
	/**
	*	Constructor
	*/
	cwView(QWidget * pParent = NULL);

	/**
	*	Destructor
	*/
	virtual ~cwView();

	//void	setSelection(double xpos, double ypos, double w, double h); // mettra l'info dans Scene


	/**
	*	@brief: set the image to the scene
	*	@param QPixmap * image: pointer to the QPixmap to display
	*/
	void					setImage(QPixmap * image);

	/**
	*	@brief: getters of the QGraphicsScene Scene
	*
	*/
	Scene *					getScene();

	/**
	*	@brief: allows different kind of clic depending on the mode 
	*		ColorEditWidget get the pixel color
	*		ROIConfigurationWidget sets the ROI's rectangle
	*	@param: int mode: 0 is ColorEdit, 1 is ROIConfig
	*/
	void					setModeClick(int mode);

	void showEvent(QShowEvent *);

protected:
	/**
	*	@brief: wheelevent to zoom/dezoom in/out of the scene
	*/
	virtual void			wheelEvent(QWheelEvent * pWheelEvent);


public:
	Scene *					p_scene;	//!< pointer to Scene which possess everything to display the video and the ROI (rectangle)	
};

#endif
