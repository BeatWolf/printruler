
#include <QtGui>

#include <QWaitCondition>
#include "Gui.h"
#include "ConfigureWindow.h"
#include "PrintDialog.h"
#include "ActionScript.h"

Gui::Gui()
{
	defaultOpenPath = QDir::homePath();
	
	doc = new VirtualDocument();
	conf = Config::instance();
	mutex = new QMutex();
	
	watch = new WatchDog(conf);
	connect(watch, SIGNAL(fileFound(QString*, bool)), this, SLOT(addDocument(QString*, bool)));
	connect(conf, SIGNAL(watchFolderChanged(QString)), watch, SLOT(folderChanged(QString)));

	zoomlevel = 0;
	documentsLoading = 0;
	
	iconPath = conf->getIconFolder();
}

Gui::~Gui(){
	delete doc;
	watch->stop();
	delete mutex;
}

void Gui::setActionEnabled(bool enable){
	printDocument->setEnabled(enable);
	exportPDFAction->setEnabled(enable);
	exportPSAction->setEnabled(enable);
	zoomInAction->setEnabled(enable);
	zoomOutAction->setEnabled(enable);
}

void Gui::initGui(){
	//Actions
	addDocumentAction = new QAction(QIcon(iconPath+"list-add.png"),QString("add"),this);
	connect(addDocumentAction, SIGNAL(triggered()), this, SLOT(add()));
	printDocument = new QAction(QIcon(iconPath+"document-print.png"),QString("Print"),this);	
	connect(printDocument, SIGNAL(triggered()), this, SLOT(print()));
	QAction *exitAction = new QAction("Close",this);
	connect(exitAction, SIGNAL(triggered()), this, SLOT(end()));
	QAction *configureAction = new QAction("Configure",this);
	connect(configureAction, SIGNAL(triggered()), this, SLOT(configure()));
	
	exportPDFAction = new QAction("Export as PDF",this);
	connect(exportPDFAction, SIGNAL(triggered()), this, SLOT(exportPDF()));
	exportPSAction = new QAction("Export as PS",this);
	connect(exportPSAction, SIGNAL(triggered()), this, SLOT(exportPS()));
	
	zoomInAction = new QAction(QIcon(iconPath+"zoom-in.png"),"Zoom in",this);
	connect(zoomInAction, SIGNAL(triggered()), this, SLOT(zoomIn()));
	
	zoomOutAction = new QAction(QIcon(iconPath+"zoom-out.png"),"Zoom out",this);
	connect(zoomOutAction, SIGNAL(triggered()), this, SLOT(zoomOut()));
	
	aboutAction = new QAction("Printruler",this);
	connect(aboutAction, SIGNAL(triggered()), this, SLOT(aboutDialog()));
	
	setActionEnabled(false);
	//System tray
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QIcon(iconPath+"document-print.png"));
	trayIcon->show();
	QMenu *trayMenu = new QMenu("TrayMenu");
	trayMenu->addAction(printDocument);
	trayMenu->addAction(exitAction);
	trayIcon->setContextMenu(trayMenu);
	connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
	             this, SLOT(iconActivated(QSystemTrayIcon::ActivationReason)));
	
	
	//Menu
	QMenuBar *menubar = new QMenuBar();
	
	QMenu *fileMenu = new QMenu("File");
	
	fileMenu->addAction(addDocumentAction);
	fileMenu->addAction(printDocument);
	fileMenu->addAction(exportPDFAction);
	fileMenu->addAction(exportPSAction);
	fileMenu->addSeparator();
	fileMenu->addAction(exitAction);
	
	menubar->addMenu(fileMenu);
	
	QMenu *settingsMenu = new QMenu("Settings");
	settingsMenu->addAction(configureAction);
	menubar->addMenu(settingsMenu);
	
	QMenu *aboutMenu = new QMenu("About");
	aboutMenu->addAction(aboutAction);
	menubar->addMenu(aboutMenu);
	
	setMenuBar(menubar);
	
	//preview
	preview = new PreviewPanel(this);
	preview->setDocument(doc);
	connect(preview, SIGNAL(documentRendered()), this, SLOT(documentUpdated()));
	setCentralWidget(preview);
	
	//Statusbar
	statusBar = new QStatusBar(this);
	setStatusBar(statusBar);
	progress = new QProgressBar();
	pageNumber = new QLabel("Pages: "+QString::number(0));
	
	statusBar->addWidget(progress,0);
	statusBar->addPermanentWidget(pageNumber,0);
	
	progress->hide();
	
	//Toolbar
	QToolBar *toolbar = new QToolBar();
	toolbar->setMovable(false);
	toolbar->addAction(printDocument);
	toolbar->addAction(addDocumentAction);
	toolbar->addSeparator();
	toolbar->addAction(zoomInAction);
	toolbar->addAction(zoomOutAction);
	addToolBar(toolbar);
	
	//left dockwidget
	dock = new QDockWidget();
	dock->setEnabled(false);
	dock->setFeatures(QDockWidget::DockWidgetMovable );
	
	//Page layout
	QWidget *dockWidget = new QWidget;
	QVBoxLayout *vboxDock = new QVBoxLayout;
	dockWidget->setLayout(vboxDock);
	
	QGroupBox *groupBox = new QGroupBox("Layout");
	QGridLayout *vbox = new QGridLayout;
	vbox->setColumnMinimumWidth(1,2);
	
	group = new QButtonGroup();

	QRadioButton *n1up = new QRadioButton("1", dock);
	n1up->setChecked(true);
	QRadioButton *n2up = new QRadioButton("2", dock);
	n4up = new QRadioButton("4", dock);
	n8up = new QRadioButton("8", dock);	
	QCheckBox *booklet = new QCheckBox("Booklet");
	connect(booklet, SIGNAL( stateChanged (int)),
				this, SLOT(bookletChanged ( int )));
	
	group->addButton(n1up,1);
	group->addButton(n2up,2);
	group->addButton(n4up,4);
	group->addButton(n8up,8);
	
	connect(group, SIGNAL(buttonClicked(int)), this, SLOT(layoutChanged(int)));
	
	vbox->addWidget(n1up,0,0);
	vbox->addWidget(n2up,1,0);
	vbox->addWidget(n4up,0,1);
	vbox->addWidget(n8up,1,1);
	vbox->addWidget(booklet,2,0);

	groupBox->setLayout(vbox);
	vboxDock->addWidget(groupBox);

	
	//Options Margin
	QComboBox *margin = new  QComboBox();
	margin->setEditable(false);
	margin->addItem("None");
	margin->addItem("small");
	margin->addItem("big");
	connect(margin, SIGNAL(currentIndexChanged(int)), this, SLOT(marginChanged(int)));
	QLabel *marginLabel = new QLabel("Margins");
	
	vboxDock->addWidget(marginLabel);
	vboxDock->addWidget(margin);
	
	//Options border
	QComboBox *border = new  QComboBox();
	border->setEditable(false);
	border->addItem("None");
	border->addItem("small");
	border->addItem("big");
	connect(border, SIGNAL(currentIndexChanged(int)), this, SLOT(borderChanged(int)));
	
	QLabel *borderLabel = new QLabel("Borders");
	
	vboxDock->addWidget(borderLabel);
	vboxDock->addWidget(border);
	
	//Document list
	listWidget = new QListWidget(this);
	vboxDock->addWidget(listWidget);
	
	dock->setMaximumWidth(50);
	dock->setWidget(dockWidget);
	

	QToolBar *documentToolbar = new QToolBar();
	QAction *deleteDocument = new QAction(QIcon(iconPath+"edit-delete.png"),QString("delete"),this);
	QAction *moveUpDocument = new QAction(QIcon(iconPath+"arrow-up.png"),QString("move up"),this);
	QAction *moveDownDocument = new QAction(QIcon(iconPath+"arrow-down.png"),QString("move down"),this);
	connect(deleteDocument, SIGNAL(triggered()), this, SLOT(deleteDocument()));
	connect(moveUpDocument, SIGNAL(triggered()), this, SLOT(moveUpDocument()));
	connect(moveDownDocument, SIGNAL(triggered()), this, SLOT(moveDownDocument()));
	
	documentToolbar->addAction(deleteDocument);
	documentToolbar->addAction(moveUpDocument);
	documentToolbar->addAction(moveDownDocument);
	vboxDock->addWidget(documentToolbar);
	
	QCheckBox *seperate = new QCheckBox("Seperate jobs");
	connect(seperate, SIGNAL( stateChanged (int)),
			this, SLOT(seperateJobs ( int )));
	
	vboxDock->addWidget(seperate);
	
	QCheckBox *showDeleted = new QCheckBox("Show deleted pages");
	connect(showDeleted, SIGNAL( stateChanged (int)),
			this, SLOT(showDeletedPages ( int )));
	vboxDock->addWidget(showDeleted);

	vboxDock->addStretch(1);
	addDockWidget(Qt::LeftDockWidgetArea,dock);	
	resize(DEFAULT_SIZE_X, DEFAULT_SIZE_Y);
	show();
	
	testDependencies();
	
	watch->run();
}

