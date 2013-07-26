PIPE_DIR=PipeLib
BASE_DIR=../..
SOURCES=Dragonfly.cpp MyCException.cpp MyCString.cpp $$PIPE_DIR/SocketPipe.cpp $$PIPE_DIR/UPipe.cpp $$PIPE_DIR/Timing.cpp
;HEADERS=$$BASE_DIR/include/Dragonfly.h $$BASE_DIR/include/MyCException.h $$BASE_DIR/include/MyCString.h $$BASE_DIR/include/internal/UPipe.h
INCLUDEPATH += $$BASE_DIR/include $$BASE_DIR/include/internal


CONFIG += release thread warn_on dll
CONFIG -= qt
TARGET = Dragonfly

unix {
UI_DIR = .ui
MOC_DIR = .moc
OBJECTS_DIR = .obj
DEFINES += USE_LINUX
TEMPLATE = lib
DESTDIR=$$BASE_DIR/lib
}

win32 {
TEMPLATE = lib
CONFIG += console
DEFINES -= UNICODE
DESTDIR = $$BASE_DIR/lib
}
