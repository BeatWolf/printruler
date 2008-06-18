#ifndef CONFIG_H_
#define CONFIG_H_

#include <QtCore>
#include <QtXml>

#define DEFAULT_TEMPFOLDER "/tmp/printruler/"
#define DEFAULT_WATCHFOLDER "/PDF/" //relative to ~
#define DEFAULT_PRINTERNAME ""
#define DEFAULT_PRINTER ""
#define DEFAULT_DELETE false
#define DEFAULT_ROTATE false
#define DEFAULT_SIDES 0


/**
 * Singleton class that holds the configuration of the application
 */
class Config: public QObject
{
	Q_OBJECT
public:
	/**
	 * returns a singleton instance of that class
	 */
	static Config *instance()
	{
	  if (m_instance == 0)
		  m_instance = new Config();
	
	  return (m_instance);
	}
	
	/**
	 * Sets the folder that has to be watched for new files
	 */
	void setWatchFolder(QString &folder);
	
	/**
	 * Sets the temporary folder. This is where all temporary files are created
	 */
	void setTempFolder(QString &folder);
	
	/**
	 * Sets the temporary folder. This is where all temporary files are created
	 */
	void setDefaultPrinter(QString printer);
	
	/**
	 * Returns the watch folder. See setWatchFolder
	 */
	QString *getWatchFolder();
	
	/**
	 * Returns the temporary folder. See setTempFolder
	 */
	QString *getTempFolder();
	
	/**
	 * Returns the temporary folder. See setTempFolder
	 */
	QString *getPrinterName();
	
	/**
	 * Returns the temporary folder. See setTempFolder
	 */
	QString *getDefaultPrinter();
	
	/**
	 * returns the folder in which de icons are
	 */
	QString getIconFolder();
	
	/**
	 * returns true if by default the document should be printed
	 * in colors
	 */
	bool isRotateEven();
	
	void setRotateEven(bool rotate);
	
	/**
	 * Returns true if the print jobs should be deleted after
	 * printing
	 */
	bool getDefaultDeleting();
	
	void setDefaultDeleting(bool deleting);
	
	/*
	 * Returns 0 if no duplex is used
	 */
	int getSides();
	
	void setSides(int s);
	
	void saveConfigFile();
	
signals:
	void watchFolderChanged(QString folder);
	
private:
	Config();
	static Config * m_instance;
	QString *watchFolder;
	QString *tmpFolder;
	QString *configFile;
	QString *printerName;
	QString *defaultPrinter;
	
	bool deleteJobsAfterPrinting;
	bool rotateEven;
	
	int sides;
	
	void loadDefault();
	void loadConfigFile();
};

#endif /*CONFIG_H_*/
