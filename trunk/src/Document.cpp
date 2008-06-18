#include <QtGui>

#include "Document.h"
#include "Config.h"
 
#include <unistd.h>

Document::Document(QString *documentfile, bool wait = true)
{
	qDebug() << "create file" << *documentfile;
	int i = 0;
	do{
		//HACK: try to do this with QTimer::singleshot
		if(wait){
			sleep(5); //artificial sleep to prevent reading a file that isnt finished yet
		}
		renderFile(documentfile);
		i++;
	}while(getSize(true) < 1 && i < 5);
}

void Document::renderFile(QString *documentfile){
	QFileInfo infoFile(*documentfile);
	file = *documentfile;
	
	Config *conf = Config::instance();
	QDir::current().mkpath(*conf->getTempFolder());
	QDir tmp(*conf->getTempFolder());
	baseFile = "";
	
	qDebug() << "convert pdf to ps";
	if(infoFile.suffix().compare("pdf") == 0){
		baseFile = *documentfile;
		QStringList argumentsPdf;
		file = *conf->getTempFolder();
		file.append(infoFile.fileName());
		file.append(".ps");
		/*argumentsPdf << "-dRotatePages=true";
		argumentsPdf << "-dSAFER" << "-dNOPAUSE" << "-dBATCH";
		argumentsPdf << "-sOutputFile="+file;
		argumentsPdf << "-sDEVICE=pswrite";
		argumentsPdf << *documentfile;
		qDebug() << argumentsPdf;*/
		argumentsPdf << *documentfile << file;
		QProcess *myProcess = new QProcess(this);
		myProcess->start("pdftops", argumentsPdf);
		myProcess->waitForFinished(-1);
		delete myProcess;
		
		infoFile = QFileInfo(file);
		qDebug() << "converted pdf to ps";
	}	
	
	fileName = infoFile.fileName();

	//render
	qDebug() << "render thumbs";
	QStringList arguments;
	QString output("-sOutputFile="+*conf->getTempFolder()+"/");
	output.append(fileName);
	output.append("%d.png");
	arguments << "-dTextAlphaBits=4"/* << "-dGraphicsAlphaBits=4"*/ <<  "-dNOPAUSE" <<"-dSAFER";
	arguments << RESOLUTION << "-sPAPERSIZE=a4";
	arguments << "-dBATCH"<< "-sDEVICE=png16m" << output;
	arguments << "-f" << infoFile.absoluteFilePath();
	qDebug() << arguments;
	QProcess *myProcess = new QProcess(this);
	myProcess->start("gs", arguments);
	myProcess->waitForFinished(-1);
	qDebug() << "rendered thumbs";
	
	//get page number
	QDir tmp2(*conf->getTempFolder());
	
	QStringList filters;
	QString filePages("");
	filePages.append(fileName);
	filePages.append("*.png");
	
    filters << filePages;
    tmp2.setNameFilters(filters);
    
    QStringList list = tmp2.entryList();
	pages = list.size();
	
	//create picture cache
	cache = new QList<QImage*>();
	qDebug() << "load in cache";
	for(int i = 0; i < pages;i++){
		QString filePage(*conf->getTempFolder()+"/");
		
		filePage.append(fileName);
		filePage.append(QString::number(i+1));
		filePage.append(".png");
		QImage *t = new QImage(filePage);
		if(t->height() < t->width()){
			//TODO: rotate!
		}
		cache->insert(i,t);
		tmp.remove(filePage);
	}
	qDebug() << "loaded in cache";
	deleted = new QList<int>();
	blank = new QList<int>();
	cachePixmaps = new QPixmap[cache->size()];
	cachePixmapsSmall = new QPixmap[cache->size()];
	cachePixmapsMedium = new QPixmap[cache->size()];
	
	/*qDebug() << arguments;
	
	qDebug() << myProcess->exitCode();
	qDebug() << myProcess->readAll();*/
	delete(myProcess);
}

Document::~Document(){
	while (!cache->isEmpty())
	     delete cache->takeFirst();
	delete cache;
	
	delete blank;
	delete deleted;
}

void Document::deleteFile(){
	Config *conf = Config::instance();
	QFileInfo thisFile(file);
	QFileInfo tmpFolder(*conf->getTempFolder());
	QFileInfo watchFolder(*conf->getWatchFolder());

	if(tmpFolder.path().compare(thisFile.path()) == 0 || 
			watchFolder.path().compare(thisFile.path()) == 0){
		QDir tmp(tmpFolder.path());
		tmp.remove(file);
	}
	
	if(!baseFile.isEmpty()){
		QFileInfo thisBaseFile(baseFile);
		if(tmpFolder.path().compare(thisBaseFile.path()) == 0 || 
					watchFolder.path().compare(thisBaseFile.path()) == 0){
			QDir tmp(tmpFolder.path());
			tmp.remove(baseFile);
		}
	}
}

