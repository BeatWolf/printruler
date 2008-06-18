#include "DocumentCreator.h"

DocumentCreator::DocumentCreator(VirtualDocument *doc, QString fileName)
{
	virtDoc = doc;
	file = fileName;
	pdf = false;
}

void DocumentCreator::run(){
	ActionScript action(virtDoc, file);
	action.run();
	
	QFile myfile(file);
	myfile.rename(file+".temp");
	
	psToPdf(file+".temp",file+".pdf");
	QFile::remove(file+".temp");
	
	//should be done in action script, but can't
	if(rotateEven){
		pdfToPs(file+".pdf" , file+".temp");
		
		QString programPstoPs = "pstops";
		QStringList argumentsPstoPs;
		argumentsPstoPs << "2:0,1U(21cm,29.7cm)";
		argumentsPstoPs << file+".temp" << file+".temp2";
		qDebug() << programPstoPs;
		qDebug() << argumentsPstoPs;
		QProcess *myProcessPstoPs = new QProcess();
		myProcessPstoPs->start(programPstoPs, argumentsPstoPs);
		myProcessPstoPs->waitForFinished();
		QFile::remove(file+".temp");
		
		psToPdf(file+".temp2",file+".pdf");
		QFile::remove(file+".temp2");
	}
	
	if(!pdf){
		pdfToPs(file+".pdf" , file);
		QFile::remove(file+".pdf");
	}else{
		QFile pdfFile(file+".pdf");
		pdfFile.rename(file);
	}
	
	emit done();
}

void DocumentCreator::setPDF(bool ispdf){
	pdf = ispdf;
}

void DocumentCreator::psToPdf(QString ps, QString pdf){
	QStringList argumentsPdf;		
	argumentsPdf << ps << pdf;
	
	QProcess *myProcess = new QProcess();
	myProcess->start("ps2pdf", argumentsPdf);
	myProcess->waitForFinished(-1);
	delete myProcess;
}

void DocumentCreator::pdfToPs(QString pdf, QString ps){
	QStringList argumentsPdf;		
	argumentsPdf << pdf << ps;
	
QProcess *myProcess = new QProcess();
	myProcess->start("pdftops", argumentsPdf);
	myProcess->waitForFinished(-1);
	delete myProcess;
}

DocumentCreator::~DocumentCreator()
{
}
