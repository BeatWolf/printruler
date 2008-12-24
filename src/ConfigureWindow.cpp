#include "ConfigureWindow.h"

ConfigureWindow::ConfigureWindow()
{
	conf = Config::instance();

	setWindowTitle("Configure");

	QLabel *label = new QLabel("Check directory");
	checkFolder = new QLineEdit();
	checkFolder->setText(*conf->getWatchFolder());
	checkFolder->setMinimumWidth(50);
	QPushButton *selectFolder = new QPushButton("Select..");
	connect(selectFolder, SIGNAL(clicked()), this, SLOT(changeWatchFolder()));

	layout = new QGridLayout();
	layout->addWidget(label,0,0);
	layout->addWidget(checkFolder,0,1);
	layout->addWidget(selectFolder,0,2);

	QLabel *labelShow = new QLabel("Start in systray only");
	showOnOpen = new QCheckBox();
	showOnOpen->setChecked(!conf->getShowOnOpen());

	layout->addWidget(showOnOpen,1,1);
	layout->addWidget(labelShow,1,0);

	QPushButton *save = new QPushButton("Save");
	connect(save, SIGNAL(clicked()), this, SLOT(save()));
	QPushButton *cancel = new QPushButton("Cancel");
	connect(cancel, SIGNAL(clicked()), this, SLOT(cancel()));
	layout->addWidget(save,2,1);
	layout->addWidget(cancel,2,2);

	setLayout(layout);
	resize(400,100);
}

ConfigureWindow::~ConfigureWindow()
{
	delete layout;
}

void ConfigureWindow::save(){
	QDir dir(checkFolder->text());

	if(dir.exists()){
		QString temp = checkFolder->text();
		conf->setWatchFolder(temp);
	}

	conf->setShowOnOpen(!showOnOpen->isChecked());
	conf->saveConfigFile();
	accept();
}

void ConfigureWindow::cancel(){
	reject();
}

void ConfigureWindow::changeWatchFolder(){
	QFileDialog::Options options;
	QString selectedFilter;
	QString fileName = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
            "/home",
            QFileDialog::ShowDirsOnly
            | QFileDialog::DontResolveSymlinks);

	QDir dir(fileName);

	if(dir.exists()){
		checkFolder->setText(fileName);
	}
}
