function MessageDataFormat = GetMDF_by_MT( MessageType)

global DF;
MessageDataFormat = DF.MDF_by_MT{MessageType+1};
