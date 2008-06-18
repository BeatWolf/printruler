#ifndef VIRTUALDOCUMENT_H_
#define VIRTUALDOCUMENT_H_


#include <QList>
#include <QPixmap>

#include "Document.h"
struct ExactPage{
	Document *doc;
	int index;
};

class VirtualDocument
{
public:
	VirtualDocument();
	virtual ~VirtualDocument();
	
	void removeDocument(int index); //removed document from VD and deletes file
	
	void moveUp(int index);
	void moveDown(int index);
	
	int size(); //number of subpages
	int realSize(); //real size, with deleted pages
	int getPageCount();//number of "real" pages in that document
	int documents();//number of documents in this virtual document
	
	QString getPageSource(int i);
	int getDocumentSize(int index);
	
	QPixmap getPage(int index);
	QPixmap getSmallPage(int index);
	QPixmap getBigPage(int index);
	
	void setNup(int pages);
	int getNup();
	
	void setBorder(int style);
	int getBorder();
	
	void setMargin(int style);
	int getMargin();
	
	void addDocument(Document *document);
	
	void setSeperateJobs(bool seperate);
	bool isSeperateJobs();
	
	void deletePage(int index);
	void addBlankPage(int index,bool before = true);
	void undeletePage(int index);
	void setShowDeleted(bool show);
	bool isShowDeleted();
	
	bool isDeleted(int index);
	
	bool isBooklet();
	void setBooklet(bool book);
	int getRealIndex(int doc, int page);
private:
	int nup;
	int border;
	int margin;
	QList<Document *> *list;
	bool seperateJobs;
	bool showDeleted;
	bool booklet;
	ExactPage *getExactPage(int index);
};


#endif /*VIRTUALDOCUMENT_H_*/
