#ifndef ACTIONSCRIPT_H_
#define ACTIONSCRIPT_H_

#include <QtCore>
#include "VirtualDocument.h"

class ActionScript : public QObject
{
	Q_OBJECT
public:
	ActionScript(VirtualDocument *vd, QString destination);
	virtual ~ActionScript();
	void run();
private:
	VirtualDocument *vd;
	QString destination;
	QList<QString> *listTmpDocuments;
private slots:
	void launchMerge();
	void launchMarginBorderNup(QString src);
	void launchDeleteBlank(int current);
	void launchBooklet();
};

#endif /*ACTIONSCRIPT_H_*/
