#PipeLib Makefile for linux
#Defines
CC = gcc
CPP = g++
CCFLAGS = -DUSE_LINUX -Wall -Wno-deprecated
CPPFLAGS = -DUSE_LINUX -Wall -Wno-deprecated
LIB_DIR = ../../Util
PIPELIB_DIR = ../PipeLib

#OBJects
OBJS = MyCString.o MyCException.o UPipe.o SocketPipe.o

#Rules
all: TestPipeServer TestPipeClient

TestPipeServer: $(OBJS) TestPipeServer.cpp
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) TestPipeServer.cpp

TestPipeClient: $(OBJS) TestPipeClient.cpp
	$(CPP) $(CPPFLAGS) -o $@ $(OBJS) TestPipeClient.cpp

#OBJ files

UPipe.o: $(PIPELIB_DIR)/UPipe.cpp $(PIPELIB_DIR)/UPipe.h
	$(CPP) $(CPPFLAGS) -c $(PIPELIB_DIR)/UPipe.cpp

UnixNamedPipe.o: $(PIPELIB_DIR)/UnixNamedPipe.cpp $(PIPELIB_DIR)/UnixNamedPipe.h
	$(CPP) $(CPPFLAGS) -c $(PIPELIB_DIR)/UnixNamedPipe.cpp

SocketPipe.o: $(PIPELIB_DIR)/SocketPipe.cpp $(PIPELIB_DIR)/SocketPipe.h
	$(CPP) $(CPPFLAGS) -c $(PIPELIB_DIR)/SocketPipe.cpp

MyCException.o: $(LIB_DIR)/MyCException.cpp $(LIB_DIR)/MyCException.h
	$(CPP) $(CPPFLAGS) -c $(LIB_DIR)/MyCException.cpp

MyCString.o: $(LIB_DIR)/MyCString.cpp $(LIB_DIR)/MyCString.h
	$(CPP) $(CPPFLAGS) -c $(LIB_DIR)/MyCString.cpp

PipeIO.o: $(LIB_DIR)/PipeIO.c $(LIB_DIR)/PipeIO.h
	$(CC) $(CCFLAGS) -c $(LIB_DIR)/PipeIO.c

NetworkLayer.o: $(LIB_DIR)/NetworkLayer.c $(LIB_DIR)/NetworkLayer.h
	$(CC) $(CCFLAGS) -c $(LIB_DIR)/NetworkLayer.c

clean:
	rm -rf *.o
	rm -f TestPipeServer
	rm -f TestPipeClient
