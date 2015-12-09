#include "XML.h"
#include "ROI.h"
#include "Thread.h"
#include <QDebug>
#include <QTime>
#include <opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <QDomDocument>
#include <QFile>
#include <QFileInfo>
#include <QTextCodec>
#include <QMessageBox>

using namespace cv;

QString	XML::ERROR_WEBCAM_CONNEXION		= "Error : missing webcam - %1 detected and %2 needed by the session";
QString	XML::ERROR_FILE_FORMAT			= "Wrong XML format";
QString	XML::ERROR_FILE_LOADING			= "Error file loading";


XML::XML()
{
	qDebug() << "=== XML constructor ===";
}

XML::~XML()
{
}


void XML::readAlarmFile(QString fileName, QVector<Alarm> & alarmTab)
{
	qDebug() << "XML::readColorFile: " << fileName;
	//fileName = "../../" + fileName;
	QFile file(fileName);

	if (file.open(QIODevice::ReadWrite | QIODevice::Text) == true)
	{
		QDomDocument doc;
		if (doc.setContent(&file) == true)
		{
			QDomNodeList alarmList = doc.elementsByTagName("alarm");
			alarmTab.clear();
			if (!alarmList.isEmpty())
			{
				// ======= For each alarm =======
				for (int i = 0; i < alarmList.size(); i++)
				{
					QDomElement alarm = alarmList.item(i).toElement();
					if (!alarm.isNull())
					{
						Alarm alarm;

						QDomNodeList alarmValues = alarmList.item(i).childNodes();

						QDomElement date = alarmValues.item(0).toElement();
						QDomElement time = alarmValues.item(1).toElement();
						QDomElement duration = alarmValues.item(2).toElement();
						QDomElement logic = alarmValues.item(3).toElement();

						QString dateValue = date.attribute("value");
						QString timeValue = time.attribute("value");
						QString durationValue = duration.attribute("value");
						QString logicValue = logic.attribute("value");

						// ======= For both ROI =======
						for (int j = 0; j < 2; j++)
						{
							QDomNodeList roi = alarmValues.item(4+j).childNodes();

							QString type = roi.at(0).toElement().attribute("value");
							QString current = roi.at(1).toElement().attribute("value");
							QString target = roi.at(2).toElement().attribute("value");
							QString image = roi.at(3).toElement().attribute("value");
							QString alarmThrower = roi.at(4).toElement().attribute("value");

							// === Targets
							if (target.startsWith("inf"))
							{
								target.replace("inf", "<");
							}

							else if (target.startsWith("sup"))
							{
								target.replace("sup", ">");
							}
							else if (target.startsWith("equals"))
							{
								target.replace("equals", "=");
							}
							else if (target.startsWith("notequals"))
							{
								target.replace("notequals", "\342\211\240");
							}

							// === Background colors
							if (alarmThrower == "1")
								alarm.setBackgroundColor(j+1, "#dd8888");
							else if(alarmThrower == "0")
								alarm.setBackgroundColor(j+1, "#88dd88");

							alarm.setDate(dateValue);
							alarm.setDuration(durationValue);
							alarm.setStartTime(timeValue);
							alarm.setLogic(logicValue);
							alarm.setStatus("Completed");

							alarm.setCurrent(j+1, current);
							alarm.setTarget(j+1, target);
							alarm.setType(j+1, type);
							alarm.setImagePath(j + 1, image);
						}

						alarmTab.push_back(alarm);
					}
				}
			}
		}
	}
	emit changed();
}

