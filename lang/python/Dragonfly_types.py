'''Wrapper for Dragonfly_types.h

Generated with:
C:\Users\emd\Documents\_apps_\ctypesgen\ctypesgen.py --includedir="../include" -a -o Dragonfly_types.py C:\Users\emd\Documents\dragonfly\include\Dragonfly_types.h

Do not modify this file.
'''

__docformat__ =  'restructuredtext'

# Begin preamble

import ctypes, os, sys
from ctypes import *

_int_types = (c_int16, c_int32)
if hasattr(ctypes, 'c_int64'):
    # Some builds of ctypes apparently do not have c_int64
    # defined; it's a pretty good bet that these builds do not
    # have 64-bit pointers.
    _int_types += (c_int64,)
for t in _int_types:
    if sizeof(t) == sizeof(c_size_t):
        c_ptrdiff_t = t
del t
del _int_types

class c_void(Structure):
    # c_void_p is a buggy return type, converting to int, so
    # POINTER(None) == c_void_p is actually written as
    # POINTER(c_void), so it can be treated as a real pointer.
    _fields_ = [('dummy', c_int)]

def POINTER(obj):
    p = ctypes.POINTER(obj)

    # Convert None to a real NULL pointer to work around bugs
    # in how ctypes handles None on 64-bit platforms
    if not isinstance(p.from_param, classmethod):
        def from_param(cls, x):
            if x is None:
                return cls()
            else:
                return x
        p.from_param = classmethod(from_param)

    return p

class UserString:
    def __init__(self, seq):
        if isinstance(seq, basestring):
            self.data = seq
        elif isinstance(seq, UserString):
            self.data = seq.data[:]
        else:
            self.data = str(seq)
    def __str__(self): return str(self.data)
    def __repr__(self): return repr(self.data)
    def __int__(self): return int(self.data)
    def __long__(self): return long(self.data)
    def __float__(self): return float(self.data)
    def __complex__(self): return complex(self.data)
    def __hash__(self): return hash(self.data)

    def __cmp__(self, string):
        if isinstance(string, UserString):
            return cmp(self.data, string.data)
        else:
            return cmp(self.data, string)
    def __contains__(self, char):
        return char in self.data

    def __len__(self): return len(self.data)
    def __getitem__(self, index): return self.__class__(self.data[index])
    def __getslice__(self, start, end):
        start = max(start, 0); end = max(end, 0)
        return self.__class__(self.data[start:end])

    def __add__(self, other):
        if isinstance(other, UserString):
            return self.__class__(self.data + other.data)
        elif isinstance(other, basestring):
            return self.__class__(self.data + other)
        else:
            return self.__class__(self.data + str(other))
    def __radd__(self, other):
        if isinstance(other, basestring):
            return self.__class__(other + self.data)
        else:
            return self.__class__(str(other) + self.data)
    def __mul__(self, n):
        return self.__class__(self.data*n)
    __rmul__ = __mul__
    def __mod__(self, args):
        return self.__class__(self.data % args)

    # the following methods are defined in alphabetical order:
    def capitalize(self): return self.__class__(self.data.capitalize())
    def center(self, width, *args):
        return self.__class__(self.data.center(width, *args))
    def count(self, sub, start=0, end=sys.maxint):
        return self.data.count(sub, start, end)
    def decode(self, encoding=None, errors=None): # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.decode(encoding, errors))
            else:
                return self.__class__(self.data.decode(encoding))
        else:
            return self.__class__(self.data.decode())
    def encode(self, encoding=None, errors=None): # XXX improve this?
        if encoding:
            if errors:
                return self.__class__(self.data.encode(encoding, errors))
            else:
                return self.__class__(self.data.encode(encoding))
        else:
            return self.__class__(self.data.encode())
    def endswith(self, suffix, start=0, end=sys.maxint):
        return self.data.endswith(suffix, start, end)
    def expandtabs(self, tabsize=8):
        return self.__class__(self.data.expandtabs(tabsize))
    def find(self, sub, start=0, end=sys.maxint):
        return self.data.find(sub, start, end)
    def index(self, sub, start=0, end=sys.maxint):
        return self.data.index(sub, start, end)
    def isalpha(self): return self.data.isalpha()
    def isalnum(self): return self.data.isalnum()
    def isdecimal(self): return self.data.isdecimal()
    def isdigit(self): return self.data.isdigit()
    def islower(self): return self.data.islower()
    def isnumeric(self): return self.data.isnumeric()
    def isspace(self): return self.data.isspace()
    def istitle(self): return self.data.istitle()
    def isupper(self): return self.data.isupper()
    def join(self, seq): return self.data.join(seq)
    def ljust(self, width, *args):
        return self.__class__(self.data.ljust(width, *args))
    def lower(self): return self.__class__(self.data.lower())
    def lstrip(self, chars=None): return self.__class__(self.data.lstrip(chars))
    def partition(self, sep):
        return self.data.partition(sep)
    def replace(self, old, new, maxsplit=-1):
        return self.__class__(self.data.replace(old, new, maxsplit))
    def rfind(self, sub, start=0, end=sys.maxint):
        return self.data.rfind(sub, start, end)
    def rindex(self, sub, start=0, end=sys.maxint):
        return self.data.rindex(sub, start, end)
    def rjust(self, width, *args):
        return self.__class__(self.data.rjust(width, *args))
    def rpartition(self, sep):
        return self.data.rpartition(sep)
    def rstrip(self, chars=None): return self.__class__(self.data.rstrip(chars))
    def split(self, sep=None, maxsplit=-1):
        return self.data.split(sep, maxsplit)
    def rsplit(self, sep=None, maxsplit=-1):
        return self.data.rsplit(sep, maxsplit)
    def splitlines(self, keepends=0): return self.data.splitlines(keepends)
    def startswith(self, prefix, start=0, end=sys.maxint):
        return self.data.startswith(prefix, start, end)
    def strip(self, chars=None): return self.__class__(self.data.strip(chars))
    def swapcase(self): return self.__class__(self.data.swapcase())
    def title(self): return self.__class__(self.data.title())
    def translate(self, *args):
        return self.__class__(self.data.translate(*args))
    def upper(self): return self.__class__(self.data.upper())
    def zfill(self, width): return self.__class__(self.data.zfill(width))

