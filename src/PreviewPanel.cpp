#include "PreviewPanel.h"
#include "Page.h"

PreviewPanel::PreviewPanel(QWidget * parent)
{
	setParent(parent);
	
	sc = new QGraphicsScene();
	sc->setBackgroundBrush(Qt::gray);
	setScene(sc);
	scale(DEFAULT_ZOOM,DEFAULT_ZOOM);
	zoomLevel = 0;
}

void PreviewPanel::renderDocument(){
	//benchmark only
	//QTime start = QTime::currentTime();
	
	int i = 0;
	QList<QGraphicsItem*> items = sc->items();
	for(i = 0; i < items.size();i++){
		sc->removeItem(items.at(i));
		delete items.at(i);
	}
	
	int pageDistance,rows,collums;
	int interPageDistance = PAGE_DISTANCE;
	int x = X_START;
	int y = Y_START;
	
	int margin;
	switch(doc->getMargin()){
		case 1:
			margin = MARGIN_SMALL;
			break;
		case 2:
			margin = MARGIN_BIG;
			break;
		default:
			margin = NO_MARGIN;
	}
	
	switch(doc->getNup()){
		case 1:{
			rows = 1;
			collums = 1;
			pageDistance = INTERPAGE_DISTANCE_BIG;
		}break;
		case 2:{
			rows = 1;
			collums = 2;
			pageDistance = INTERPAGE_DISTANCE_BIG;
		}break;
		case 4:{
			rows = 2;
			collums = 2;
			pageDistance = INTERPAGE_DISTANCE_SMALL;
		}break;
		case 8:{
			rows = 2;
			collums = 4;
			pageDistance = INTERPAGE_DISTANCE_SMALL;
		}break;
	}
	
	int width = 1;
	int height = 1;
	
	for(i = 0;i < doc->size();i++){
		QPixmap tempPic;
		
		if(doc->getNup() > 2 && zoomLevel >= 0){
			tempPic = doc->getSmallPage(i);
		}else if(doc->getNup() <= 2 && zoomLevel < 0){
			tempPic = doc->getBigPage(i);
		}else{
			tempPic = doc->getPage(i);
		}
		
		QPixmap pix(1,1);
		
		if(tempPic.width() != 0){
			pix = tempPic;
			width = pix.width() + 2 * margin;
			height = pix.height() + 2 * margin;
		}
		
		if(tempPic.width() != 0){
			Page *p = new Page(&pix, i, doc->isShowDeleted() && doc->isDeleted(i) );
			connect(p, SIGNAL(pageDeleted(int)), this, SLOT(pageDeleted(int)));
			connect(p, SIGNAL(pageUndeleted(int)), this, SLOT(pageUndeleted(int)));
			connect(p, SIGNAL(addBlankPage(int,bool)), this, SLOT(addBlankPage(int,bool)));
			p->setZValue(10);
			sc->addItem(p);
			
			p->setPos(x+margin,y+margin);
			
			//add border
			if(doc->getBorder()){
				QGraphicsRectItem *border = 
					new QGraphicsRectItem(p->x()-doc->getBorder(),p->y()-doc->getBorder(),
							pix.width()+2*doc->getBorder(),
							pix.height()+2*doc->getBorder());
				
				QPen pen(Qt::black, doc->getBorder(), Qt::SolidLine);
				
				border->setPen(pen);
				border->setBrush(Qt::transparent);
				border->setZValue(5);
				sc->addItem(border);
			}
		}
		
		//add background page
		if(i % (rows * collums) == 0){
			QGraphicsRectItem *background = 
				new QGraphicsRectItem(x-BACKGROUNDPAGE_BORDER,y-BACKGROUNDPAGE_BORDER, 
						width * collums + 2*BACKGROUNDPAGE_BORDER 
						+ pageDistance*(collums-1), 
						height * rows + 2*BACKGROUNDPAGE_BORDER 
						+ pageDistance*(rows-1));
			//background->setPen(Qt::);
			background->setBrush(Qt::white);
			background->setZValue(-10);
			sc->addItem(background);
		}
		
		if((i+1) % collums != 0){
			x += width+pageDistance;
		}else{
			x = X_START;
			y += height + pageDistance;
		}
		if((i+1)% (rows * collums) == 0){
			y+= interPageDistance;
		}
	}
	
	emit documentRendered();
	//Benchmark only
	//qDebug() << "rendered in: " << start.elapsed();
}

void PreviewPanel::setDocument(VirtualDocument *document){
	doc = document;
	renderDocument();
}

PreviewPanel::~PreviewPanel()
{
}


void PreviewPanel::pageDeleted(int page){
	doc->deletePage(page);
	renderDocument();
}

void PreviewPanel::pageUndeleted(int page){
	doc->undeletePage(page);
	renderDocument();
}

void PreviewPanel::addBlankPage(int page, bool before){
	doc->addBlankPage(page,before);
	renderDocument();
}

void PreviewPanel::setZoomLevel(int zoom){
	zoomLevel = zoom;
}

void PreviewPanel::resetZoom(){
	resetMatrix();
	scale(DEFAULT_ZOOM,DEFAULT_ZOOM);
}
