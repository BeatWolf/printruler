#ifndef PRINTEROPTIONS_H_
#define PRINTEROPTIONS_H_

#include <QtGui>

class PrinterOptions: public QDialog
{
	Q_OBJECT
public:
	PrinterOptions(QWidget * parent, QString printerName);
	virtual ~PrinterOptions();

	QString getPrinterName(){
		return printer;
	}
	
	QStringList getOptions();
	
public slots:
	void cancel();
	void save();
private:
	QString printer;
	QList <QString>optionList;
	QList <int>optionDefaults;
	QList <QComboBox *>optionValues;
};

#endif /*PRINTEROPTIONS_H_*/
