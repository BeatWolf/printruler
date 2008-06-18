#ifndef DOCUMENT_H_
#define DOCUMENT_H_

#include <QtGui>

#define SMALLER_FACTOR 0.3
#define MEDIUM_FACTOR 0.5

#define RESOLUTION "-r90" // "-r90"

/**
 * Document class. Contains one document
 */
class Document: public QObject
 {
     Q_OBJECT
     
public:
	/**
	 * Creates a document from a given file
	 * The file can be a postscript or a pdf file
	 */
	Document(QString *file, bool wait);
	~Document();
	
	/**
	 * Returns the size of the document. This includes blank pages.
	 * If the parameter real is given, the deleted pages are counted
	 */
	int getSize(bool real);
	
	/**
	 * Deletes a page with a given index.
	 * If real set to true, then the index contains the deleted pages
	 */
	void deletePage(int index,bool real);
	
	/**
	* Undeletes a page with a given index.
	* If real set to true, then the index contains the deleted pages
	*/
	void undeletePage(int index,bool real);
	
	/**
	 * Returns true if a given page is deleted
	 * Asumes real to be true
	 */
	bool isDeleted(int index);
	
	/**
	 * Adds a blank 
	 */
	void addBlankPage(int index, bool real);
	
	/**
	 * Deletes the file if it's in the watch folder or the temp folder
	 */
	void deleteFile();
	
	/**
	 * Returns a pixmap with the biggest size of a given index
	 */
	QPixmap getBigPage(int index,bool real);
	
	/**
	 * Returns a pixmap with the medium size of a given index
	 */
	QPixmap getPage(int index,bool real);
	
	/**
	 * Returns a pixmap with the smallest size of a given index
	 */
	QPixmap getSmallPage(int index,bool real);
	
	/**
	 * Returns the filename of that document.
	 * If the file was a pdf file, the name of the temporary
	 * postscript document is returned
	 */
	QString *getFileName();
	
	/**
	 * Returns the full path and filename of that document.
	 * If the file was a pdf file, the name of the temporary
	 * postscript document is returned
	 */
	QString *getFile();
	
	/**
	 * Returns the index that this page has in the source file.
	 * Returns -1 if the page is blank, or -2 if the page is invalid
	 */
	int getRealIndex(int index);
	
private:
	int realIndex(int index, bool real);
	int blankIndex(int index);
	void renderFile(QString *documentfile);
	
	QString baseFile;
	QString file;
	QString fileName;
	QList<QImage*> *cache;
	QPixmap *cachePixmaps;
	QPixmap *cachePixmapsMedium;
	QPixmap *cachePixmapsSmall;
	
	QList<int> *deleted;
	QList<int> *blank;
	int pages;
};

#endif /*DOCUMENT_H_*/