void XML::writeAlarmFile(QString fileName, QVector<Alarm>& alarmTab)
{
	qDebug() << "XML::writeAlarmFile: " << fileName;
	//fileName = "../../" + fileName;
	QFile file(fileName);
	QDomDocument doc;

	// Fill XML doc
	QDomElement journal = doc.createElement("journal");

	for (int i = 0; i < alarmTab.size(); i++)
	{
		QDomElement alarm = doc.createElement("alarm");

		QDomElement date = doc.createElement("date");
		QDomElement time = doc.createElement("time");
		QDomElement duration = doc.createElement("duration");
		QDomElement logic = doc.createElement("operator");
		QDomElement roi = doc.createElement("roi");

		date.setAttribute("value", alarmTab.at(i).getDate());
		time.setAttribute("value", alarmTab.at(i).getStartTime());
		duration.setAttribute("value", alarmTab.at(i).getDuration());
		logic.setAttribute("value", alarmTab.at(i).getLogic());
		
		alarm.appendChild(date);
		alarm.appendChild(time);
		alarm.appendChild(duration);
		alarm.appendChild(logic);
		
		for (int j = 0; j < 2; j++)
		{
			QDomElement roi = doc.createElement("roi");

			QDomElement type = doc.createElement("type");
			QDomElement current = doc.createElement("current");
			QDomElement target = doc.createElement("target");
			QDomElement image = doc.createElement("image");
			QDomElement alarmThrower = doc.createElement("alarmThrow");

			// === Targets
			QString targetValue = alarmTab.at(i).getTarget(j + 1);
			if (targetValue.startsWith("<"))
			{
				targetValue.replace("<", "inf");
			}

			else if (targetValue.startsWith(">"))
			{
				targetValue.replace(">", "sup");
			}
			else if (targetValue.startsWith("="))
			{
				targetValue.replace("=", "equals");
			}
			else if (targetValue.startsWith("\342\211\240"))
			{
				targetValue.replace("\342\211\240", "notequals");
			}

			roi.setAttribute("value", j);
			type.setAttribute("value", alarmTab.at(i).getTypeROI(j+1));
			current.setAttribute("value", alarmTab.at(i).getCurrent(j+1));
			target.setAttribute("value", targetValue);
			image.setAttribute("value", alarmTab.at(i).getImagePath(j+1));

			roi.appendChild(type);
			roi.appendChild(current);
			roi.appendChild(target);
			roi.appendChild(image);

			// === Background colors (if background color is red then value == 1)
			if (alarmTab.at(i).getTarget(j + 1) != "")
			{
				alarmThrower.setAttribute("value", (alarmTab.at(i).getBackgroundColor(j + 1) == "#dd8888"));
				roi.appendChild(alarmThrower);
			}

			alarm.appendChild(roi);
		}
		journal.appendChild(alarm);

	}

	// Write XML to file
	doc.appendChild(journal);

	if (file.open(QIODevice::ReadWrite | QIODevice::Text | QFile::Truncate))
	{
		QTextStream out(&file);
		//out << doc.toString();
		out.setCodec("UTF-8");
		out << QString("<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?>\n");
		doc.save(out, 4);

		qDebug() << "XML file created";
	}
}

