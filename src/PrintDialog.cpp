#include "PrintDialog.h"

PrintDialog::PrintDialog(VirtualDocument *document)
{
	options = 0;
	setWindowTitle("Print");
	Config *conf = Config::instance();
	doc = document;
	config = Config::instance();
	QGridLayout *layout = new QGridLayout();
	firstSide = true;

	printerChooser = new QComboBox();
	fillPrintChooser();
	QPushButton *print = new QPushButton("Print");
	connect(print, SIGNAL(clicked()), this, SLOT(print()));
	QPushButton *cancel = new QPushButton("Cancel");
	connect(cancel, SIGNAL(clicked()), this, SLOT(cancel()));

	QPushButton *properties = new QPushButton("Properties");
	connect(properties, SIGNAL(clicked()), this, SLOT(properties()));
	
	deleteAfter = new QCheckBox("Delete jobs afterwards");
	deleteAfter->setChecked(conf->getDefaultDeleting());
	
	manualDuplex = new QCheckBox("Manual duplex printing");
	connect(manualDuplex, SIGNAL(stateChanged(int)), this, SLOT(manualDuplexChanged(int)));
	manualDuplex->setChecked(conf->getSides() == 1);
	
	rotateEven = new QCheckBox("Rotate even pages (for booklet)");
	rotateEven->setChecked(conf->isRotateEven());
	rotateEven->setToolTip("Rotates every second page. This is usefull\n if the printer does not have a option to turn\n the pages correctly");
	
	spinBox = new QSpinBox();
	spinBox->setMinimum(1);
	spinBox->setMaximum(100);
	spinBox->setEnabled(conf->getSides() != 1);
	
	QLabel *copies = new QLabel("Copies:");
	
	layout->setColumnMinimumWidth(0,150);

	layout->addWidget(printerChooser,0,0,1,2);
	layout->addWidget(properties,0,2);
	layout->addWidget(manualDuplex,1,0,1,2);
	layout->addWidget(deleteAfter,3,0);
	layout->addWidget(rotateEven,2,0);
	
	layout->addWidget(copies,3,2);
	layout->addWidget(spinBox,4,2);
	layout->addWidget(print,5,1);
	layout->addWidget(cancel,5,2);
	

	setLayout(layout);
}

void PrintDialog::manualDuplexChanged(int state){
	spinBox->setEnabled(state != Qt::Checked);
}

PrintDialog::~PrintDialog()
{
}

void PrintDialog::fillPrintChooser(){
	Config *conf = Config::instance();
	QProcess *printerInfos = new QProcess();
	printerInfos->start("lpstat -a");
	printerInfos->waitForFinished();
	QByteArray output = printerInfos->readAllStandardOutput();

	QString temp(output);
	QStringList list = temp.split('\n');
	int defaultPrinter = 0;
	int loop = 0;
	for(int i = 0; i < list.size(); i++){
		QStringList name = list.at(i).split(' ');
		if(name.size() > 1 && name.at(0).compare(*conf->getPrinterName()) != 0){
			printerChooser->addItem(QIcon(conf->getIconFolder()+"document-print.png"),name.at(0));

			QString tmp(name.at(0));
			tmp.append("0"); //very ugly workaround for a strange bug
			
			qDebug() << tmp << *conf->getDefaultPrinter();
			
			if(name.at(0).compare(*conf->getDefaultPrinter()) == 0 ||
					tmp.compare(*conf->getDefaultPrinter()) == 0){
				defaultPrinter = loop;
			}
			loop++;
		}
	}

	printerChooser->setCurrentIndex(defaultPrinter);

	delete printerInfos;
}

void PrintDialog::cancel(){
	reject();
}

void PrintDialog::print(){
	Config *conf = Config::instance();
	conf->setDefaultDeleting(deleteAfter->isChecked());
	conf->setDefaultPrinter(printerChooser->itemText(printerChooser->currentIndex()));
	conf->setRotateEven(rotateEven->isChecked());
	saveConfig();
	
	fileName = *conf->getTempFolder()+"/tempPrint.pdf";

	//create documentcreater
	creator = new DocumentCreator(doc,fileName);
	//creator->setPDF(true);
	creator->setRotateEven(rotateEven->isChecked());
	connect(creator, SIGNAL(done()), this, SLOT(sendToPrinter()));

	progress = new QProgressDialog("Printing",0,0,0,this,Qt::Dialog);
	progress->show();
	creator->start();
	setEnabled(false);
}

void PrintDialog::sendToPrinter(){
	QProcess *printerProcess = new QProcess();
	QStringList arguments;
	
	saveConfig();
	
	if(manualDuplex->isChecked() && firstSide){
		arguments << "-o page-set=even";
		firstSide = false;
	}
	else if(manualDuplex->isChecked() && !firstSide){
		arguments << "-o page-set=odd";
		firstSide = true;
	}
	
	qDebug() << "get printer options";
	if(options && 
			options->getPrinterName().compare(printerChooser->itemText(printerChooser->currentIndex())) == 0){
		qDebug() << "we have options";
		QStringList list = options->getOptions();
		for(int i = 0; i < list.size(); i++){
			arguments << "-o" << list.at(i);
		}
	}
	
	if(!manualDuplex->isChecked()){
		arguments << "-#" << QString::number(spinBox->value());
	}

	arguments << "-P" << printerChooser->itemText(printerChooser->currentIndex()) << fileName;
	qDebug() << "lpr";
	qDebug() << arguments;
	printerProcess->start("lpr",arguments);
	printerProcess->waitForFinished();

	if(manualDuplex->isChecked() && !firstSide){
		int ret = QMessageBox::information(this, "PrintRuler : unSupported Duplex Dialog",
				"Turn pages in printer.\n Click Ok when it is done.", QMessageBox::Ok);
		if(ret == QMessageBox::Ok)
			sendToPrinter();
	}else{
		progress->hide();

		delete progress;
	}

	delete printerProcess;
	
	Config *conf = Config::instance();
	if(conf->getDefaultDeleting()){
		
		for(int i = 0; i < doc->documents(); i++){
			doc->removeDocument(i);
		}
		
	}
	
	//delete documents printed
	accept();
}

void PrintDialog::saveConfig(){
	
	config->saveConfigFile();
}

void PrintDialog::properties(){
	if(options){
		delete options;
	}
	options = new PrinterOptions(this,
			printerChooser->itemText(printerChooser->currentIndex()));
	options->exec();
}
