function MessageTypeName = GetMTN_by_MT( MessageType)

global DF;
MessageTypeName = DF.MTN_by_MT{MessageType+1};