class MutableString(UserString):
    """mutable string objects

    Python strings are immutable objects.  This has the advantage, that
    strings may be used as dictionary keys.  If this property isn't needed
    and you insist on changing string values in place instead, you may cheat
    and use MutableString.

    But the purpose of this class is an educational one: to prevent
    people from inventing their own mutable string class derived
    from UserString and than forget thereby to remove (override) the
    __hash__ method inherited from UserString.  This would lead to
    errors that would be very hard to track down.

    A faster and better solution is to rewrite your program using lists."""
    def __init__(self, string=""):
        self.data = string
    def __hash__(self):
        raise TypeError("unhashable type (it is mutable)")
    def __setitem__(self, index, sub):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data): raise IndexError
        self.data = self.data[:index] + sub + self.data[index+1:]
    def __delitem__(self, index):
        if index < 0:
            index += len(self.data)
        if index < 0 or index >= len(self.data): raise IndexError
        self.data = self.data[:index] + self.data[index+1:]
    def __setslice__(self, start, end, sub):
        start = max(start, 0); end = max(end, 0)
        if isinstance(sub, UserString):
            self.data = self.data[:start]+sub.data+self.data[end:]
        elif isinstance(sub, basestring):
            self.data = self.data[:start]+sub+self.data[end:]
        else:
            self.data =  self.data[:start]+str(sub)+self.data[end:]
    def __delslice__(self, start, end):
        start = max(start, 0); end = max(end, 0)
        self.data = self.data[:start] + self.data[end:]
    def immutable(self):
        return UserString(self.data)
    def __iadd__(self, other):
        if isinstance(other, UserString):
            self.data += other.data
        elif isinstance(other, basestring):
            self.data += other
        else:
            self.data += str(other)
        return self
    def __imul__(self, n):
        self.data *= n
        return self

