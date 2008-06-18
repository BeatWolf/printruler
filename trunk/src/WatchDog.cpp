#include "WatchDog.h"

WatchDog::WatchDog(Config *config)
{
	loop = true;
	map = new QMap<QString, bool>();
	conf = config;
	
}

WatchDog::~WatchDog()
{
	delete map;
	delete watcher;
}

void WatchDog::stop(){
	loop = false;
}

void  WatchDog::checkFolderSlot(QString path){
	checkFolder(path, true);
}

void  WatchDog::checkFolder(QString path, bool wait = true){
	QStringList filters;
		
    filters << "*.ps" << "*.pdf";
	QDir dir(path);
	dir.setNameFilters(filters);
	QStringList list = dir.entryList();
	
	for(int i = 0; i < list.size(); i++){
		
    	if(!map->contains(list.at(i))){
    		map->insert(list.at(i),true);
    		
    		QString *tmp = new QString(dir.path());
    		tmp->append("/");
    		tmp->append(list.at(i));
    		qDebug() << "found file" << *tmp;
    		emit fileFound(tmp, wait);
    	}
	}
}

void WatchDog::folderChanged(QString folder){
	delete watcher;
	watcher = new QFileSystemWatcher();
	watcher->addPath(folder);
	connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(checkFolder(QString)));
}

void WatchDog::run(){
	checkFolder(*conf->getWatchFolder(), false);
		
	watcher = new QFileSystemWatcher();
	watcher->addPath(*conf->getWatchFolder());
	connect(watcher, SIGNAL(directoryChanged(QString)), this, SLOT(checkFolderSlot(QString)));

}

