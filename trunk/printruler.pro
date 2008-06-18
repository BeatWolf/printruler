TEMPLATE = app
TARGET = printruler
QT += core \
    gui \
    xml
HEADERS += src/PrinterOptions.h \
    src/ActionScript.h \
    src/Config.h \
    src/ConfigureWindow.h \
    src/Document.h \
    src/DocumentCreator.h \
    src/DocumentRenderer.h \
    src/Gui.h \
    src/Page.h \
    src/PreviewPanel.h \
    src/PrintDialog.h \
    src/VirtualDocument.h \
    src/WatchDog.h
SOURCES += src/PrinterOptions.cpp \
    main.cpp \
    src/ActionScript.cpp \
    src/Config.cpp \
    src/ConfigureWindow.cpp \
    src/Document.cpp \
    src/DocumentCreator.cpp \
    src/DocumentRenderer.cpp \
    src/Gui.cpp \
    src/Page.cpp \
    src/PreviewPanel.cpp \
    src/PrintDialog.cpp \
    src/VirtualDocument.cpp \
    src/WatchDog.cpp
FORMS += 
RESOURCES += 
icons.files += arrow-down.png \
    document-print.png \
    list-add.png \
    zoom-in.png \
    arrow-up.png \
    edit-delete.png \
    logo.png \
    zoom-out.png
icons.path = /usr/share/printruler/
target.path = /usr/bin/
INSTALLS += target \
    icons
CONFIG+=debug_and_release
CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT
