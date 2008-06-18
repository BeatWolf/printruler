#include "ActionScript.h"
#include "VirtualDocument.h"
#include <QProcess>
#define STR_FILE_PSNUP "tmp_file_psnup.ps"
#define STR_FILE_PSSELECT "tmp_file_psselect.ps"
#define STR_FILE_PSBOOK "tmp_file_psbook.ps"
#define I_MULTI_BORDER 0.15
#define I_MULTI_MARGIN 0.3
#include "Config.h"

ActionScript::ActionScript(VirtualDocument *vdDist, QString destinationDist)
{
	vd = vdDist;
	destination = destinationDist;
	listTmpDocuments = new QList<QString>;
}
ActionScript::~ActionScript()
{
	qDebug()<<"Destruction";
	Config *conf = Config::instance();
	
	QFile::remove(*conf->getTempFolder()+STR_FILE_PSNUP);
	QFile::remove(*conf->getTempFolder()+STR_FILE_PSBOOK);
	for(int i = 0; i < vd->documents(); i++)
		QFile::remove(listTmpDocuments->at(i));
}
void ActionScript::run()
{	
	for(int i = 0; i < vd->documents(); i++)
		launchDeleteBlank(i);

	if(vd->isSeperateJobs())
	{
		for(int i = 0; i < vd->documents(); i++)
			launchMarginBorderNup(listTmpDocuments->at(i));
		launchMerge();
	}
	else
	{
		launchMerge();
		launchMarginBorderNup(destination);
	}
	if(vd->isBooklet())
		launchBooklet();
}

/* MERGE */
void ActionScript::launchMerge()
{
	qDebug()<<"Start ScriptMerge";
	/* launchScript */
	QString program = "gs";
	QStringList arguments;
	arguments << "-dNOPAUSE";
	arguments << "-sDEVICE=pswrite";
	arguments << "-dBATCH";
	arguments << "-dEmbedAllFonts=true";
	QString str;
	str.append("-sOutputFile=");
	str.append(destination);
	arguments << str;
	for(int i = 0; i < vd->documents(); i++)
	{
		arguments << listTmpDocuments->at(i);
	}
	qDebug() << program;
	qDebug() << arguments;
	QProcess *myProcess = new QProcess(this);
	myProcess->start(program, arguments);
	myProcess->waitForFinished();
	qDebug()<<"Finish ScriptMerge";
	
	/*arguments.clear();
	arguments << "-o"+destination;
	arguments << destination+".temp";
	
	myProcess->start("psmerge", arguments);
	myProcess->waitForFinished(-1);
	qDebug() << arguments;*/
	delete myProcess;
}

/* MARGIN, BORDER and NUP */
void ActionScript::launchMarginBorderNup(QString src)
{
	Config *conf = Config::instance();
	qDebug()<<"Start ScriptMarginBorderNup";
	QString program = "psnup";
	QStringList arguments;
	/* nup */
	if(vd->getNup()!=1)
	{
		//qDebug()<<"Add Nup";
		QString str;
		str.append("-");
		str.append(QString::number(vd->getNup()));
		arguments << str;
	}
	
	
	int margin = vd->getMargin();
	if(vd->getBorder()>0){
		margin += 2;
	}
	
	arguments << "-pA4";
	
	/* margin */
	if(vd->getMargin()>0 || margin > 0)
	{
		//qDebug()<<"Add Margin";
		margin += vd->getMargin();
		double d_margin = margin*I_MULTI_MARGIN;
		arguments << "-m" + QString::number(d_margin) +"cm";
	}
	/* border */
	if(vd->getBorder()>0)
	{
		//qDebug()<<"Add Border";
		double d_border = vd->getBorder()*I_MULTI_BORDER;
		arguments << "-d" + QString::number(d_border) + "cm";
	}
	QString tmp_file = *conf->getTempFolder()+STR_FILE_PSNUP;
	QFile *tmp_qfile = new QFile(tmp_file);
	arguments << src << tmp_file;
	qDebug() << program;
	qDebug() << arguments;
	QProcess *myProcess = new QProcess(this);
	myProcess->start(program, arguments);
	myProcess->waitForFinished();
	QFile::remove(src);
	tmp_qfile->copy(src);
	qDebug()<<*conf->getTempFolder()+STR_FILE_PSNUP<<"->"<<src;	
	qDebug()<<"Finish ScriptMarginBorderNup";
}

/* DELETE and BLANK */
void ActionScript::launchDeleteBlank(int current)
{
	Config *conf = Config::instance();
	qDebug()<<"Start ScriptDeleteBlank";
	int currentDocument = vd->getDocumentSize(current);
	QString program = "psselect";
	QStringList arguments;
	QString str; 
	str.append("-p");
	//qDebug() << "sizeDoc" << currentDocument;
	
	for(int i = 0; i < currentDocument; i++)
	{
		//qDebug() << "Begin:" << current << "/" << i;
		int index = vd->getRealIndex(current, i);
		switch(index){
		case -1:
			//qDebug() << "BlankPage:" << current << "/" << i;
			str.append("_");
			break;
		case -2:
			qDebug() << "Error : Selected Page unexistant";
			break;
		default:
			//qDebug() << "NormalPage:" << current << "/" << i;
			str.append(QString::number(index));
			break;
		}
		if(i+1 != currentDocument)
			str.append(",");
	}
	arguments << str;
	
	QString tmp_file;
	tmp_file.append(*conf->getTempFolder());
	tmp_file.append(QString::number(current));
	tmp_file.append("_");
	tmp_file.append(STR_FILE_PSSELECT);
	listTmpDocuments->append(tmp_file);
	arguments << vd->getPageSource(current) << tmp_file;
	qDebug() << program;
	qDebug() << arguments;
	QProcess *myProcess = new QProcess(this);
	myProcess->start(program, arguments);
	myProcess->waitForFinished();
	qDebug()<<"Finish ScriptDeleteBlank";
}

/* BOOKLET */
void ActionScript::launchBooklet()
{
	Config *conf = Config::instance();
	qDebug()<<"Start ScriptBooklet";
	QString programBook = "psbook";
	QStringList argumentsBook;
	QString tmp_file = *conf->getTempFolder()+STR_FILE_PSBOOK;
	QFile *tmp_qfile = new QFile(tmp_file);
	argumentsBook << destination << tmp_file;
	qDebug() << programBook;
	qDebug() << argumentsBook;
	QProcess *myProcessBook = new QProcess(this);
	myProcessBook->start(programBook, argumentsBook);
	myProcessBook->waitForFinished();
	QFile::remove(destination);
	tmp_qfile->copy(destination);
	qDebug()<<STR_FILE_PSBOOK<<"->"<<destination;
	
	QString programNup = "psnup";
	QStringList argumentsNup;
	argumentsNup << "-l" << "-2"; 
	argumentsNup << destination << tmp_file;
	qDebug() << programNup;
	qDebug() << argumentsNup;
	QProcess *myProcessNup = new QProcess(this);
	myProcessNup->start(programNup, argumentsNup);
	myProcessNup->waitForFinished();
	QFile::remove(destination);
	tmp_qfile->copy(destination);
	qDebug()<<*conf->getTempFolder()+STR_FILE_PSBOOK<<"->"<<destination;
	
	
	

	qDebug()<<"Finish ScriptBooklet";
	
	delete myProcessBook;
	delete myProcessNup;
}
