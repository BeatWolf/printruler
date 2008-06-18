#ifndef PRINTDIALOG_H_
#define PRINTDIALOG_H_

#include <QtGui>
#include "DocumentCreator.h"
#include "VirtualDocument.h"
#include "ActionScript.h"
#include "Config.h"
#include "PrinterOptions.h"

/**
 * This class shows the printer dialog and then prints the virtual document
 */
class PrintDialog: public QDialog
{
	Q_OBJECT
public:
	PrintDialog(VirtualDocument *document);
	virtual ~PrintDialog();
	void fillPrintChooser();
	
public slots:
	void cancel();
	void print();
	void sendToPrinter();
	void properties();
	void manualDuplexChanged(int state);
	
private:
	Config *config;
	
	bool firstSide;
	
	VirtualDocument *doc;
	QComboBox *printerChooser;
	DocumentCreator *creator;
	QString fileName;
	QProgressDialog *progress;
	
	void saveConfig();
		
	QCheckBox *deleteAfter;
	QCheckBox *manualDuplex;
	QCheckBox *rotateEven;
	
	QSpinBox *spinBox;
	
	PrinterOptions *options;
};

#endif /*PRINTDIALOG_H_*/
