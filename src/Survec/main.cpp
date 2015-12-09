#include <QDebug>
#include <QTextCodec>
#include <QProcessEnvironment>
#include "MainWindow.h"
#include <QTranslator>
#include <QSettings>
#include <QMessageBox>
#include <QLibraryInfo>

int main(int argc, char *argv[])
{
	int returnCode = 0;

	
	QTextCodec::setCodecForLocale(QTextCodec::codecForName("utf-8"));

	QApplication application(argc, argv);

	// gestion de la traduction
	QTranslator translator;
	QTranslator translatorQt;
	QTranslator translatorQtBase;
	QSettings settings("Survec", "");
	QString localization = settings.value("GUI/localization").toString();
	if (localization != "")
	{
		QString localFilename = QString("survec_%1").arg(localization);
		if (translator.load(localFilename))
			application.installTranslator(&translator);
		QString qtlocalFilename = QString("qt_%1").arg(localization);
		if (translatorQt.load(qtlocalFilename))
			application.installTranslator(&translatorQt);
		QString qtBaselocalFilename = QString("qtbase_%1.qm").arg(localization);
		QString path = QCoreApplication::applicationDirPath(); 
		if (translatorQtBase.load(qtBaselocalFilename, path))
			application.installTranslator(&translatorQtBase);
	}

	// gestion des donnees tesseract
	QString appTessDataDir = QString("%1/tessdata").arg(qApp->applicationDirPath());
	if (QDir(appTessDataDir).exists())
	{
		qputenv("TESSDATA_PREFIX", appTessDataDir.toLocal8Bit());
	}

	// test sur le nombre de Webcams disponibles
	VideoDeviceManager::getInstance();

	if (VideoDeviceManager::getInstance()->getNbDevice() == 0)
	{
		QMessageBox::warning(NULL, QObject::tr("No webcam found"), QObject::tr("No webcam found. Please connect your webcam and restart Survec..."));
		return 0;
	}

	// Launch main window 
	MainWindow mw(&settings);
	mw.show();

	returnCode = application.exec();
	return returnCode;
}
