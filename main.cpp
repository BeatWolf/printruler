#include <QApplication>
#include <QColor>
#include "src/Gui.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
	
    Gui *gui = new Gui();
    gui->initGui();
    return a.exec();
}
