#ifndef DOCUMENTRENDERER_H_
#define DOCUMENTRENDERER_H_
#include <QtCore>
#include "Document.h"

/**
 * This class renders constructs a Document. It is threaded
 * and emits a signal when finished
 */
class DocumentRenderer:public QThread
{
	Q_OBJECT
public:
	DocumentRenderer(QString &doc, bool wait);
	virtual ~DocumentRenderer();
	void run();
	
signals:
	void finished(Document *doc);
	
private:
	QString *file;
	bool m_wait;
};

#endif /*DOCUMENTRENDERER_H_*/
