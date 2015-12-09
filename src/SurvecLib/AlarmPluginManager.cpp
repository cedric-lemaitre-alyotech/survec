#include "AlarmPluginManager.h"
#include <QDebug>
#include <QApplication>
#include <QPluginLoader>
#include <QDir>
#include <QFileInfoList>

using namespace cv;

AlarmPluginManager* AlarmPluginManager::p_instance = NULL;

AlarmPluginManager::AlarmPluginManager()
{	
	qDebug() << ">CONSTRUCTOR: AlarmPluginManager";
}

AlarmPluginManager::~AlarmPluginManager()
{
}

AlarmPluginManager* AlarmPluginManager::getInstance()
{
	if (p_instance == NULL)
	{
		p_instance = new AlarmPluginManager();
	}
	return p_instance;
}

void AlarmPluginManager::destroyInstance()
{
	delete p_instance;
	p_instance = NULL;
}

void AlarmPluginManager::loadPlugins()
{
	QPluginLoader loader;
	QObject * pPlugin;
	QDir pluginsDir;
	QStringList filtres;
	QFileInfoList files;
	QFileInfoList::const_iterator itFiles;

	//on decharge les plugins charges
	//TODO destroy();

	//parcours des fichiers du repertoire "plugins"
	pluginsDir = QDir(QFileInfo(qApp->applicationFilePath()).absoluteDir().path() + "/plugins");
	qDebug() << "plugins dir=" << pluginsDir.absolutePath();
	QDir::setCurrent(pluginsDir.absolutePath());
	filtres << "*.dll";
	files = pluginsDir.entryInfoList(filtres,QDir::Files);
	for (itFiles=files.begin(); itFiles!=files.end(); ++itFiles)
	{
		QString filePath = itFiles->fileName();
		//tentative de chargement du fichier
		loader.setFileName(filePath);
		qDebug() << "plugins filePath=" << filePath;
		if (loader.load())
		{
			pPlugin = loader.instance();

			//plugin traitement ?
			if (qobject_cast<AlarmPluginInterface*>(pPlugin)!=NULL)
			{
				m_processPlugins.push_back(qobject_cast<AlarmPluginInterface*>(pPlugin));
			}
		}
	}

	if (m_processPlugins.count() == 0)
	{
		qWarning() << QString("No plugins loaded in %1").arg(pluginsDir.path());
	}
}

const QList<AlarmPluginInterface*>& AlarmPluginManager::getPlugins() const
{
	return m_processPlugins;
}

AlarmPluginInterface* AlarmPluginManager::findPlugin(const QString& name) const
{
	foreach(AlarmPluginInterface* plugin, m_processPlugins)
	{
		if (plugin->name() == name)
			return plugin;
	}
	return NULL;
}