void Gui::testDependencies(){
	QString  apps[] = {"gs", "ps2pdf", "ps2ps2", "psnup",
			"psbook", "psmerge", "psselect", "pdftops", "ps2ps"};
	QProcess *proc = new QProcess();
	for(int i = 0; i < 9; i++){
		proc->start(apps[i]);
		proc->waitForStarted();
		if(proc->error() != QProcess::UnknownError){
			QMessageBox::critical(this, "Could not find",
			                   "The application "+apps[i]+" could not be found.\n"+
			                		   "This application is needed for printruler.\n"+
			                		   "Please install it.",
			                   QMessageBox::Cancel);
		}
		proc->kill();
		proc->waitForFinished();
	}
}

void Gui::documentLoads(){
	mutex->lock();
	documentsLoading++;
	mutex->unlock();
}

int Gui::documentFinished(){
	mutex->lock();
	documentsLoading--;
	mutex->unlock();
	return documentsLoading;
}

void Gui::addDocument(Document *document){
	doc->addDocument(document);
	preview->renderDocument();
	
	listWidget->addItem(*document->getFileName());
	dock->setEnabled(true);
	setActionEnabled(true);
	
	if(documentFinished() == 0){
		progress->hide();
	}
	
	show();
	//mutex->unlock();
}

void Gui::addDocument(QString *file, bool wait = true){
	progress->show();
	progress->setMinimum(0);
	progress->setMaximum(0);
	if(wait){
		QFileInfo infoFile(*file);
	    trayIcon->showMessage("Loading new printjob", infoFile.fileName(), QSystemTrayIcon::Information,
	                           4000);
	}
	documentLoads();
	renderer = new DocumentRenderer(*file, wait);
	connect(renderer, SIGNAL(finished(Document *)),
			this, SLOT(addDocument(Document *)));
	renderer->start();
	delete file;
}

