BASE_DIR = ../../..
SOURCES +=  MessageManager.cpp

HEADERS +=  MessageManager.h
LIBS += -L$$BASE_DIR/lib -lDragonfly 
TEMPLATE = app

INCLUDEPATH += $$BASE_DIR/include $$BASE_DIR/include/internal
unix {
  UI_DIR = .ui
  MOC_DIR = .moc
  OBJECTS_DIR = .obj
  DEFINES += USE_LINUX
}

win32 {
	DEFINES += USE_WINDOWS
	TEMPLATE = vcapp
    CONFIG += console
}


TARGET = $$BASE_DIR/bin/MessageManager

CONFIG += release warn_off  thread embed_manifest_exe
CONFIG -= qt x11