class String(MutableString, Union):

    _fields_ = [('raw', POINTER(c_char)),
                ('data', c_char_p)]

    def __init__(self, obj=""):
        if isinstance(obj, (str, unicode, UserString)):
            self.data = str(obj)
        else:
            self.raw = obj

    def __len__(self):
        return self.data and len(self.data) or 0

    def from_param(cls, obj):
        # Convert None or 0
        if obj is None or obj == 0:
            return cls(POINTER(c_char)())

        # Convert from String
        elif isinstance(obj, String):
            return obj

        # Convert from str
        elif isinstance(obj, str):
            return cls(obj)

        # Convert from c_char_p
        elif isinstance(obj, c_char_p):
            return obj

        # Convert from POINTER(c_char)
        elif isinstance(obj, POINTER(c_char)):
            return obj

        # Convert from raw pointer
        elif isinstance(obj, int):
            return cls(cast(obj, POINTER(c_char)))

        # Convert from object
        else:
            return String.from_param(obj._as_parameter_)
    from_param = classmethod(from_param)

def ReturnString(obj, func=None, arguments=None):
    return String.from_param(obj)

# As of ctypes 1.0, ctypes does not support custom error-checking
# functions on callbacks, nor does it support custom datatypes on
# callbacks, so we must ensure that all callbacks return
# primitive datatypes.
#
# Non-primitive return values wrapped with UNCHECKED won't be
# typechecked, and will be converted to c_void_p.
def UNCHECKED(type):
    if (hasattr(type, "_type_") and isinstance(type._type_, str)
        and type._type_ != "P"):
        return type
    else:
        return c_void_p

# ctypes doesn't have direct support for variadic functions, so we have to write
# our own wrapper class
class _variadic_function(object):
    def __init__(self,func,restype,argtypes):
        self.func=func
        self.func.restype=restype
        self.argtypes=argtypes
    def _as_parameter_(self):
        # So we can pass this variadic function as a function pointer
        return self.func
    def __call__(self,*args):
        fixed_args=[]
        i=0
        for argtype in self.argtypes:
            # Typecheck what we can
            fixed_args.append(argtype.from_param(args[i]))
            i+=1
        return self.func(*fixed_args+list(args[i:]))

# End preamble

_libs = {}
_libdirs = []

# Begin loader

# ----------------------------------------------------------------------------
# Copyright (c) 2008 David James
# Copyright (c) 2006-2008 Alex Holkner
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
#
#  * Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
#  * Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in
#    the documentation and/or other materials provided with the
#    distribution.
#  * Neither the name of pyglet nor the names of its
#    contributors may be used to endorse or promote products
#    derived from this software without specific prior written
#    permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
# FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
# COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
# BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
# LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
# LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
# ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
# ----------------------------------------------------------------------------

import os.path, re, sys, glob
import ctypes
import ctypes.util

def _environ_path(name):
    if name in os.environ:
        return os.environ[name].split(":")
    else:
        return []

class LibraryLoader(object):
    def __init__(self):
        self.other_dirs=[]

    def load_library(self,libname):
        """Given the name of a library, load it."""
        paths = self.getpaths(libname)

        for path in paths:
            if os.path.exists(path):
                return self.load(path)

        raise ImportError("%s not found." % libname)

    def load(self,path):
        """Given a path to a library, load it."""
        try:
            # Darwin requires dlopen to be called with mode RTLD_GLOBAL instead
            # of the default RTLD_LOCAL.  Without this, you end up with
            # libraries not being loadable, resulting in "Symbol not found"
            # errors
            if sys.platform == 'darwin':
                return ctypes.CDLL(path, ctypes.RTLD_GLOBAL)
            else:
                return ctypes.cdll.LoadLibrary(path)
        except OSError,e:
            raise ImportError(e)

    def getpaths(self,libname):
        """Return a list of paths where the library might be found."""
        if os.path.isabs(libname):
            yield libname
        else:
            # FIXME / TODO return '.' and os.path.dirname(__file__)
            for path in self.getplatformpaths(libname):
                yield path

            path = ctypes.util.find_library(libname)
            if path: yield path

    def getplatformpaths(self, libname):
        return []

