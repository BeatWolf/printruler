#ifndef PAGE_H_
#define PAGE_H_

#include <QtGui>
#include <QtCore>

/**
 * Page is the object put on the graphics view. It shows the thumbnail of the page
 * and can also mark a page as deleted. Different actions can be made on a page,
 * like deleting and undeleting.
 */
class Page: public QObject,public QGraphicsPixmapItem
{
	Q_OBJECT
public:
	/**
	 * pix is the thumbnail
	 * index the index of the page in the whole document
	 * if deleted is true, the page will draw a overlay to show its deleted status
	 */
	Page(QPixmap *pix, int index, bool deleted);
	void keyPressEvent ( QKeyEvent * event );
	virtual ~Page();
	void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	
signals:
	/**
	 * emitted when the page is deleted
	 */
	void pageDeleted(int page);
	
	/**
	 * emitted when the page is undeleted
	 */
	void pageUndeleted(int page);
	
	/**
	 * emitted when a blank page has been added at that position.
	 * Before indicates if the page should be added before or after the current page
	 */
	void addBlankPage(int page, bool before);
	
public slots:
	void pageDelete();
	void pageUndelete();
	void addBlank();
	void addBlankAfter();
private:
	int index;
	bool isDeleted;
};

#endif /*PAGE_H_*/
