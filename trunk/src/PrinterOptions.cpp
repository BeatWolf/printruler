#include "PrinterOptions.h"

PrinterOptions::PrinterOptions(QWidget * parent, QString printerName): QDialog(parent)
{
	QWidget *listWidget = new QWidget;
	printer = printerName;
	setWindowTitle("Printer options");
	QPushButton *ok = new QPushButton("Ok");
	connect(ok, SIGNAL(clicked()), this, SLOT(save()));
	QPushButton *cancel = new QPushButton("Cancel");
	connect(cancel, SIGNAL(clicked()), this, SLOT(cancel()));
	
	QGridLayout *layout = new QGridLayout(); //listwidget
	QGridLayout *layout2 = new QGridLayout(); //main widget
	
	//get the printer options from cups
	QProcess *myProcess = new QProcess(this);
	QStringList arguments;
	arguments << "-p" << printer << "-l";	
	myProcess->start("lpoptions", arguments);
	myProcess->waitForFinished(-1);
	
	QByteArray output = myProcess->readAllStandardOutput();
	
	QString temp(output);
	QStringList list = temp.split('\n');
	
	//fill list with the options
	for(int i = 0; i < list.size(); i++){
		QStringList line = list.at(i).split(':');
		
		if(line.size() > 1){
			QStringList options = line.at(1).split(' ');
			
			QLabel *label = new QLabel(line.at(0).split('/').at(1));
			
			QComboBox *box = new QComboBox();
			
			int select = -1;
			
			for(int loop = 1; loop < options.size(); loop++){
				QString tempOption = options.at(loop);
				//remove star from default
				if(tempOption.startsWith("*")){
					tempOption = tempOption.remove(0,1);
					select = loop - 1;
				}
				box->addItem(tempOption);
			}
			
			optionList.append(line.at(0).split('/').at(0));
			optionValues.append(box);
			optionDefaults.append(select);
			
			if(select == -1){
				box->addItem("");
				select = box->count() - 1;
			}
			
			box->setCurrentIndex(select);//select default option
					
			layout->addWidget(label,i,0);
			layout->addWidget(box,i,1);			
		}
		
	}
	
	delete myProcess;
	
	listWidget->setLayout(layout);
	
	QScrollArea *scrollArea = new QScrollArea;
	scrollArea->setWidget(listWidget);
	
	layout2->addWidget(scrollArea,0,0,1,2);
	layout2->addWidget(ok,1,0);
	layout2->addWidget(cancel,1,1);
	setLayout(layout2);
	resize(listWidget->width() + 50,300);
}

PrinterOptions::~PrinterOptions()
{
}

void PrinterOptions::cancel(){
	printer = "";
	reject();
}

void PrinterOptions::save(){
	accept();
}

QStringList PrinterOptions::getOptions(){
	QStringList list;
	
	for(int i = 0; i < optionList.size(); i++){
		if(optionDefaults.at(i) != optionValues.at(i)->currentIndex()){
			QString temp = optionList.at(i);
			temp += "=";
			QString value = optionValues.at(i)->itemText(optionValues.at(i)->currentIndex());
			if(value.compare("") != 0){
				temp += value;
				list << temp;
			}
		}
	}
	
	return list;
}
