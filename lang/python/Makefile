BASE_DIR   = ../..
SWIG       = swig
SYMBOLS    = -DUSE_LINUX
GCC        = g++
CPPFLAGS   = -shared -Wall -Wno-deprecated -pthread -fPIC
LIBS       = -L${BASE_DIR}/lib -lDragonfly
PY_INC     = /usr/include/python2.7
PYTHON     = /usr/bin/python
INCLUDES   = -I${BASE_DIR}/include -I${BASE_DIR}/include/internal
H2XML      = h2xml
XML2PY     = xml2py

all:  _PyDragonfly.so Dragonfly_types.py

PyDragonfly.py: PyDragonfly.i
	${SWIG} ${SYMBOLS} ${INCLUDES} -c++ -python PyDragonfly.i

_PyDragonfly.so:  PyDragonfly.py
	${GCC} ${CPPFLAGS} ${INCLUDES} ${SYMBOLS} -I${PY_INC} ${LIBS} PyDragonfly_wrap.cxx -o _PyDragonfly.so -lpython2.7 -lDragonfly

Dragonfly_types.xml: ${BASE_DIR}/include/Dragonfly_types.h
	${H2XML} -c -o Dragonfly_types.xml `pwd`/../../include/Dragonfly_types.h

Dragonfly_types.py: Dragonfly_types.xml
	${XML2PY} -o Dragonfly_types.py Dragonfly_types.xml

clean:
	rm -f *.so
	rm -f *.cxx
	rm -f PyDragonfly.py
	rm -f *.pyc
	rm -f Dragonfly_types.xml
	rm -f Dragonfly_types.py

