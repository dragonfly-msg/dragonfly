// This is the SWIG interface file for generating structures that will enable
// one to use the Dragonfly application classes/interface from Python.
// Andrew S. Whitford (asw35) 01/09, 06/09, 09/09
// Angus McMorland
// Emrah Diril 05/01/2013

%define DOCSTRING
"Python interface to the University of Pittsburgh Motor Control Lab's Dragonfly messaging system."
%enddef

%module(docstring=DOCSTRING) PyDragonfly
%feature("autodoc", "1");
%{

#include "MyCString.h"
#include "Dragonfly.h"
%}

%exception {
	try {
		$action
	}
	catch (MyCException e) {
	    MyCString errMsg;
		
		e.AppendTraceToString(errMsg);
		PyErr_SetString(PyExc_Exception, errMsg.GetContent());
		return NULL;
	}
}

%inline %{
	void *void_ptr(long long n)
	{
		return (void *) n;
	}
%}

%pythoncode %{
	from ctypes import memmove, addressof, sizeof

	def copy_from_msg(data, cmsg):
		#memmove(addressof(data), cmsg.data, sizeof(data))
		cmsg.GetData(void_ptr(addressof(data)))

	def copy_to_msg(data, cmsg):
		cmsg.SetData(void_ptr(addressof(data)), sizeof(data))

%}

%include "Dragonfly.h"

%include "Dragonfly_types.h"

// Helper classes for working with subscription lists and variable-length data
// types.
//%include "carrays.i"
//%array_class(int, MSG_TYPE_ARRAY)
//%array_class(double, DoubleArray)

