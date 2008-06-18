#ifndef WATCHDOG_H_
#define WATCHDOG_H_

#include <QThread>
#include <QtCore>
#include "Config.h"

#define INTERVAL 1

/**
 * This class monitors a certain folder for new printjobs
 */
class WatchDog: public QObject
{
	Q_OBJECT
public:

	void run();
	WatchDog(Config *conf);
	virtual ~WatchDog();
	void stop();
	
signals:
	void fileFound(QString *file, bool wait);

public slots:
	void checkFolder(QString path, bool wait);
	void checkFolderSlot(QString path);
	void folderChanged(QString pat);
	
private:
	bool loop;
	QMap<QString, bool> *map;
	Config *conf;
	QFileSystemWatcher *watcher;
};

#endif /*WATCHDOG_H_*/