int Document::getSize(bool real){
	if(real){
		return pages + blank->size();
	}else{
		return pages - deleted->size() + blank->size();
	}
}

void Document::addBlankPage(int index, bool real){
	index = realIndex(index,real);
	//index = blankIndex(index);
	
	for(int i = 0; i < blank->size();i++){
		if(blank->at(i) >= index){
			blank->replace(i,blank->at(i)+1);
		}
	}
	
	blank->append(index);
	qSort(blank->begin(), blank->end());
	
	for(int i = 0; i < deleted->size();i++){
		if(deleted->at(i) >= index){
			deleted->replace(i, deleted->at(i) + 1);
		}
	}
}

void Document::deletePage(int index, bool real){
	index = realIndex(index,real);
	
	if(blank->contains(index)){
		blank->removeAll(index);
		
		for(int i = 0; i < deleted->size();i++){
			if(deleted->at(i) >= index){
				deleted->replace(i, deleted->at(i) - 1);
			}
		}
		
		for(int i = 0; i < blank->size();i++){
			if(blank->at(i) >= index){
				blank->replace(i, blank->at(i) - 1);
			}
		}
		return;
	}
	
	if(!deleted->contains(index)){
		deleted->append(index);
		qSort(deleted->begin(), deleted->end());
	}
}

bool Document::isDeleted(int index){
	return deleted->contains(index);
}

void Document::undeletePage(int index,bool real){
	index = realIndex(index,real);
	
	deleted->removeAll(index);
}

QString *Document::getFileName(){
	return new QString(fileName);
}

QString *Document::getFile(){
	return new QString(file);
}

QPixmap Document::getPage(int index, bool real){
	index = realIndex(index,real);
	
	if(blank->contains(index)){
		int width = cache->at(0)->width();
		int height = cache->at(0)->height();		
		QPixmap pic = QPixmap(width, height).scaledToHeight(height * MEDIUM_FACTOR);
		pic.fill();
		return pic;
	}
	
	index = blankIndex(index);
	
	if(index < cache->size()){
		if(cachePixmapsMedium[index].width() == 0){
			cachePixmapsMedium[index] = QPixmap::fromImage(*cache->at(index)).
			scaledToHeight(cache->at(index)->height()*MEDIUM_FACTOR);
		}
		return cachePixmapsMedium[index];
	}
	
	return QPixmap(0,0);
}

QPixmap Document::getSmallPage(int index, bool real){	
	index = realIndex(index,real);
		
	if(blank->contains(index)){
		int width = cache->at(0)->width();
		int height = cache->at(0)->height();		
		QPixmap pic = QPixmap(width, height).scaledToHeight(height * SMALLER_FACTOR);
		pic.fill();
		return pic;
	}
	
	index = blankIndex(index);
	
	if(index < cache->size()){
		if(cachePixmapsSmall[index].width() == 0){
			cachePixmapsSmall[index] = 
				QPixmap::fromImage(*cache->at(index)).
				scaledToHeight(cache->at(index)->height()*SMALLER_FACTOR);
		}
		return cachePixmapsSmall[index];
	}
	
	return QPixmap(0,0);
}

int Document::getRealIndex(int index){
	index = realIndex(index,false);
			
	if(blank->contains(index)){
		return -1;
	}
	
	index = blankIndex(index);
	
	if(index < cache->size()){
		return index + 1;
	}
	
	return -2;
}

QPixmap Document::getBigPage(int index, bool real){	
	index = realIndex(index,real);

	if(blank->contains(index)){
		int width = cache->at(0)->width();
		int height = cache->at(0)->height();		
		QPixmap pic = QPixmap(width, height);
		pic.fill();
		return pic;
	}
	
	index = blankIndex(index);
	
	if(index < cache->size()){
		if(cachePixmaps[index].width() == 0){
			cachePixmaps[index] = QPixmap::fromImage(*cache->at(index));
		}
		return cachePixmaps[index];
	}
	
	return QPixmap(0,0);
}

int Document::blankIndex(int index){
	int temp = 0;
	for(int i = 0; i < blank->size();i++){
		if(blank->at(i) < index){
			temp++;
		}
	}
	return index-temp;
}

int Document::realIndex(int index,bool real){
	if(real) 
		return index;
	
	for(int i = 0; i < deleted->size();i++){
		if(deleted->at(i)<= index){
			index++;
		}
	}
	return index;
}