# Darwin (Mac OS X)

class DarwinLibraryLoader(LibraryLoader):
    name_formats = ["lib%s.dylib", "lib%s.so", "lib%s.bundle", "%s.dylib",
                "%s.so", "%s.bundle", "%s"]

    def getplatformpaths(self,libname):
        if os.path.pathsep in libname:
            names = [libname]
        else:
            names = [format % libname for format in self.name_formats]

        for dir in self.getdirs(libname):
            for name in names:
                yield os.path.join(dir,name)

    def getdirs(self,libname):
        '''Implements the dylib search as specified in Apple documentation:

        http://developer.apple.com/documentation/DeveloperTools/Conceptual/
            DynamicLibraries/Articles/DynamicLibraryUsageGuidelines.html

        Before commencing the standard search, the method first checks
        the bundle's ``Frameworks`` directory if the application is running
        within a bundle (OS X .app).
        '''

        dyld_fallback_library_path = _environ_path("DYLD_FALLBACK_LIBRARY_PATH")
        if not dyld_fallback_library_path:
            dyld_fallback_library_path = [os.path.expanduser('~/lib'),
                                          '/usr/local/lib', '/usr/lib']

        dirs = []

        if '/' in libname:
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))
        else:
            dirs.extend(_environ_path("LD_LIBRARY_PATH"))
            dirs.extend(_environ_path("DYLD_LIBRARY_PATH"))

        dirs.extend(self.other_dirs)
        dirs.append(".")
        dirs.append(os.path.dirname(__file__))

        if hasattr(sys, 'frozen') and sys.frozen == 'macosx_app':
            dirs.append(os.path.join(
                os.environ['RESOURCEPATH'],
                '..',
                'Frameworks'))

        dirs.extend(dyld_fallback_library_path)

        return dirs

# Posix

class PosixLibraryLoader(LibraryLoader):
    _ld_so_cache = None

    def _create_ld_so_cache(self):
        # Recreate search path followed by ld.so.  This is going to be
        # slow to build, and incorrect (ld.so uses ld.so.cache, which may
        # not be up-to-date).  Used only as fallback for distros without
        # /sbin/ldconfig.
        #
        # We assume the DT_RPATH and DT_RUNPATH binary sections are omitted.

        directories = []
        for name in ("LD_LIBRARY_PATH",
                     "SHLIB_PATH", # HPUX
                     "LIBPATH", # OS/2, AIX
                     "LIBRARY_PATH", # BE/OS
                    ):
            if name in os.environ:
                directories.extend(os.environ[name].split(os.pathsep))
        directories.extend(self.other_dirs)
        directories.append(".")
        directories.append(os.path.dirname(__file__))

        try: directories.extend([dir.strip() for dir in open('/etc/ld.so.conf')])
        except IOError: pass

        directories.extend(['/lib', '/usr/lib', '/lib64', '/usr/lib64'])

        cache = {}
        lib_re = re.compile(r'lib(.*)\.s[ol]')
        ext_re = re.compile(r'\.s[ol]$')
        for dir in directories:
            try:
                for path in glob.glob("%s/*.s[ol]*" % dir):
                    file = os.path.basename(path)

                    # Index by filename
                    if file not in cache:
                        cache[file] = path

                    # Index by library name
                    match = lib_re.match(file)
                    if match:
                        library = match.group(1)
                        if library not in cache:
                            cache[library] = path
            except OSError:
                pass

        self._ld_so_cache = cache

    def getplatformpaths(self, libname):
        if self._ld_so_cache is None:
            self._create_ld_so_cache()

        result = self._ld_so_cache.get(libname)
        if result: yield result

        path = ctypes.util.find_library(libname)
        if path: yield os.path.join("/lib",path)

