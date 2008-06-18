#ifndef GUI_H_
#define GUI_H_

#include <QString>
#include "VirtualDocument.h"
#include "PreviewPanel.h"
#include "WatchDog.h"
#include "Config.h"
#include "DocumentRenderer.h"

#include "ActionScript.h"
#include "DocumentCreator.h"

#define DEFAULT_SIZE_X 800
#define DEFAULT_SIZE_Y 600

class Gui : public QMainWindow
 {
     Q_OBJECT
public:
	Gui();
	~Gui();
	void initGui();
	void closeEvent ( QCloseEvent * event );
	void documentLoads();
	int documentFinished();
	
	void setActionEnabled(bool enable);
	
	VirtualDocument *doc;
	PreviewPanel *preview;
	QListWidget *listWidget;
	QDockWidget *dock;
	WatchDog *watch;
	
	Config *conf;
	QMutex *mutex;
	DocumentRenderer *renderer;
	
	QStatusBar *statusBar;
	QProgressBar *progress;
	
	QAction *zoomInAction;
	QAction *zoomOutAction;
	QAction *addDocumentAction;
	QAction *printDocument;
	QAction *exportPDFAction;
	QAction *exportPSAction;
	QAction *aboutAction;
	
	QLabel *pageNumber;
	int zoomlevel;
	QSystemTrayIcon *trayIcon;
	QButtonGroup *group;
	QRadioButton *n4up;
	QRadioButton *n8up;
	
public slots:
	void print();
	void add();
	void deleteDocument();
	void moveUpDocument();
	void moveDownDocument();
	void layoutChanged(int id);
	void borderChanged(int border);
	void marginChanged(int margin);
	void configure();
	void addDocument(QString *file, bool wait);
	void iconActivated(QSystemTrayIcon::ActivationReason reason);
	void seperateJobs(int state);
	void showDeletedPages(int state);
	void addDocument(Document *document);
	void end();
	void zoomIn();
	void zoomOut();
	void documentUpdated();
	
	void exportFinished();
	
	void bookletChanged(int state);
	
	void exportPDF(); //exports the current document as a pdf file
	void exportPS(); //exports the current document as a ps file
	void aboutDialog(); //shows the about dialog
	
private:
	/**
	 * Tests if all required applications are present,
	 * and gives a error if not.
	 */
	void testDependencies();

	QProgressDialog *progressDialog; //progress bar while exporting
	DocumentCreator *creator; // used while exporting
	int documentsLoading; //number of documents loading
	QString iconPath;
	
	QString defaultOpenPath;
};

#endif /*GUI_H_*/