void Gui::deleteDocument(){
	QList<QListWidgetItem*> items = listWidget->selectedItems();
	if(items.size() > 0){
		doc->removeDocument(listWidget->row(items.at(0)));
		delete listWidget->takeItem(listWidget->row(items.at(0)));
		preview->renderDocument();
	}
	
	if(doc->realSize() == 0){
		dock->setEnabled(false);
		setActionEnabled(false);
	}
}

void Gui::moveUpDocument(){
	QList<QListWidgetItem*> items = listWidget->selectedItems();
	if(items.size() > 0){
		int index = listWidget->row(items.at(0));
		if(index > 0){
			doc->moveUp(index);
			
			QListWidgetItem *item = listWidget->takeItem(index);
			listWidget->insertItem(index-1,item);
			listWidget->setCurrentItem(item);
			preview->renderDocument();
		}
	}
}

void Gui::moveDownDocument(){
	QList<QListWidgetItem*> items = listWidget->selectedItems();
	if(items.size() > 0){
		int index = listWidget->row(items.at(0));
		if(index < doc->documents()-1){
			doc->moveDown(index);
			
			QListWidgetItem *item = listWidget->takeItem(index);
			listWidget->insertItem(index+1,item);
			listWidget->setCurrentItem(item);
			preview->renderDocument();
		}
	}
}

void Gui::add(){
	QFileDialog::Options options;
	QString selectedFilter;
	QString fileName = QFileDialog::getOpenFileName(this,
	                             tr("Open file"),
	                             defaultOpenPath,
	tr("PostScript Files and PDF Files (*.ps *.pdf)"),
	                            &selectedFilter,
	                             options);
	if (!fileName.isEmpty()){
		QFileInfo info(fileName);
		defaultOpenPath = info.path();
				
		QString * fileNameTmp = new QString(fileName);
		addDocument(fileNameTmp, false);
	}
}

void Gui::exportFinished(){
	progressDialog->hide();
		
	delete progressDialog;
	
	//delete creator;
}

void Gui::exportPDF(){
	QFileDialog::Options options;
	QString selectedFilter;
	QString fileName = QFileDialog::getSaveFileName(this,
	                             tr("Export to PDF"),
	                             QDir::homePath(),
								tr("PDF Files (*.pdf)"),
	                            &selectedFilter,
	                             options);
	if (!fileName.isEmpty()){
		if(!fileName.endsWith(".pdf")){
			fileName+=".pdf";
		}
		creator = new DocumentCreator(doc,fileName);
		creator->setPDF(true);
		connect(creator, SIGNAL(done()), this, SLOT(exportFinished()));
		
		progressDialog = new QProgressDialog("Export to PDF",0,0,0,this,Qt::Dialog);
		progressDialog->show();
		creator->start();
	}
}