bool XML::readSessionFile(QString fileName, Session * p_session)
{
	QString errorMsg;
	qDebug() << "XML::readColorFile: " << fileName;
	bool ok = true;
	//fileName = "../../" + fileName;
	QFile file(fileName);

	if (file.open(QIODevice::ReadWrite | QIODevice::Text) == true)
	{
		QDomDocument doc;
		if (doc.setContent(&file) == true)
		{
			// ======= For each camera =======
			QDomNodeList cameraList = doc.elementsByTagName("camera");

			if (!cameraList.isEmpty())
			{
				if (VideoDeviceManager::getInstance()->getNbDevice() >= cameraList.size())		// checks if the number of detected webcam matches the number of webcam in the XML file
				{
					for (int i = 0; i < cameraList.size(); i++)
					{
						QDomElement camera = cameraList.item(i).toElement();
						if (!camera.isNull())
						{
							// ======= For each color =======
							QDomNodeList colorList = camera.elementsByTagName("color");

							qDebug() << "colorList.size() " << colorList.size();
							for (int j = 0; j < colorList.size(); j++)
							{
								QDomElement color = colorList.item(j).toElement();
								QString colorName = color.attribute("name");

								QDomNodeList hsv = colorList.item(j).childNodes();

								int hue = hsv.at(0).toElement().attribute("value").toInt();		// set HSV to target color
								int saturation = hsv.at(1).toElement().attribute("value").toInt();
								int value = hsv.at(2).toElement().attribute("value").toInt();

								VideoDeviceManager::getInstance()->getDevice(i)->getColorToDetect().setColor(colorName, hue, saturation, value);
							}
						}
					}
				}
				else	// if a webcam is missing
				{
					errorMsg = QString(ERROR_WEBCAM_CONNEXION).arg(VideoDeviceManager::getInstance()->getNbDevice()).arg(cameraList.size());
					ok = false;
				}
			}
			if (ok)		// keep reading XML file if no webcam is missing
			{
				QDomNodeList roiList = doc.elementsByTagName("roi");

				// ======= For each roi =======
				if (!roiList.isEmpty())
				{
					for (int i = 0; i < roiList.size(); i++)
					{
						QDomElement roi = roiList.item(i).toElement();

						if (!roi.isNull())
						{
							QDomNodeList roiValues = roiList.item(i).childNodes();
							QString roiName = roiValues.at(0).toElement().attribute("value");
							QString webcam = roiValues.at(1).toElement().attribute("value");
							p_session->getROI(i)->setVideoDeviceId(webcam.toInt());

							QDomNodeList geometry = roiValues.item(2).childNodes();
							int xmin = geometry.at(0).toElement().attribute("value").toInt();
							int ymin = geometry.at(1).toElement().attribute("value").toInt();
							int xmax = geometry.at(2).toElement().attribute("value").toInt();
							int ymax = geometry.at(3).toElement().attribute("value").toInt();

							QDomNodeList detection = roiValues.item(3).childNodes();
							QString type = detection.at(0).toElement().attribute("value");
							QString op = detection.at(1).toElement().attribute("value");
							QString target = detection.at(2).toElement().attribute("value");
							QString font = detection.at(3).toElement().attribute("value");
							QString tolerance = detection.at(4).toElement().attribute("value");
							QString sensitivity = detection.at(5).toElement().attribute("value");

							// In case of NUMBER_DT
							// operators are <, >, = and !=
							if (type == "number")
							{
								NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(p_session->getROI(i)->getDetectionType(ROI::NUMBER_DT));
								NumberDetection::operatorType typeop = NumberDetection::operatorType::INF;
								if (op == "inf")
								{
									op = "<";
									typeop = NumberDetection::operatorType::INF;
								}

								else if (op == "sup")
								{
									op = ">";
									typeop = NumberDetection::operatorType::SUP;
								}
								else if (op == "equals")
								{
									op = "=";
									typeop = NumberDetection::operatorType::EQUALS;
								}
								else if (op == "notequals")
								{
									op = "\342\211\240";
									typeop = NumberDetection::operatorType::NEQUALS;
								}
								else
								{
									op = "Error in operator type";
									typeop = NumberDetection::operatorType::INF;
								}
								ptrNumberDetection->setOperator(typeop);
							}
							// In case of detection different from number
							// operators are "is" (1) or "is not" (0) and op stays the same


							// Set the index of the color in model
							if (type == "color")
							{
								QString indexColor = detection.at(0).toElement().attribute("index");
								ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(p_session->getROI(i)->getDetectionType(ROI::COLOR_DT));
								ptrColorDetection->setIndexColor(indexColor.toInt());
							}

							// Set the symbol in model
							if (type == "symbol")
							{
								QString symbolPath = detection.at(2).toElement().attribute("path");
								p_session->setSymbolPath(i + 1, symbolPath);

								SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(p_session->getROI(i)->getDetectionType(ROI::SYMBOL_DT));
								if (QFileInfo(symbolPath).exists())
								{
									Mat temp = imread(symbolPath.toStdString());
									cvtColor(temp, temp, CV_BGR2RGB);
									ptrSymbolDetection->setSymbol(temp);
								}
							}

							p_session->getROI(i)->setCurrentDetectionTypeAsQString(type);
							p_session->getROI(i)->setBounds(xmin, ymin, xmax, ymax);
							p_session->getROI(i)->setTarget(target);
							p_session->getROI(i)->setFont(font);
							p_session->getROI(i)->setTolerance(tolerance.toInt());
							p_session->getROI(i)->setFiltration(sensitivity.toInt());

							p_session->getROI(i)->setOperator(op);
						}
					}
				}
				else
				{
					ok = false;
					errorMsg = ERROR_FILE_FORMAT;
				}


				// ======= For analysis =======
				QDomNodeList analysis = doc.elementsByTagName("analysis");
				if (!analysis.isEmpty())
				{
					QDomNodeList analysisValues = analysis.item(0).childNodes();

					// Analysis
					QDomNodeList actives = analysisValues.item(0).childNodes();

					QString roi1Active = actives.at(0).toElement().attribute("value");
					QString roi2Active = actives.at(1).toElement().attribute("value");

					// Operators
					QDomNodeList opValues = analysisValues.item(1).childNodes();
					QString typeOperator = opValues.at(0).toElement().attribute("value");

					// Journal
					QDomNodeList journalValues = analysisValues.item(2).childNodes();
					QString writeResult = journalValues.at(0).toElement().attribute("value");
					QString writeImages = journalValues.at(1).toElement().attribute("value");

					// alarm Interface
					QDomNodeList alarmInterface = analysisValues.item(3).childNodes();
					QString alarmPluginName = alarmInterface.at(0).toElement().attribute("value");

					// Video recording
					QDomNodeList videoRecording = analysisValues.item(4).childNodes();
					QString roi1 = videoRecording.at(0).toElement().attribute("value");
					QString roi2 = videoRecording.at(1).toElement().attribute("value");
					QString duration = videoRecording.at(2).toElement().attribute("value");

					// Convert QString (XML) to correct type (GUI)
					p_session->getROI(0)->setActive((roi1Active == "1"));
					p_session->getROI(1)->setActive((roi2Active == "1"));

					// Logic
					p_session->setLogic((typeOperator == "AND"));

					// Write result/images
					p_session->setWriteImages((writeImages == "1"));
					p_session->setWriteResults((writeResult == "1"));

					// Video recording
					p_session->setRecordROI1((roi1 == "1"));
					p_session->setRecordROI2((roi2 == "1"));
					p_session->setRecordDuration(QTime::fromString(duration));

					// alarm Interface
					p_session->setAlarmPluginName(alarmPluginName);

				}
				else
				{
					ok = false;
					errorMsg = ERROR_FILE_FORMAT;
				}

			}
		}
		else
		{
			ok = false;
			errorMsg = ERROR_FILE_FORMAT;
		}
	}
	else
	{
		ok = false;
		errorMsg = ERROR_FILE_FORMAT;
	}
	if (!ok)
	{
		QMessageBox::warning(NULL, ERROR_FILE_LOADING, errorMsg);
	}

	emit changed();
	return ok;
}


