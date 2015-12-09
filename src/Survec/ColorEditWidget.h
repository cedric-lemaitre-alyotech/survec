#ifndef COLOREDITWIDGET_H
#define COLOREDITWIDGET_H

#include "ui_ColorEditWidget.h"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

// Qt
#include <qimage.h>
#include <QGraphicsScene>
#include <QtGui/QImage>
#include <qlabel.h>
#include <qtimer.h>
#include <QDialog>

// Class
#include <Session.h>
#include <VideoDevice.h>
#include <cwVideoPlayer.h>
#include <Scene.h>

class ColorEditWidget: public QDialog
{
	Q_OBJECT


public:

	/**
	 * Constructor
	 */
	ColorEditWidget(QWidget * pParent = 0, Qt::WindowFlags flags = 0);
	
	/**
	 * Destructor
	 */
	~ColorEditWidget();
	
	// temp method
	void displayCurrentColors();

	/**
	*	@brief: links the ColorEditWidget to a Session object
	*	@param Session * pSession: pointer towards the Session
	*/
	void							setSession(Session* pSession); 
	
	/**
	*	@brief: saves all the current color as a ColorToDetect
	*/
	void							saveCurrentColor();


	/**
	*	@brief: reads the saved ColorToDetect colors and display them in the pushbutton (real colors)
	*/
	void							initCTD();
	
	/**
	*	@brief: update the previsualisation color
	*/
	void							updatePrevis(int r, int g, int b);

	/**
	*	@brief: sets the real color from either the ColorToDetect (past/xml colors) or from the picking (previsualisation)
	*/
	void							setRealPickingColors();

public slots:

	/**
	*	@brief: enables or disables the colorPicking toolbutton in the GUI
	*/
	void							enableAllPipette(bool value);

	/**	Slot: on click on the scene (gets a pixel color)
	*	@brief: update the previsualisation color label
	*	@param r,g,b: change the color of the previsualisation label
	*/
	void							updateCurrent(int, int, int);

	
	/** Slot: on click on a resetButton
	*	@brief: reset the realButtonColor to its predefined color
	*		called when realColor button is clicked
	*/
	void							resetRealButton();
	
	/** Slot: on colorPicker toolbutton clicked
	*	brief: enables/disables all the other colorPicker toolbuttons
	*			changes the color of the realColor pushButton to the currentColor
	*			and updates the currentCOLOR (ie currentRed if colorPickerRed is clicked) 
	*			with the currentColor (the one in the previsualisation label)
	*/
	void							pipette();

	/** Slot: on combobox 'webcam' index changed
	*	@brief: changes the view
	*/
	void							webcamChanged(int);

	/** Slot: on okButton clicked
	*	@brief: update all the currentCOLOR to those visible in realColor pushButtons
	*/
	void							okButton();

	/** Slot: on cancelButton clicked
	*	@brief: quits the dialog
	*/
	void							cancelButton();

	/** Slot: on drawing a rectangle in the view
	*	@brief: extracts a rectangle from the view and calculates in average color
	*	@brief int x, y, w, h: the topLeft corner (x, y), width (w) and height (h)
	*/
	void							extractAvgColor(int, int, int, int);

	/**
	*	@brief: update the color the user wants to change by the current color (previsualisation color)
	*/
	void							setCurrent();

	/**
	*	@brief: sets if the user made a rectangle to chose a color (average) or simply clicked on a pixel (value of pixel) to measure the current color
	*/
	void							setAvg(bool value);
	
protected:

	Ui::ColorEditWidget				m_ui;				//!<ui
	Session *						ptr_session;		//!<pointer to Session

	bool							updateColor;		//!<update the realColor after the user clicked on the scene to pick a new currentColor
	bool							isAllEnabled;		//!<boolean to enable or disable the colorPicking toolbutton
	bool							avg;

	QColor							currentRed;			//!<currentRed
	QColor							currentOrange;		//!<currentOrange
	QColor							currentYellow;		//!<currentYellow
	QColor							currentGreen;		//!<currentGreen
	QColor							currentCyan;		//!<currentCyan
	QColor							currentBlue;		//!<currentBlue
	QColor							currentPink;		//!<currentPink
	QColor							currentWhite;		//!<currentWhite
	QColor							currentGrey;		//!<currentGrey
	QColor							currentBlack;		//!<currentBlack
	
	QColor							currentColor;		//!<currentColor being picked
	
	int								m_deviceId;			//!<id of the displayed device
};

#endif
