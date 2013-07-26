## Introduction

Dragonfly is a simple messaging system that helps programmers create modular distributed 
applications rapidly. It hides the complexities of socket programming and provides a uniform 
high-level API in each of the supported programming languages (C++, C#, Python, Matlab) 
and operating systems (Windows, Linux). Therefore, programmers are able to write each part of 
their application in their programming language of choice and on their operating system of choice 
without having to worry about how the modules will communicate with each other.

Dragonfly consists of several components: a central message exchange daemon (MessageManager), 
a data logger daemon (Logger), and utilities to create message definitions in all supported programming languages.

Dragonfly uses a client-server architecture where MessageManager is the central server and software 
modules that would like to talk to each other are the clients. MessageManager keeps a listening socket for modules 
to connect to and start sending messages. All messages go through MessageManager which forwards 
them to the connected modules based on their subscriptions. Modules connect to MessageManager, subscribe 
to message types they care about, send messages that will be forwarded by MessageManager to all modules 
that have subscribed to those message types, and receive messages that they themselves have subscribed to. 
The modules remain independent of each other and do not have to know which modules will consume their messages or 
where the messages they consume originate.


## Prerequisites

Bare minimum requirement is that you have a C++ compiler installed. On linux, you also need to have qt4-qmake 
installed (in a future release, this requirement will be eliminated). If you’d like to have support for other languages, 
see below further requirements:

#### Python
- Version >= 2.6 (python3 is currently not supported)
- Install swig >= 2.0.3 (on windows, make sure `swig.exe` is in PATH)
- Install ctypeslib 
  * Linux: `sudo apt-get install python-ctypeslib`
  * Windows: Download from http://code.google.com/p/ctypesgen 

#### C&#35;
- Windows only, Visual Studio 2005 or later

#### Matlab 
- Configure Matlab to recognize the Visual Studio C++ compiler


## Installation

#### Linux

Clone the repository and compile the source as follows:

1. In a terminal execute the following:

        cd Dragonfly/build
        make

2. Create `DRAGONFLY` environment variable and set it to where your Dragonfly folder is

3. Add `Dragonfly/lib` to `LD_LIBRARY_PATH` or copy `Dragonfly/lib/libDragonfly.so` to `/usr/lib`

4. If you plan to use the matlab interface, start matlab and execute the following:

        cd Dragonfly/lang/matlab
        make
        cd Dragonfly/src/utils/LogReader
        make

5. If you plan to use the python interface, add `Dragonfly/lang/python` to `PYTHONPATH` environment variable
        

#### Windows

We recommended that you use the provided installer (link) which contains prebuilt binaries and will also set
environment variables automatically.

If you'd like to compile from source, clone the repository and follow these instructions:

1. Build `Dragonfly/build/Dragonfly.sln` with Visual Studio (2005 or later)

2. Create `DRAGONFLY` environment variable and set it to where your Dragonfly folder is

3. If you plan to use the python interface, 
 * Set `PYTHON_LIB` environment variable (ex: C:\Python27\libs)
 * Set `PYTHON_INCLUDE` environment variable (ex: C:\Python27\include)
 * Build `Dragonfly/lang/python/PyDragonfly.sln` with Visual Studio (2005 or later)
 * Add `%DRAGONFLY%\lang\python` to `PYTHONPATH` environment variable
	
4. If you plan to use the Matlab interface, start matlab and execute the following:
    	
        cd Dragonfly/lang/matlab
        make
        cd Dragonfly/src/utils/LogReader
        make


## Directory Organization
- `bin`    
    executable modules
- `build`    
    source code build scripts
- `examples`    
    example programs showing how to use Dragonfly
- `include`  
    include files for the C++ API
- `lang`     
    APIs for other languages
- `lib`      
    library files for the C++ API
- `src`      
    source code for C++ API and executable modules
- `tools`    
    scripts for creating installers and generating message definition files


## API Summary
- ConnectToMMM(ModuleID, ServerAddress)
- DisconnectFromMMM()
- Subscribe(MessageType)
- Unsubscribe(MessageType)
- ReadMessage(Timeout)
- SendMessage(MessageType, MessageData)
- SendSignal(MessageType)


## Example Code
`Dragonfly/examples` folder contains ready to run modules in all supported languages. See the README.txt file 
in each example folder for further information.


## Creating Message Definitions
Dragonfly uses standard C header files to describe message definitions.

Each message consists of a message type and an optional message body. 

The message type is an integer that should be selected to uniquely identify each message. 
It is set with a `#define` statement and the name of the message needs to begin with `MT_`.
Here is an example: 

        #define MT_ROBOT_FEEDBACK               100
        
The message body is a `struct` composed of one or more data fields which can be standard C data types and other structs.
The struct has to have the same message name as the message type, and it needs to begin with `MDF_`. 
Here is an example:
        
        typedef struct {
          double    position;
          double    velocity;
          double    force;
        } MDF_ROBOT_FEEDBACK;        

Here is a more complex example:

        typedef struct {
            int     SerialNo;
            int     Flags;
            double  dt;
        } SAMPLE_HEADER;

        #define MAX_ROBOT_FEEDBACK_DIMS     10
        typedef struct {
          SAMPLE_HEADER sample_header;
          double        position[MAX_ROBOT_FEEDBACK_DIMS];
          double        velocity[MAX_ROBOT_FEEDBACK_DIMS];
          double        force[MAX_ROBOT_FEEDBACK_DIMS];
        } MDF_ROBOT_FEEDBACK;

The message body needs to be manually padded for data alignment as necessary. The following is an example of how to align
the message body for a 64-bit system:

        typedef struct {
          int source_index;    		
          int reserved;        		// for 64-bit alignment
          double source_timestamp;
        } MDF_RAW_SAMPLE_RESPONSE;

To translate the message definitions in C header files into constructs in your choice of language, you need to process them 
with the appropriate build script for your language. The build scripts are located in the `Dragonfly\tools` folder.
Python build script is written in python. Matlab and C&#35; build scripts require Matlab. (In a future release, dependency on 
Matlab will be eliminated)

#### Matlab

        build_matlab_message_defs(path_to_message_definition_header_file)

#### C&#35;
        
        build_dotNet_message_defs(path_to_message_definition_header_file)

#### Python
    
        build_python_message_defs(path_to_message_definition_header_file)


