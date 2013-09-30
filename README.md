## Introduction

Dragonfly is a simple messaging system that helps programmers create modular distributed 
applications rapidly. It hides the complexities of socket programming and data translation, also provides a uniform 
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


## History

Dragonfly was first developed, under the name Real-Time Messaging Architecture (RTMA), by Meel Velliste and Sagi Perel
for use in the development of brain-computer interface development.

Publications whose experiments utilized Dragonfly Messaging include:
- Velliste, M., Perel, S., Spalding, M. C., Whitford, A. S., & Schwartz, A. B. (2008). **Cortical control of a prosthetic arm for self-feeding.** Nature, 453(7198), 1098-101. doi:10.1038/nature06996
- Clanton, S. T., McMorland, A. J. C., Zohny, Z., Jeffries, S. M., Rasmussen, R. G., Flesher, S. N., & Velliste, M. (2013). **Seven Degree of Freedom Cortical Control of a Robotic Arm.** In C. Guger, B. Z. Allison, & G. Edlinger (Eds.), Brain-Computer Interface Research (pp. 73-81). Berlin, Heidelberg: Springer Berlin Heidelberg. doi:10.1007/978-3-642-36083-1
- Collinger, J. L., Wodlinger, B., Downey, J. E., Wang, W., Tyler-Kabara, E. C., Weber, D. J., McMorland, A. J. C., Velliste, M., Boninger, M. L., Schwartz, A. B. (2012). **High-performance neuroprosthetic control by an individual with tetraplegia.** The Lancet. doi:10.1016/S0140-6736(12)61816-9

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
- Version >= 2007b
- Configure Matlab to recognize the Visual Studio C++ compiler


## Installation

#### Linux

Clone the repository and compile the source as follows:

1. In a terminal execute the following:

        cd Dragonfly/build
        make

2. Create `DRAGONFLY` environment variable and set it to where your Dragonfly folder is

3. Copy `Dragonfly/lib/libDragonfly.so` to `/usr/lib` or add `Dragonfly/lib` to `LD_LIBRARY_PATH`
(See set_env_vars.sh in `tools' folder for reference)

4. If you plan to use the matlab interface, start matlab and execute the following:

        cd Dragonfly/lang/matlab
        make
        cd Dragonfly/src/utils/LogReader
        make

5. If you plan to use the python interface, append `Dragonfly/lang/python` to `PYTHONPATH` environment variable 
(See set_env_vars.sh in `tools' folder for reference)
        

#### Windows

We recommend that you use the installer provided in the [binaries repo](https://github.com/dragonfly-msg/binaries/blob/master/dragonfly_windows_setup.exe?raw=true) 
which contains ready-to-use executables and will also set the necessary environment variables automatically.

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
        
The message body is a `struct` composed of one or more data fields which can be standard [C data types](http://en.wikipedia.org/wiki/C_data_types) 
and other structs. The struct has to have the same message name as the message type, and it needs to begin with `MDF_`. 
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

The message body fields need to be manually padded for [data alignment](http://en.wikipedia.org/wiki/Data_alignment) as necessary. 
The following is an example of how to define the fields for 64-bit alignment:

        typedef struct {
          int source_index;    		
          int reserved;        		// for 64-bit alignment
          double source_timestamp;
        } MDF_RAW_SAMPLE_RESPONSE;

If you are not sure how to align message fields on your system, it is safe to use 64-bit alignment. 
Even if your system is not 64-bit, or if you have a mixture of systems with different alignment requirements,
this practice will ensure proper alingment.

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


