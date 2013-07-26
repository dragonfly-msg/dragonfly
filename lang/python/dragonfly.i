// This is the SWIG interface file for the fundamental Dragonfly data types. The
// primary function is to generate wrappers for the message data structures, so
// that they may be imported into Python.
// Andrew S. Whitford 06/09

%module Dragonfly_Definitions
%{
#include "../../include/Dragonfly_types.h"
%}

%include "../../include/Dragonfly_types.h"

// Helper classes required for variable-length data.
%include "carrays.i"
%array_class(double, DoubleArray)
%array_class(unsigned char, UcharArray)

//swig -includeall -c++ -python dragonfly.i