void XML::writeSessionFile(QString fileName, Session * p_session)
{
	qDebug() << "XML::writeSessionFile: " << fileName;
	//fileName = "../../" + fileName;
	QFile file(fileName);
	QDomDocument doc;

	// Fill XML doc
	QDomElement sessionNode = doc.createElement("session");
	QDomElement version = doc.createElement("version");
	version.setAttribute("value", 0);
	sessionNode.appendChild(version);

	// ====== Colors ======
	QDomElement colorimetry = doc.createElement("colorimetry");

	if (VideoDeviceManager::getInstance()->getNbDevice() > 0)
	{
		for (int i = 0; i < VideoDeviceManager::getInstance()->getNbDevice(); i++)
		{
			ColorToDetect cTD = VideoDeviceManager::getInstance()->getDevice(i)->getColorToDetect();

			QDomElement camera = doc.createElement("camera");
			camera.setAttribute("id", i);

			QDomElement name = doc.createElement("name");
			name.setAttribute("value", "logitech-c920");
			camera.appendChild(name);

			for (int j = 0; j < 10; j++)
			{
				QDomElement color = doc.createElement("color");
				color.setAttribute("name", cTD.getIndexColorName(j));

				QDomElement hue = doc.createElement("hue");
				QDomElement saturation = doc.createElement("saturation");
				QDomElement value = doc.createElement("value");

				hue.setAttribute("value", cTD.getIndexColor(j).hue());
				saturation.setAttribute("value", cTD.getIndexColor(j).saturation());
				value.setAttribute("value", cTD.getIndexColor(j).value());

				color.appendChild(hue);
				color.appendChild(saturation);
				color.appendChild(value);

				camera.appendChild(color);
			}
			colorimetry.appendChild(camera);
		}
		sessionNode.appendChild(colorimetry);

		// ====== ROIs ======
		for (int i = 0; i < p_session->getNumberOfROI(); i++)
		{
			QDomElement roi = doc.createElement("roi");

			// Name
			QDomElement name = doc.createElement("name");
			name.setAttribute("value", i + 1);
			roi.appendChild(name);

			// Webcam
			QDomElement webcam = doc.createElement("webcam");
			webcam.setAttribute("value", p_session->getROI(i)->getVideoDeviceId());
			roi.appendChild(webcam);

			// Bounds of ROI
			QDomElement geometry = doc.createElement("geometry");
			int bounds[4];

			p_session->getROI(i)->getBounds(bounds);
			QDomElement xmin = doc.createElement("xMin");
			QDomElement ymin = doc.createElement("yMin");
			QDomElement xmax = doc.createElement("xMax");
			QDomElement ymax = doc.createElement("yMax");
			xmin.setAttribute("value", bounds[0]);
			ymin.setAttribute("value", bounds[1]);
			xmax.setAttribute("value", bounds[2]);
			ymax.setAttribute("value", bounds[3]);
			geometry.appendChild(xmin);
			geometry.appendChild(ymin);
			geometry.appendChild(xmax);
			geometry.appendChild(ymax);
		
			roi.appendChild(geometry);

			// Detection type
			QDomElement detection	= doc.createElement("detection");
			QDomElement type		= doc.createElement("type");
			QDomElement op			= doc.createElement("operator");
			QDomElement target		= doc.createElement("target");
			QDomElement font		= doc.createElement("font");
			QDomElement tolerance	= doc.createElement("tolerance");
			QDomElement sensitivity = doc.createElement("sensitivity");

			QString detectionType = "";
			QString targetValue = "";
			QString fontValue = "";
			QString opValue = "";
			QString toleranceValue = "";
			QString sensitivityValue = "";

			TextDetection* ptrTextDetection = dynamic_cast<TextDetection*>(p_session->getROI(i)->getDetectionType(ROI::TEXT_DT));
			NumberDetection* ptrNumberDetection = dynamic_cast<NumberDetection*>(p_session->getROI(i)->getDetectionType(ROI::NUMBER_DT));
			ColorDetection* ptrColorDetection = dynamic_cast<ColorDetection*>(p_session->getROI(i)->getDetectionType(ROI::COLOR_DT));
			SymbolDetection* ptrSymbolDetection = dynamic_cast<SymbolDetection*>(p_session->getROI(i)->getDetectionType(ROI::SYMBOL_DT));

			// Get detection type, operator, target and font
			Mat img = imread(p_session->getSymbolPath(i + 1).toStdString());
			switch (p_session->getROI(i)->getCurrentDetectionType())
			{
			case 0:
				detectionType = "undefined";
				break;
			case 1:
				detectionType = "color";
				opValue = QString::number(p_session->getROI(i)->getAlarmOnEquals());
				type.setAttribute("index", ptrColorDetection->getIndexColor());
				targetValue = p_session->getROI(i)->getTarget();
				break;
			case 2:
				detectionType = "symbol";
				opValue = QString::number(p_session->getROI(i)->getAlarmOnEquals());
				targetValue = "link";
				target.setAttribute("path", p_session->getSymbolPath(i + 1));
				if (!img.empty())
				{
					cvtColor(img, img, CV_BGR2RGB);
					ptrSymbolDetection->setSymbol(img);	// set symbol from image path
				}
				break;
			case 3:
				detectionType = "text";
				fontValue = ptrTextDetection->getFontAsQString();
				opValue = QString::number(p_session->getROI(i)->getAlarmOnEquals());
				targetValue = p_session->getROI(i)->getTarget();
				break;
			default:
				detectionType = "number";
				fontValue = ptrNumberDetection->getFontAsQString();
				opValue = ptrNumberDetection->getOperatorXML();
				targetValue = QString::number(ptrNumberDetection->getTarget());
				break;
			}
			toleranceValue = QString::number(p_session->getROI(i)->getTolerance());
			sensitivityValue = QString::number(p_session->getROI(i)->getFiltration());

			type.setAttribute("value", detectionType);
			op.setAttribute("value", opValue);
			qDebug() << "writeSession::XML : targetValue : " << targetValue.toUtf8();
			target.setAttribute("value", targetValue);
			font.setAttribute("value", fontValue);
			tolerance.setAttribute("value", toleranceValue);
			sensitivity.setAttribute("value", sensitivityValue);

			detection.appendChild(type);
			detection.appendChild(op);
			detection.appendChild(target);
			detection.appendChild(font);
			detection.appendChild(tolerance);
			detection.appendChild(sensitivity);

			roi.appendChild(detection);

			sessionNode.appendChild(roi);
		}

	
		// ====== Analysis ======
		QDomElement analysis = doc.createElement("analysis");

		// Actives
		QDomElement actives = doc.createElement("actives");
		int temp = 0;
		for (int j = 0; j < p_session->getNumberOfROI(); j++)
		{
			QDomElement roi = doc.createElement("roiActive");
			roi.setAttribute("value", p_session->getROI(j)->getActive());
			temp++;
			actives.appendChild(roi);
		}
		actives.setAttribute("value", temp);
		analysis.appendChild(actives);

		// Operators
		QDomElement op = doc.createElement("operators");
		QDomElement type = doc.createElement("type");
		type.setAttribute("value", p_session->getLogic());
		op.appendChild(type);
		analysis.appendChild(op);

		// Journal
		QDomElement journal = doc.createElement("journal");
		QDomElement writeResult = doc.createElement("writeResult");
		QDomElement writeImages = doc.createElement("writeImages");

		writeResult.setAttribute("value", p_session->getWriteResults());
		writeImages.setAttribute("value", p_session->getWriteImages());

		journal.appendChild(writeResult);
		journal.appendChild(writeImages);
		analysis.appendChild(journal);

		// alarm Interface
		QDomElement alarmInterface = doc.createElement("alarmInterface");
		QDomElement typeOp = doc.createElement("type");

		typeOp.setAttribute("value", p_session->getAlarmPluginName());

		alarmInterface.appendChild(typeOp);

	
		analysis.appendChild(alarmInterface);

		// Video recording
		QDomElement videoRecording = doc.createElement("videoRecording");
		QDomElement roi1 = doc.createElement("roi1");
		QDomElement roi2 = doc.createElement("roi2");
		QDomElement duration = doc.createElement("duration");
		roi1.setAttribute("value", p_session->getRecordROI1());
		roi2.setAttribute("value", p_session->getRecordROI2());
		duration.setAttribute("value", p_session->getRecordDuration().toString());
		videoRecording.appendChild(roi1);
		videoRecording.appendChild(roi2);
		videoRecording.appendChild(duration);

		analysis.appendChild(videoRecording);

		sessionNode.appendChild(analysis);

		// Write XML to file
		doc.appendChild(sessionNode);
		if (file.open(QIODevice::ReadWrite | QIODevice::Text | QFile::Truncate))
		{
			QTextStream out(&file);
			out.setCodec("UTF-8");
			out << QString("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n");
			doc.save(out, 4);
		}

	}
}