# Windows

class _WindowsLibrary(object):
    def __init__(self, path):
        self.cdll = ctypes.cdll.LoadLibrary(path)
        self.windll = ctypes.windll.LoadLibrary(path)

    def __getattr__(self, name):
        try: return getattr(self.cdll,name)
        except AttributeError:
            try: return getattr(self.windll,name)
            except AttributeError:
                raise

class WindowsLibraryLoader(LibraryLoader):
    name_formats = ["%s.dll", "lib%s.dll", "%slib.dll"]

    def load_library(self, libname):
        try:
            result = LibraryLoader.load_library(self, libname)
        except ImportError:
            result = None
            if os.path.sep not in libname:
                for name in self.name_formats:
                    try:
                        result = getattr(ctypes.cdll, name % libname)
                        if result:
                            break
                    except WindowsError:
                        result = None
            if result is None:
                try:
                    result = getattr(ctypes.cdll, libname)
                except WindowsError:
                    result = None
            if result is None:
                raise ImportError("%s not found." % libname)
        return result

    def load(self, path):
        return _WindowsLibrary(path)

    def getplatformpaths(self, libname):
        if os.path.sep not in libname:
            for name in self.name_formats:
                dll_in_current_dir = os.path.abspath(name % libname)
                if os.path.exists(dll_in_current_dir):
                    yield dll_in_current_dir
                path = ctypes.util.find_library(name % libname)
                if path:
                    yield path

# Platform switching

# If your value of sys.platform does not appear in this dict, please contact
# the Ctypesgen maintainers.

loaderclass = {
    "darwin":   DarwinLibraryLoader,
    "cygwin":   WindowsLibraryLoader,
    "win32":    WindowsLibraryLoader
}

loader = loaderclass.get(sys.platform, PosixLibraryLoader)()

def add_library_search_dirs(other_dirs):
    loader.other_dirs = other_dirs

load_library = loader.load_library

del loaderclass

# End loader

add_library_search_dirs([])

# No libraries

# No modules

MODULE_ID = c_short # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 6

HOST_ID = c_short # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 7

MSG_TYPE = c_int # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 8

MSG_COUNT = c_int # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 9

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 56
class struct_anon_1(Structure):
    pass

struct_anon_1.__slots__ = [
    'msg_type',
    'msg_count',
    'send_time',
    'recv_time',
    'src_host_id',
    'src_mod_id',
    'dest_host_id',
    'dest_mod_id',
    'num_data_bytes',
    'remaining_bytes',
    'is_dynamic',
    'reserved',
]
struct_anon_1._fields_ = [
    ('msg_type', MSG_TYPE),
    ('msg_count', MSG_COUNT),
    ('send_time', c_double),
    ('recv_time', c_double),
    ('src_host_id', HOST_ID),
    ('src_mod_id', MODULE_ID),
    ('dest_host_id', HOST_ID),
    ('dest_mod_id', MODULE_ID),
    ('num_data_bytes', c_int),
    ('remaining_bytes', c_int),
    ('is_dynamic', c_int),
    ('reserved', c_int),
]

DF_MSG_HEADER = struct_anon_1 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 56

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 63
class struct_anon_2(Structure):
    pass

struct_anon_2.__slots__ = [
    'msg_type',
    'msg_count',
    'send_time',
    'recv_time',
    'src_host_id',
    'src_mod_id',
    'dest_host_id',
    'dest_mod_id',
    'num_data_bytes',
    'remaining_bytes',
    'is_dynamic',
    'reserved',
    'data',
]
struct_anon_2._fields_ = [
    ('msg_type', MSG_TYPE),
    ('msg_count', MSG_COUNT),
    ('send_time', c_double),
    ('recv_time', c_double),
    ('src_host_id', HOST_ID),
    ('src_mod_id', MODULE_ID),
    ('dest_host_id', HOST_ID),
    ('dest_mod_id', MODULE_ID),
    ('num_data_bytes', c_int),
    ('remaining_bytes', c_int),
    ('is_dynamic', c_int),
    ('reserved', c_int),
    ('data', c_char * 9000),
]

