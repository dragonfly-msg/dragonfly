BASE_DIR = ../../..
SOURCES +=  QuickLogger.cpp

HEADERS += MessageBufferer.h \
MessageBuffer.h \ 
QuickLogger.h

INCLUDEPATH += $$BASE_DIR/include $$BASE_DIR/include/internal
LIBS += -L$$BASE_DIR/lib -lDragonfly 
TEMPLATE = app

unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  DEFINES += USE_LINUX
}
win32 {
	TEMPLATE = vcapp
    CONFIG += console
}


TARGET = $$BASE_DIR/bin/QuickLogger

CONFIG += release warn_off  thread embed_manifest_exe
CONFIG -= qt
#it's no use, it wants x11 CONFIG -= x11
