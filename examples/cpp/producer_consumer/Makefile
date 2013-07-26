CPP = g++
CPPFLAGS = -g -DUSE_LINUX -Wall -Wno-deprecated -pthread

LIB_DIR = ../../../lib
INC_DIR = ../../../include
TM_DIR = .

Consumer: ${TM_DIR}/Consumer.cpp
	${CPP} ${CPPFLAGS} -L${LIB_DIR} -I${INC_DIR} -o ${TM_DIR}/Consumer ${TM_DIR}/Consumer.cpp -lDragonfly

Producer: ${TM_DIR}/Producer.cpp
	${CPP} ${CPPFLAGS} -L${LIB_DIR} -I${INC_DIR} -o ${TM_DIR}/Producer ${TM_DIR}/Producer.cpp -lDragonfly

all: Consumer Producer

clean:
	rm -f ${TM_DIR}/Consumer
	rm -f ${TM_DIR}/Producer