void Gui::exportPS(){
	QFileDialog::Options options;
	QString selectedFilter;
	QString fileName = QFileDialog::getSaveFileName(this,
	                             tr("Export to PostScript"),
	                             QDir::homePath(),
								tr("PostScript Files (*.ps)"),
	                            &selectedFilter,
	                             options);
	
	if (!fileName.isEmpty()){
		if(!fileName.endsWith(".ps")){
			fileName+=".ps";
		}
		creator = new DocumentCreator(doc,fileName);
		connect(creator, SIGNAL(done()), this, SLOT(exportFinished()));
		
		progressDialog = new QProgressDialog("Export to PostScript",0,0,0,this,Qt::Dialog);
		progressDialog->show();
		creator->start();
	}
}

void Gui::print(){
	PrintDialog dialog(doc);
	dialog.exec();
	
	Config *conf = Config::instance();
	if(conf->getDefaultDeleting()){
		listWidget->clear();
	}
	//TODO:FIX
	if(doc->realSize() == 0){
		dock->setEnabled(false);
		setActionEnabled(false);
	}
	
	preview->renderDocument();
}

void Gui::layoutChanged(int id){
	doc->setNup(id);
	preview->renderDocument();
}

void Gui::borderChanged(int border){
	doc->setBorder(border);
	preview->renderDocument();
}

void Gui::marginChanged(int margin){
	doc->setMargin(margin);
	preview->renderDocument();
}

void Gui::configure(){
	ConfigureWindow *configure = new ConfigureWindow();
	configure->exec();
	delete configure;
}

void Gui::iconActivated(QSystemTrayIcon::ActivationReason reason){
	if(reason == QSystemTrayIcon::DoubleClick){
		if(!isVisible()){
			show();
		}else{
			hide();
		}
	}
}

void Gui::seperateJobs(int state){
	doc->setSeperateJobs(state == Qt::Checked);
	if(doc->documents() > 1){
		preview->renderDocument();
	}
}

void Gui::bookletChanged(int state){
	doc->setBooklet(state == Qt::Checked);
	
	if(state == Qt::Checked){
		n8up->setEnabled(false);
		if(doc->getNup() > 4){
			n4up->setChecked(true);
			layoutChanged(4);
		}
	}else{
		n8up->setEnabled(true);
	}	
}

void Gui::showDeletedPages(int state){
	doc->setShowDeleted(state == Qt::Checked);
	preview->renderDocument();
}

void Gui::closeEvent ( QCloseEvent * event ){
	if(trayIcon->isVisible()){
		event->ignore();
		hide();
	}else{
		event->accept();
		end();
	}
}

void Gui::end(){
	delete doc;
	qApp->quit();
}

void Gui::zoomIn(){
	zoomOutAction->setEnabled(true);
	
	if(zoomlevel == 0){
		zoomlevel--;
		zoomInAction->setEnabled(false);
		preview->setZoomLevel(zoomlevel);
		preview->renderDocument();
	}else if(zoomlevel == 1){
		zoomlevel--;
		preview->setZoomLevel(zoomlevel);
		preview->resetZoom();
	}else if(zoomlevel == 2){
		zoomlevel--;
		preview->setZoomLevel(zoomlevel);
		preview->scale(2,2);
	}
}

void Gui::zoomOut(){
	zoomInAction->setEnabled(true);
	
	if(zoomlevel == -1){
		zoomlevel++;
		preview->setZoomLevel(zoomlevel);
		preview->resetZoom();
		preview->renderDocument();
	}else if(zoomlevel == 0){
		zoomlevel++;
		preview->setZoomLevel(zoomlevel);
		preview->scale(0.5,0.5);
	}else if(zoomlevel == 1){
		zoomlevel++;
		preview->setZoomLevel(zoomlevel);
		preview->scale(0.5,0.5);
		zoomOutAction->setEnabled(false);
	}
}

void Gui::documentUpdated(){
	pageNumber->setText("Pages: "+QString::number(doc->getPageCount()));
}

void Gui::aboutDialog(){
	QMessageBox::information(this, "Printruler",
	                   "Printruler version \n $Header: /home/cvs/FPL/CVSROOT/src/src/Gui.cpp,v 1.36 2008/06/15 19:20:48 wolfb05 Exp $\nLicence: GPLv3",
	                   QMessageBox::Ok,
	                   QMessageBox::Ok); 
}
