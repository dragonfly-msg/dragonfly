This directory contains the top-level build-scripts for Dragonfly.
To build Dragonfly:

On Windows, open Dragonfly.sln in Visual Studio (tested with version 2005), 
select the "Release" configuration and run Build All. This will build
the C++ API (lib/Dragonfly.lib), the .NET API (lang/dot_net/Dragonfly.NET.dll), 
and the executable modules (bin/MessageManager.exe and bin/QuickLogger.exe).

On Linux, make sure you have qmake installed in addition to regular make,
the run "make" within this build directory. This will build the C++ API
(lib/libDragonfly.so), the Python API (lang/python/PyDragonfly.?), and the
executable modules (bin/MessageManager and bin/QuickLogger).

The Matlab API needs to be compiled separately by starting matlab, changing current
directory to lang/matlab and running the make.m script. It works the same on
Windows or Linux.
