#ifndef ROICONFIGURATIONWIDGET_H
#define ROICONFIGURATIONWIDGET_H

#include "ui_ROIConfigurationWidget.h"

// OpenCV
#include <opencv2/core/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>

// Qt
#include <QImage>
#include <QGraphicsScene>
#include <QtGui/QImage>
#include <QLabel>
#include <QTimer>
#include <QDialog>
#include <QGraphicsRectItem>

#include <ROI.h>

//class QKeyEvent;
class Scene;
class DetectionType;

class ROIConfigurationWidget: public QDialog
{
	Q_OBJECT

public:

	/**
	*	Constructor
	*/
	ROIConfigurationWidget(QWidget * pParent = 0, Qt::WindowFlags flags = 0);
	
	/**
	*	Destructor
	*/
	~ROIConfigurationWidget();

	/**
	*	@brief: initialize the widget. Reads the model values and display them
	*	@param Session* pSession: pointer to the current Session
	*/
	void							setROI(ROI* pROI, int roiNmber);

	/**
	*	@brief: get the GUI user's value and set them as the model
	*	@param ROI* p_roi: pointer to the ROI to update
	*/
	void							updateModel(ROI * p_roi);

	/**
	*	@brief: reads the values of the model (ROI * roi) and write them in the GUI field
	*/
	void							updateGUIFromModel();

	/**
	*	@brief: calls blockSignals of every GUI item
	*	@param bool value: boolean to block or not the signal
	*/
	void							blockAllSignals(bool value);

	/** 
	*	@brief: display the boundaries of the rectangle (x, y, width, height)
	*/
	void							displayBounds();

	/**
	*	@brief: inits the color of the color combobox
	*/
	void							initColorCombobox();
	
	/**
	*	@brief: return a pointer to the ROI
	*/
	ROI*							getROI() const;
	
	/**
	*	@brief: ask the user to confirm before closing
	*/
	void							closeEvent(QCloseEvent * event);

protected slots:

	/** Slot: on "OK" button clicked
	*	@brief: update the value of the field of detection when Ok button is clicked
	*/
	void							updateModelFromGUI();

	/** Slot: on DetectionType selection (by radioButton)
	*	@brief: enable or disable the type of detection that the user picked (GUI only)
	*	@param bool toggle: is not used but radioButton signal needs it
	*/
	void							chooseDetectionType(bool toggle);

	/** Slot: on spinbox valueChanged
	*	@brief: changes the x, y, width or height of the rectangle
	*	@param int value: is not used but spinBox signal need it
	*/
	void							setROIValues(int value);
	
	/** Slot: when the user changes the rectangle with the mouse (move/resize). Comes from CustomRectItem
	*	@brief: update the values of the GUI in the spinboxes
	*	@param int x, y, w, h: new values of the rectangle to put in the spinbox
	*/
	void							setROIGUIValues(int x, int y, int w, int h);

	/** Slot: on toolButton "Set" click
	*	@brief: update the symbol to detect with what is inside of the rectangle
	*/
	void							updateROI(int x, int y, int w, int h);

	/** Slot: on "newImageWebcam", a signal from cwVideoPlayer (sent every 40ms) when reading a new image from the webcam
	*	@brief: update the label displaying the ROI
	*/
	void							updateROI();

	/** Slot: when the user starts ROIConfigWidget or when he clicks on the "set symbol" button
	*	@brief: update the label displaying the symbol
	*/
	void							updateSymbol();

	/** Slot: when the user picks a color in the combobox (ColorDetection)
	*	@brief: set the current color to detect based on the combobox current index
	*/
	void							colorModified(int index);

	/** Slot: when the user changes the text in the textEdit (TextDetection)
	*	@brief: set the current text to detect in the model
	*/
	void							textModified(QString text);

	/** Slot: when the user changes the font of the text to detect
	*	@brief: set the current font to work with
	*/
	void							textFontModified(int);

	/** Slot: when the user changes the value of the number to detect
	*	@brief: set the current value to detect
	*/
	void							numberModified(double value);
	
	/** Slot: when the user changes the font of the number to detect
	*	@brief: set the current font to work with
	*/
	void							numberFontModified(int);

	/**
	*	@brief: set the filtration value
	*/
	void							setFiltration(int value);
	

	/** Slot: when the user change the tolerance slider
	*	@brief: changes the text label next to the slider to show percentage
	*/
	void							updateSliderPercentage(int);

	/** Slot: when the user changes the current webcam
	*	@brief: show the video feed of the chosen webcam
	*/
	void							webcamChanged(int);
protected:
	Ui::ROIConfigurationWidget		m_ui;								//!<ui
	ROI *							ptr_roi;							//!<pointer to current ROI
	Scene *							ptr_scene;							//!<graphics pointer scene
};

#endif