DF_MESSAGE = struct_anon_2 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 63

STRING_DATA = POINTER(c_char) # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 77

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 88
class struct_anon_3(Structure):
    pass

struct_anon_3.__slots__ = [
    'mod_id',
    'reserved',
    'msg_type',
]
struct_anon_3._fields_ = [
    ('mod_id', MODULE_ID),
    ('reserved', c_short),
    ('msg_type', MSG_TYPE),
]

MDF_FAIL_SUBSCRIBE = struct_anon_3 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 88

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 95
class struct_anon_4(Structure):
    pass

struct_anon_4.__slots__ = [
    'dest_mod_id',
    'reserved',
    'time_of_failure',
    'msg_header',
]
struct_anon_4._fields_ = [
    ('dest_mod_id', MODULE_ID),
    ('reserved', c_short * 3),
    ('time_of_failure', c_double),
    ('msg_header', DF_MSG_HEADER),
]

MDF_FAILED_MESSAGE = struct_anon_4 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 95

MDF_MM_ERROR = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 97

MDF_MM_INFO = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 99

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 103
class struct_anon_5(Structure):
    pass

struct_anon_5.__slots__ = [
    'logger_status',
    'daemon_status',
]
struct_anon_5._fields_ = [
    ('logger_status', c_short),
    ('daemon_status', c_short),
]

MDF_CONNECT = struct_anon_5 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 103

MDF_SUBSCRIBE = MSG_TYPE # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 111

MDF_UNSUBSCRIBE = MSG_TYPE # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 112

MDF_PAUSE_SUBSCRIPTION = MSG_TYPE # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 113

MDF_RESUME_SUBSCRIPTION = MSG_TYPE # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 114

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 119
class struct_anon_6(Structure):
    pass

struct_anon_6.__slots__ = [
    'mod_id',
]
struct_anon_6._fields_ = [
    ('mod_id', c_int),
]

MDF_FORCE_DISCONNECT = struct_anon_6 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 119

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 127
class struct_anon_7(Structure):
    pass

struct_anon_7.__slots__ = [
    'pid',
]
struct_anon_7._fields_ = [
    ('pid', c_int),
]

MDF_MODULE_READY = struct_anon_7 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 127

MDF_DYNAMIC_DD_READ_ERR = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 129

MDF_DEBUG_TEXT = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 131

MDF_START_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 136

MDF_STOP_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 138

MDF_RESTART_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 140

MDF_KILL_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 142

MDF_SLAVE_START_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 147

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 149
class struct_anon_8(Structure):
    pass

struct_anon_8.__slots__ = [
    'num_remote_hosts',
]
struct_anon_8._fields_ = [
    ('num_remote_hosts', c_int),
]

MDF_SLAVE_START_APP_ACK = struct_anon_8 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 149

MDF_SLAVE_STOP_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 151

MDF_SLAVE_KILL_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 153

MDF_SLAVE_RESTART_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 155

MDF_AM_ERROR = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 159

MDF_FAIL_START_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 162

MDF_FAIL_STOP_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 164

MDF_FAIL_KILL_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 166

MDF_AM_CONFIG_FILE_DATA = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 174

MDF_AM_APP_NAME = POINTER(c_char) # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 176

MDF_SLAVE_FAIL_START_APP = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 181

MDF_SLAVE_AM_ERROR = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 188

MDF_APP_ERROR = STRING_DATA # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 193

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 201
class struct_anon_9(Structure):
    pass

struct_anon_9.__slots__ = [
    'timer_id',
]
struct_anon_9._fields_ = [
    ('timer_id', c_int),
]

MDF_TIMER_EXPIRED = struct_anon_9 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 201

MDF_TIMED_OUT = MDF_TIMER_EXPIRED # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 203

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 205
class struct_anon_10(Structure):
    pass

