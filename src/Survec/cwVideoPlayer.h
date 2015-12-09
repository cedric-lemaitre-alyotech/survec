#ifndef CWVIDEOPLAYER_H
#define CWVIDEOPLAYER_H

#include "ui_cwVideoPlayer.h"

#include <QObject>
#include <VideoDeviceManager.h>
#include <QTimer>
#include <cwView.h>

//class cwView;

class cwVideoPlayer: public QWidget
{
	Q_OBJECT

public:
	/**
	*  Constructor
	*/
	cwVideoPlayer(QWidget * pParent = 0, Qt::WindowFlags flags = 0);

	/**
	*  Destructor
	*/
	~cwVideoPlayer();

	/**
	*	@brief: returns true if the video is in pause, false otherwise
	*	@return: a bool
	*/
	bool				isStopped();

	/**
	*	@brief: set the isPause value to 'value'
	*	@param bool value: the value isPause is taking
	*/
	void				pause(bool value);

	/**
	*	@brief: empty //TODO
	*/
	void				setROIInfo(int device /* , rectangle*/);

	/**
	*	@brief: get the cwView inside this cwVideoPlayer
	*	@return: a pointer to cwView
	*/
	cwView *			getCwView();

	/**
	*	@brief: set the pixmap of the videoPlayer
	*/
	//void				setPixmap(QPixmap * pixmap);

	/**
	*	@brief: set the pixmapRes of the videoPlayer
	*	@param QPixmap &pixmap: the result pixmap that will be displayed
	*	@param int x, y, w, h: the boundaries of the pixmap to display
	*/
	void				setPixmapRes(QPixmap & pixmap, int x, int y, int w, int h);

	/**
	*	@brief: get the pixmap of the videoPlayer
	*/
	//QPixmap *			getPixmap();

	/**
	*	@brief: set the boundaries of the ROI's rectangle
	*	@param int x, y, w, h: the boundaries of the rectangle
	*/
	void				setBounds(int x, int y, int w, int h);

	/**
	*	@brief: set the visibility of the widget containing the play/pause button
	*/
	void				setButtonVisible(bool visible);

	/**
	*	@brief: calls fitInView
	*/
	void				showEvent(QShowEvent * event);

	/**
	*	@brief: set the ID of the webcam to display (VideoDevice)
	*/
	void				setFeedId(int id);

	/**
	*	@brief: read the webcam from the deviceId
	*/
	void				readWebcamFeed();

	/**
	*	@brief: set the device id
	*/
	void				setDeviceId(int id);

	/**
	*	@brief: get the device id
	*/
	int					getDeviceId();

	void				detachVideoDevice();

public slots:

	/** Slot: on timer's tick
	*	@brief: if !isPause, refresh the video
	*/
	void				updateGUI();

	/** Slot: on click on the play/pause button at the bottom of the cwView
	*	@brief: isPause: change the icon to "pause" , starts the timer and start updateGUI
	*			!isPause: change the icon to "play" and stop updateGUI
	*/
	void				playPause();

	/** Slot: on click on the fitView button at the bottom of the cwView
	*	@brief: calls cwView fitInView method
	*/
	void				fitView();

	void				delayedFitView();

	/**
	*	@brief: allows the read the full pixmap or just the content of the ROI's rectangle
	*/
	void				setReadFullPixmap(bool value);
	
signals:

	/**
	*	@brief: a new image has been read
	*/
	void				newImageWebcam();

	/**
	*	@brief: the webcam has been changed
	*/
	void				webcamChanged();

protected:
	Ui::cwVideoPlayer	m_ui;						//!<ui
	Mat					m_image;					//!<image to display as a Mat
	int					m_deviceId;					//!<device id of the webcam to display
	bool				isPause;					//!<boolean to tell if cwVideoPlayer is in pause or playing
	QPixmap	*			p_pixmap;					//!<image init to display
	QPixmap				m_pixmapRes;				//!<image result to display
	bool				readFullPixmap;				//!<display the full pixmap of the scene or not
	int					bounds[4];					//!<boundaries of the rectangle: topleft corner (x, y), width (w) and height (h)
	VideoDevice*		p_videoDevice;
	QTimer				m_fitDelayTimer;
};

#endif // CWVIDEOPLAYER_H
// @}
