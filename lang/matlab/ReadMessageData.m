function MessageData = ReadMessageData( DataTemplate)

    global DF;
    MessageData = MatlabDragonfly( DF.mex_opcode.READ_MESSAGE_DATA, DataTemplate);