struct_anon_10.__slots__ = [
    'mod_id',
    'timer_id',
    'snooze_time',
]
struct_anon_10._fields_ = [
    ('mod_id', MODULE_ID),
    ('timer_id', c_int),
    ('snooze_time', c_int),
]

MDF_SET_TIMER_FAILED = struct_anon_10 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 205

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 212
class struct_anon_11(Structure):
    pass

struct_anon_11.__slots__ = [
    'timer_id',
    'snooze_time',
]
struct_anon_11._fields_ = [
    ('timer_id', c_int),
    ('snooze_time', c_int),
]

MDF_SET_TIMER = struct_anon_11 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 212

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 215
class struct_anon_12(Structure):
    pass

struct_anon_12.__slots__ = [
    'timer_id',
]
struct_anon_12._fields_ = [
    ('timer_id', c_int),
]

MDF_CANCEL_TIMER = struct_anon_12 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 215

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 232
class struct_anon_13(Structure):
    pass

struct_anon_13.__slots__ = [
    'pathname',
    'pathname_length',
]
struct_anon_13._fields_ = [
    ('pathname', c_char * 256),
    ('pathname_length', c_int),
]

MDF_SAVE_MESSAGE_LOG = struct_anon_13 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 232

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 246
class struct_anon_14(Structure):
    pass

struct_anon_14.__slots__ = [
    'time',
]
struct_anon_14._fields_ = [
    ('time', c_double * 12),
]

