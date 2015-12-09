#ifndef ALARMPLUGINMANAGER_H
#define ALARMPLUGINMANAGER_H

#include "Survec_dll.h"
#include <QList>

#include "AlarmPluginInterface.h"

/* Classe singleton donnant acces aux plugins disponibles
*/
class SURVECLIB_EXPORT AlarmPluginManager: public QObject
{
	Q_OBJECT

protected:
	static AlarmPluginManager* p_instance;

	/**
	*	Constructor
	*/
	AlarmPluginManager();

	/**
	*	Destructor
	*/
	~AlarmPluginManager();
	
public:
	/**
	 * Retourne l'instance unique de la classe
	 */
	static AlarmPluginManager* getInstance();

	/**
	 * Destruction de l'instance unique de la classe
	 */
	static void destroyInstance();


	void loadPlugins();

	/**
	 * Retourne la liste des plugins de traitement disponibles
	 */
	const QList<AlarmPluginInterface*>& getPlugins() const;

	AlarmPluginInterface* findPlugin(const QString& name) const;

private:
	QList<AlarmPluginInterface*> m_processPlugins;
};

#endif
