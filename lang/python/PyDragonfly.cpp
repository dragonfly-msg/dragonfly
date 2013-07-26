// Andrew S. Whitford 01/09 asw35
#include "PyDragonfly.h"


/// Destructor. Dragonfly cleanup.
PyDragonfly::~PyDragonfly(void)
{
    EndModule();

 return;
}


/// Initializes a connection to Dragonfly and subscribes to relevant message types.
void
PyDragonfly::InitializeAndSubscribe(MODULE_ID moduleid, MSG_TYPE* types,
                               unsigned num_types)
{
    try
    {
        mid = moduleid;
        Dragonfly_Module::InitVariables(mid, HID);
        BeginModule(types, num_types); // Might want to just connect.
    }
    catch(MyCException& e)
    {
        e.AddToStack("PyDragonfly::InitializeAndSubscribe");
        e.ReportToFile(const_cast<char*>(LOG_FILENAME));
        throw e;
    }

 return;
}


/// Send a signal (a message with no data) to all Dragonfly modules. Does not
/// (yet) reproduce the capability to send signals to specific modules.
void
PyDragonfly::Signal(int type)
{
    try { SendSignal(type); }
    catch(MyCException& e)
    {
        e.AddToStack("PyDragonfly::Signal");
        e.ReportToFile(const_cast<char*>(LOG_FILENAME));
        throw e;
    }

 return;
}


/// Send a message to all Dragonfly modules.
void
PyDragonfly::Send(int type, void *data, unsigned size)
{
    try
    {
        CMessage ms(type, data, size);
        SendMessage(&ms);
    }
    catch(MyCException& e)
    {
        e.AddToStack("PyDragonfly::Send");
        e.ReportToFile(const_cast<char*>(LOG_FILENAME));
        throw e;
    }

 return;
}


/// Read a Dragonfly message. If no argument is provided, then the timeout defaults
/// to a value of -1, which signals a blocking read. A timeout of 0 signals a
/// non-blocking read. If the timeout is positive, then ReadMessage blocks for
/// up to timeout seconds.
MSG_TYPE
PyDragonfly::Read(double timeoutS) 
{
    // ReadMessage returns 1 if a message was read, and 0 if no message is
    // available.
    try { ReadMessage(&mr, timeoutS); }
    catch(MyCException& e)
    {
        e.AddToStack("PyDragonfly::Read");
        e.ReportToFile(const_cast<char*>(LOG_FILENAME));
        throw e;
    }

 return mr.msg_type;
}


/// Retrieves the MDF for the last message that was read.
void
PyDragonfly::GetMessageData(void* buffer, unsigned buffer_size)
{
    try
    {
        if(mr.num_data_bytes <= buffer_size)
            mr.GetData(buffer);
        else
            throw MyCException("Data buffer too small");
    }
    catch(MyCException& e)
    {
        e.AddToStack("PyDragonfly::GetMessageData");
        e.ReportToFile(const_cast<char*>(LOG_FILENAME));
        throw e;
    }

 return;
}