MDF_TIMING_TEST = struct_anon_14 # C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 246

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 12
try:
    MAX_MODULES = 200
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 13
try:
    DYN_MOD_ID_START = 100
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 14
try:
    MAX_HOSTS = 5
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 15
try:
    MAX_MESSAGE_TYPES = 10000
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 16
try:
    MIN_STREAM_TYPE = 9000
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 17
try:
    MAX_TIMERS = 100
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 18
try:
    MAX_INTERNAL_TIMERS = 20
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 21
try:
    MAX_DRAGONFLY_MSG_TYPE = 99
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 22
try:
    MAX_DRAGONFLY_MODULE_ID = 9
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 58
try:
    MAX_CONTIGUOUS_MESSAGE_DATA = 9000
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 67
try:
    MID_MESSAGE_MANAGER = 0
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 68
try:
    MID_COMMAND_MODULE = 1
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 69
try:
    MID_APPLICATION_MODULE = 2
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 70
try:
    MID_NETWORK_RELAY = 3
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 71
try:
    MID_STATUS_MODULE = 4
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 72
try:
    MID_QUICKLOGGER = 5
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 74
try:
    HID_LOCAL_HOST = 0
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 75
try:
    HID_ALL_HOSTS = 32767
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 81
try:
    ALL_MESSAGE_TYPES = 2147483647
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 84
try:
    MT_EXIT = 0
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 85
try:
    MT_KILL = 1
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 86
try:
    MT_ACKNOWLEDGE = 2
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 87
try:
    MT_FAIL_SUBSCRIBE = 6
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 89
try:
    MT_FAILED_MESSAGE = 7
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 96
try:
    MT_MM_ERROR = 83
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 98
try:
    MT_MM_INFO = 84
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 102
try:
    MT_CONNECT = 13
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 104
try:
    MT_DISCONNECT = 14
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 107
try:
    MT_SUBSCRIBE = 15
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 108
try:
    MT_UNSUBSCRIBE = 16
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 109
try:
    MT_PAUSE_SUBSCRIPTION = 85
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 110
try:
    MT_RESUME_SUBSCRIPTION = 86
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 116
try:
    MT_SHUTDOWN_DRAGONFLY = 17
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 117
try:
    MT_SHUTDOWN_APP = 18
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 118
try:
    MT_FORCE_DISCONNECT = 82
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 123
try:
    MT_CORE_MODULE_REINIT_ACK = 25
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 126
try:
    MT_MODULE_READY = 26
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 128
try:
    MT_DYNAMIC_DD_READ_ERR = 90
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 130
try:
    MT_DEBUG_TEXT = 91
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 134
try:
    MT_AM_EXIT = 30
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 135
try:
    MT_START_APP = 31
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 137
try:
    MT_STOP_APP = 32
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 139
try:
    MT_RESTART_APP = 33
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 141
try:
    MT_KILL_APP = 34
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 143
try:
    MT_AM_RE_READ_CONFIG_FILE = 89
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 144
try:
    MT_AM_GET_APP_NAME = 92
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 146
try:
    MT_SLAVE_START_APP = 64
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 148
try:
    MT_SLAVE_START_APP_ACK = 65
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 150
try:
    MT_SLAVE_STOP_APP = 66
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 152
try:
    MT_SLAVE_KILL_APP = 67
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 154
try:
    MT_SLAVE_RESTART_APP = 68
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 158
try:
    MT_AM_ERROR = 35
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 160
try:
    MT_AM_ACKNOWLEDGE = 36
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 161
try:
    MT_FAIL_START_APP = 37
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 163
try:
    MT_FAIL_STOP_APP = 38
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 165
try:
    MT_FAIL_KILL_APP = 39
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 167
try:
    MT_APP_START_COMPLETE = 40
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 168
try:
    MT_APP_SHUTODWN_COMPLETE = 41
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 169
try:
    MT_APP_RESTART_COMPLETE = 42
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 170
try:
    MT_APP_KILL_COMPLETE = 43
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 171
try:
    MT_ALL_MODULES_READY = 44
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 172
try:
    MT_CORE_MODULE_REINIT = 45
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 173
try:
    MT_AM_CONFIG_FILE_DATA = 46
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 175
try:
    MT_AM_APP_NAME = 93
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 179
try:
    MT_SLAVE_ALL_MODULES_READY = 69
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 180
try:
    MT_SLAVE_FAIL_START_APP = 70
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 182
try:
    MT_SLAVE_FAIL_STOP_APP = 71
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 183
try:
    MT_SLAVE_FAIL_KILL_APP = 72
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 184
try:
    MT_SLAVE_APP_SHUTODWN_COMPLETE = 74
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 185
try:
    MT_SLAVE_APP_RESTART_COMPLETE = 75
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 186
try:
    MT_SLAVE_APP_KILL_COMPLETE = 76
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 187
try:
    MT_SLAVE_AM_ERROR = 77
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 192
try:
    MT_APP_ERROR = 47
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 196
try:
    MT_SM_EXIT = 48
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 199
try:
    MT_CLOCK_SYNC = 49
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 200
try:
    MT_TIMER_EXPIRED = 50
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 202
try:
    MT_TIMED_OUT = 73
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 204
try:
    MT_SET_TIMER_FAILED = 51
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 210
try:
    MT_TM_EXIT = 52
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 211
try:
    MT_SET_TIMER = 53
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 214
try:
    MT_CANCEL_TIMER = 54
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 218
try:
    MT_LM_EXIT = 55
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 221
try:
    MT_MM_READY = 94
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 222
try:
    MT_LM_READY = 96
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 227
try:
    MT_SAVE_MESSAGE_LOG = 56
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 228
try:
    MAX_LOGGER_FILENAME_LENGTH = 256
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 234
try:
    MT_MESSAGE_LOG_SAVED = 57
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 237
try:
    MT_PAUSE_MESSAGE_LOGGING = 58
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 238
try:
    MT_RESUME_MESSAGE_LOGGING = 59
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 239
try:
    MT_RESET_MESSAGE_LOG = 60
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 240
try:
    MT_DUMP_MESSAGE_LOG = 61
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 242
try:
    MT_TIMING_TEST = 62
except:
    pass

# C:\\Users\\emd\\Documents\\dragonfly\\include\\Dragonfly_types.h: 243
try:
    MAX_TIMING_TEST_TIME_POINTS = 12
except:
    pass

# No inserted files

