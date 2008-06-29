#ifndef CONFIGUREWINDOW_H_
#define CONFIGUREWINDOW_H_

#include <QtGui>
#include "Config.h"

class ConfigureWindow: public QDialog
{
	Q_OBJECT
public:
	ConfigureWindow();
	virtual ~ConfigureWindow();
public slots:
	void changeWatchFolder();
	void save();
	void cancel();
	
private:
	QGridLayout *layout;
	Config *conf;
	QLineEdit *checkFolder;
	QCheckBox *showOnOpen;
};

#endif /*CONFIGUREWINDOW_H_*/
