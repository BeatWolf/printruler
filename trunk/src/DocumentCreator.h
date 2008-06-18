#ifndef DOCUMENTCREATOR_H_
#define DOCUMENTCREATOR_H_

#include <QtCore>
#include "ActionScript.h"
#include "VirtualDocument.h"

class DocumentCreator: public QThread
{
	Q_OBJECT
public:
	DocumentCreator(VirtualDocument *doc, QString fileName);
	virtual ~DocumentCreator();
	void run();
	void setPDF(bool ispdf);
	void psToPdf(QString ps, QString pdf);
	void pdfToPs(QString ps, QString pdf);
	void setRotateEven(bool rot){
		rotateEven = rot;
	}
	
signals:
	void done();
private:
	VirtualDocument *virtDoc;
	QString file;
	bool pdf;
	bool rotateEven;
};

#endif /*DOCUMENTCREATOR_H_*/
