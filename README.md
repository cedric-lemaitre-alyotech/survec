# survec
<img src="https://github.com/cedric-lemaitre-alyotech/survec/blob/master/Icon.png" height="60" width="60" />
A tool for monitoring screen(s) from webcams !<br />
SURVEC is made with QT5, OpenCV and Tesseract/Leptonica.

<b>Objectives:</b> <br/>
SURVEC is a tool wich enables to use webcams (1 or 2) for monitoring basically a screen but also everything coming from webcam pictures.
The software enables to trigger alarms when a change is detected in a Region Of Interest (ROI) in the webcam video stream.
The detection could be defined on 4 differents types:
- Color: the average color of the ROI is extracted and compared to a target color
- Text: the text of the ROI is readed with an OCR technology (Tesseract) and compared to the target text
- Numeric: works like the text but with a numerical conversion and a comparison to a target value (comparison could be equals, greater than or lower than)
- Symbol: the symbol is a picture recorded from an initial ROI. The comparison works with a comparison image to image with sturctual similarity process (SSIM).

<b>Dependances</b> <br/>
SURVEC needs others libraries for working:
- OpenCV2.4.10 : used for image processing 
- Tesseract3.02 and Leptonica1.71 : used for OCR processing
- QT5 : used for GUI

<b>Licences</b><br/>
SURVEC licences is LGPL v2.
