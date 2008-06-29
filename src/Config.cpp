#include "Config.h"

Config *Config::m_instance = 0;

void Config::setWatchFolder(QString &folder){
	if(watchFolder){
		delete watchFolder;
	}
	
	watchFolder = new QString(folder);
	saveConfigFile();
	emit watchFolderChanged(*watchFolder);
}

Config::Config()
{
	QDir configDir(QDir::homePath());
	configDir.mkdir(".printruler");
	
	configFile = new QString(QDir::homePath()+"/.printruler/config.xml");
	
	loadConfigFile();
}

void Config::setTempFolder(QString &folder){
	if(tmpFolder){
		delete tmpFolder;
	}
	
	tmpFolder = new QString(folder);
	saveConfigFile();
}

void Config::setDefaultPrinter(QString printer){
	if(defaultPrinter){
		delete defaultPrinter;
	}
	
	defaultPrinter = new QString(printer);
	saveConfigFile();
}

QString *Config::getWatchFolder(){
	return watchFolder;
}

QString *Config::getTempFolder(){
	return tmpFolder;
}

QString *Config::getPrinterName(){
	return printerName;
}

QString *Config::getDefaultPrinter(){
	 qDebug() << "printer return: "<< *defaultPrinter;
	return defaultPrinter;
}

bool Config::getDefaultDeleting(){
	return deleteJobsAfterPrinting;
}
	
void Config::setDefaultDeleting(bool deleting){
	deleteJobsAfterPrinting = deleting;
}

bool Config::isRotateEven(){
	return rotateEven;
}

bool Config::getShowOnOpen(){
	return showOnOpen;
}

void Config::setShowOnOpen(bool show){
	showOnOpen = show;
}
	
void Config::setRotateEven(bool rotate){
	rotateEven = rotate;
}
	
int Config::getSides(){
	return sides;
}
		
void Config::setSides(int s){
	sides = s;
}

QString Config::getIconFolder(){
	if(!QFile("list-add.png").exists()){
		return "/usr/share/printruler/";
	}else{
		return "";
	}
}

void Config::loadConfigFile(){
	QDomDocument doc("config");
	
	QFile file(*configFile);
	if (!file.open(QIODevice::ReadOnly)){
		loadDefault();
	    return;
	}
	if (!doc.setContent(&file)) {
	    file.close();
	    loadDefault();
	    return;
	}
	
	QDomElement docElem = doc.documentElement();
	
	QDomNode n = docElem.firstChildElement("watchfolder");
	
	if(!n.isNull()){
		QDomElement e = n.toElement();
		watchFolder = new QString(e.text());
	}else{
		watchFolder = new QString(QDir::homePath());
		watchFolder->append(DEFAULT_WATCHFOLDER);
	}
	
	n = docElem.firstChildElement("tempfolder");
	
	if(!n.isNull()){
		QDomElement e = n.toElement();
		tmpFolder = new QString(e.text());
	}else{
		tmpFolder = new QString(DEFAULT_TEMPFOLDER);
	}
	
	n = docElem.firstChildElement("hiddenprintername");
		
	if(!n.isNull()){
		QDomElement e = n.toElement();
		printerName = new QString(e.text());
	}else{
		printerName = new QString(DEFAULT_PRINTERNAME);
	}
	
	n = docElem.firstChildElement("defaultprinter");
			
	if(!n.isNull()){
		QDomElement e = n.toElement();
		defaultPrinter = new QString(e.text());
		qDebug() << "loaded printer: "<< *defaultPrinter;
	}else{
		defaultPrinter = new QString(DEFAULT_PRINTER);
	}
	
	n = docElem.firstChildElement("deleteprinted");
				
	if(!n.isNull()){
		QDomElement e = n.toElement();
		deleteJobsAfterPrinting =e.text().compare("true") == 0;
	}else{
		deleteJobsAfterPrinting = DEFAULT_DELETE;
	}
	
	n = docElem.firstChildElement("rotateevenpages");
					
	if(!n.isNull()){
		QDomElement e = n.toElement();
		rotateEven =e.text().compare("true") == 0;
	}else{
		rotateEven = DEFAULT_ROTATE;
	}
	
	n = docElem.firstChildElement("twosidedprinting");
						
	if(!n.isNull()){
		QDomElement e = n.toElement();
		sides =e.text().toInt();
	}else{
		sides = DEFAULT_SIDES;
	}
	
	n = docElem.firstChildElement("showonopen");
						
	if(!n.isNull()){
		QDomElement e = n.toElement();
		showOnOpen =e.text().compare("true") == 0;
	}else{
		showOnOpen = DEFAULT_SHOW_ON_OPEN;
	}
	
	file.close();
}

