CONFIG      += designer plugin release
TEMPLATE    = lib
DESTDIR     = $$[QT_INSTALL_PLUGINS]/designer



INCLUDEPATH += .

# Input
HEADERS += compass.h compassplugin.h
SOURCES += compass.cpp compassplugin.cpp

target.path = $$[QT_INSTALL_PLUGINS]/designer
INSTALLS += target
