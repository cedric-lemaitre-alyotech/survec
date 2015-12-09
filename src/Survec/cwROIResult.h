#ifndef CWROIRESULT_H
#define CWROIRESULT_H

#include "ui_cwROIResult.h"

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

#include <ROI.h>

class cwROIResult: public QWidget
{
	Q_OBJECT

public:

	/**
	*	Constructor
	*/
	cwROIResult(QWidget * pParent = 0, Qt::WindowFlags flags = 0);
	
	/**
	*	Destructor
	*/
	~cwROIResult();

	/**
	*	@brief: sets the ROI to update
	*	called from MainWindow::updateROIResult
	*/
	void					setROI(ROI * p_roi, int roiNumber);

	/**
	*	@brief: calls setActiveDETECTION_TYPE depending on the current ROI detection
	*/
	void					updateGUI();

	/**
	*	@brief: set the pixmap of the alarm leds
	*/
	void					updateAlarm();
	
	/**
	*	@brief: return the groupbox
	*/
	QGroupBox *				getGroupBox();

	/**
	*	@brief:	return the current value
	*/
	QLabel *				getValueCurrent();

	/**
	*	@brief: return the label of currente value
	*/
	QLabel *				getLabelCurrent();

	/**
	*	@brief: return the push button to ROIConfigurationWidget
	*/
	QPushButton *			getROIConfigPushButton();

	/**
	*	@brief: return the alarm led label
	*/
	QLabel *				getAlarmLabel();

	/**
	*	@brief: return the operator label
	*/
	QLabel *				getLabelOperator();

	/**
	*	@brief: return the test pushbutton
	*/
	QPushButton *			getPbTest();

	/**
	*	@brief: return the screenshot pushbutton
	*/
	QPushButton *			getPbScreenshot();

	/**
	*	@brief: set the isTest bool
	*/
	void					setIsTest(bool val);

protected:

	/**
	*	@brief: reads from ROI and set the color value to detect
	*/
	void					setActiveColorDetection();

	/**
	*	@brief: reads from ROI and set the text value to detect
	*/
	void					setActiveTextDetection();

	/**
	*	@brief: reads from ROI and set the number value to detect
	*/
	void					setActiveNumberDetection();

	/**
	*	@brief: reads from ROI and set the symbol image to detect
	*/
	void					setActiveSymbolDetection();

	/**
	*	@brief: reads from ROI and display "Detection type: undefined"
	*/
	void					setActiveUndefinedDetection();



	/**
	*	@brief: toggle the visibility of labels (target, current, etc.)
	*/
	void					setLabelVisibility(bool visible);

	/**
	*	@brief: sets the shape/frame of valueCurrent and valueTarget (raised/box or noframe/plain)
	*/
	void					setFrameRaised(bool raise);

	/**
	*	@brief: blocks all signals
	*/
	void					blockAllSignals(bool value);


	protected slots:

	/**
	*	@brief				ROI activation/desactivation modified 
	*/
	void					activeROIModified(bool active);

	/**
	*	@brief				action opening the ROIConfigurationWidget dialog
	*/
	void					editROIConfig();

	/**
	*	@brief:				test the current config with only the current frame
	*/
	void					testConfig();

	/**
	*	@brief:				take a screenshot of the video
	*/
	void					screenshot();

	/**
	*	@brief:				set to debug (display info during process)
	*/
	void					setDebug(bool val);

signals:
	/** 
	*	@brief:				sends a signal to change the state of the ROI (in MainWindow), allowing to check/uncheck ROIs (and Start process button)
	*/
	void					roiStateChanged();

	/**
	*	@brief: pauses the video of MainWindow while ROIConfigurationWidget is open
	*/
	void					stopPlaying(bool);

	/**
	*	@brief: changes the webcam to be displayed
	*/
	void					webcamChanged();

	/**
	*	@brief: when user clicks on "Test!", show the alarm
	*/
	void					alarmChanged();

	/**
	*	@brief: when the user clicks on "Test!", update the currently detected value
	*/
	void					changed();

protected:
	Ui::cwROIResult			m_ui;					//!<ui
	int						m_roiNumber;
	ROI *					ptr_roi;				//!<pointer to current ROI
	bool					m_disabled;				//!<display the background color
	bool					m_isTest;				//!<is this updating because of test or because of detection
};

#endif

