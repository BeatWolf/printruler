#ifndef PREVIEWPANEL_H_
#define PREVIEWPANEL_H_

#include <QtGui>
#include <QtCore>

#include "VirtualDocument.h"


//default zoom at the programm start
#define DEFAULT_ZOOM 1.0

//coordinates of the first page
#define X_START 50 
#define Y_START 50
#define PAGE_DISTANCE 25

//margin size in pixels
#define NO_MARGIN 0
#define MARGIN_SMALL 5
#define MARGIN_BIG 10

//pages distance on the same page
#define INTERPAGE_DISTANCE_SMALL 2
#define INTERPAGE_DISTANCE_BIG 5

//Background page size
#define BACKGROUNDPAGE_BORDER 3

/**
 * This class renders the preview in the GUI
 */
class PreviewPanel : public QGraphicsView
{
	Q_OBJECT
public:
	PreviewPanel(QWidget * parent);
	virtual ~PreviewPanel();
	
	/**
	 * sets the document to be rendered
	 */
	void setDocument(VirtualDocument *document);
	
	/**
	 * renders the document currently set. to be called when
	 * a change occurs to the document
	 */
	void renderDocument();
	
	/**
	 * Returns the current document of the preview panel
	 */
	VirtualDocument *document(){
		return doc;
	}
	
public slots:
	/*
	 * Called when a page is deleted from the GUI.
	 * Deletes the page in the virtual document and renders the preview
	 */
	void pageDeleted(int page);
	
	/*
	 * Called when a page is undeleted from the GUI.
	 * Undeletes the page in the virtual document and renders the preview
	 */
	void pageUndeleted(int page);
	
	/**
	 * Adds a blank page at the position of that page
	 */
	void addBlankPage(int page, bool before);
	
	void setZoomLevel(int zoom);
	void resetZoom();
	
signals:
	void documentRendered();
	
private:
	//the current document that is rendered
	VirtualDocument *doc;
	QGraphicsScene *sc;

	int zoomLevel;
};

#endif /*PREVIEWPANEL_H_*/
