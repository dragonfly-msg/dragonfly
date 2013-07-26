# defines
CPP = mex
INCLUDES  = -I../../../src/core -I.. -I../../../include -I../../../include/internal -I../../../lang/matlab/ -I../QuickLogger
REQS = LogReader.cpp
LIBPATH = -L../../../lib
LIBS = -lDragonfly
DEFINES =  -D_MEX_FILE_ -DUSE_LINUX
#rules
# this should depend on Dragonfly lib---
default: LogReader

LogReader: LogReader.cpp
	${CPP} ${DEFINES} ${INCLUDES} LogReader.cpp ../../../src/core/MyCException.cpp ../../../src/core/MyCString.cpp

clean:
	rm -rf *.o
	rm -rf *.mex*
