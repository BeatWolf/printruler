#include "Page.h"

Page::Page(QPixmap *pix,int i, bool deleted)
{
	QPixmap *temp= new QPixmap(*pix);
	if(deleted){
		QPainter p(temp);
		p.fillRect(temp->rect(), QColor(255, 0, 0, 80));
		p.end();
	}
	setPixmap(*temp);
	delete temp;
	setFlags(QGraphicsItem::ItemIsSelectable | QGraphicsItem::ItemIsFocusable);
	index = i;
	isDeleted = deleted;
}

Page::~Page()
{
}

void Page::keyPressEvent ( QKeyEvent * event ){
	if(event->key() ==   Qt::Key_Delete){
		if(isDeleted){
			emit pageUndeleted(index);
		}else{
			emit pageDeleted(index);
		}
	}
}

void Page::pageDelete(){
	emit pageDeleted(index);
}

void Page::pageUndelete(){
	emit pageUndeleted(index);
}

void Page::addBlank(){
	emit addBlankPage(index,true);
}

void Page::addBlankAfter(){
	emit addBlankPage(index+1,false);
}

void Page::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;
    QAction *action;
    
    if(isDeleted){
    	action = new QAction("undelete",this);
    	menu.addAction(action);
    	connect(action, SIGNAL(triggered()), this, SLOT(pageUndelete()));
    }else{
    	action = new QAction("delete",this);
    	menu.addAction(action);
    	connect(action, SIGNAL(triggered()), this, SLOT(pageDelete()));
    }
    
    QAction *actionAddBlank = new QAction("Add blank page before",this);
    menu.addAction(actionAddBlank);
	connect(actionAddBlank, SIGNAL(triggered()), this, SLOT(addBlank()));
	
	QAction *actionAddBlankAfter = new QAction("Add blank page after",this);
    menu.addAction(actionAddBlankAfter);
	connect(actionAddBlankAfter, SIGNAL(triggered()), this, SLOT(addBlankAfter()));
    
    menu.exec(event->screenPos());
    //delete action;
   // delete actionAddBlank;
}
