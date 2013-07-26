CPP = g++
CPPFLAGS = -g -DUSE_LINUX -Wall -Wno-deprecated -pthread

LIB_DIR = ../../../lib
INC_DIR = ../../../include
TM_DIR = .

Reply: ${TM_DIR}/Reply.cpp
	${CPP} ${CPPFLAGS} -L${LIB_DIR} -I${INC_DIR} -o ${TM_DIR}/Reply ${TM_DIR}/Reply.cpp -lDragonfly

Request: ${TM_DIR}/Request.cpp
	${CPP} ${CPPFLAGS} -L${LIB_DIR} -I${INC_DIR} -o ${TM_DIR}/Request ${TM_DIR}/Request.cpp -lDragonfly

all: Reply Request

clean:
	rm -f ${TM_DIR}/Reply
	rm -f ${TM_DIR}/Request