void Config::loadDefault(){
	watchFolder = new QString(QDir::homePath());
	watchFolder->append(DEFAULT_WATCHFOLDER);
	tmpFolder = new QString(DEFAULT_TEMPFOLDER);
	printerName = new QString(DEFAULT_PRINTERNAME);
	defaultPrinter = new QString(DEFAULT_PRINTER);
	deleteJobsAfterPrinting = DEFAULT_DELETE;
	rotateEven = DEFAULT_ROTATE;
	showOnOpen = DEFAULT_SHOW_ON_OPEN;
	saveConfigFile();
}

void Config::saveConfigFile(){
	 QDomDocument doc("config");
	 QDomElement root = doc.createElement("config");
	 doc.appendChild(root);

	 QDomElement watch = doc.createElement("watchfolder");
	 root.appendChild(watch);
	 QDomText watchValue = doc.createTextNode(*watchFolder);
 	 watch.appendChild(watchValue);

	 QDomElement temp = doc.createElement("tempfolder");
	 root.appendChild(temp);
	 QDomText tempValue = doc.createTextNode(*tmpFolder);
	 temp.appendChild(tempValue);
	 
	 QDomElement printer = doc.createElement("hiddenprintername");
 	 root.appendChild(printer);
 	 QDomText printerValue = doc.createTextNode(*printerName);
 	 printer.appendChild(printerValue);
	 
 	 QDomElement defPrinter = doc.createElement("defaultprinter");
 	 root.appendChild(defPrinter);
 	 QDomText defaultPrinterValue = doc.createTextNode(*defaultPrinter);
 	 defPrinter.appendChild(defaultPrinterValue);
 	 
 	 //delete printed
 	 QDomElement defdelete = doc.createElement("deleteprinted");
 	 root.appendChild(defdelete);
 	 QString tempDeleted("true");
 	 if(!deleteJobsAfterPrinting){
 		 tempDeleted = "false";
 	 }
 	 
 	 QDomText deletePrintedValue = doc.createTextNode(tempDeleted);
 	 defdelete.appendChild(deletePrintedValue);
 	 //deletedprinted end
 	 
 	//colored output
 	 QDomElement defrotate = doc.createElement("rotateevenpages");
 	 root.appendChild(defrotate);
 	 QString tempRotate("true");
 	 if(!rotateEven){
 		tempRotate = "false";
 	 }
 	 
 	 QDomText rotateValue = doc.createTextNode(tempRotate);
 	 defrotate.appendChild(rotateValue);
 	 
 	 //show on load
 	 QDomElement defshow = doc.createElement("showonopen");
 	 root.appendChild(defshow);
 	 QString tempShow("true");
 	 if(!showOnOpen){
 		tempShow = "false";
 	 }
 	 
 	 QDomText showValue = doc.createTextNode(tempShow);
     defshow.appendChild(showValue);
 	 
 	 //deletedprinted end
 	 
 	 QDomElement defsides = doc.createElement("twosidedprinting");
	 root.appendChild(defsides);
	 QDomText sidesValue = doc.createTextNode(QString::number(sides));
	 defPrinter.appendChild(sidesValue);
 	 
	 doc.implementation().setInvalidDataPolicy(QDomImplementation::DropInvalidChars);
	 QString xml = doc.toString();
	 
	 QFile config(*configFile);
	 if(!config.open(QIODevice::WriteOnly)){
		 qDebug() << "Error writing config file";
	 }
	 
	 config.write(xml.toAscii());
	 
}

