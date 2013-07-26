function Subscribe( varargin)
    
    global DF;
    global Dragonfly_runtime;

    for i = 1 : nargin
    
        MessageType = varargin{i};
        
        % convert Message Type String to numeric Message Type ID
        MessageTypeNumber = MessageTypeID_from_String( MessageType);

        status = MatlabDragonfly( DF.mex_opcode.SUBSCRIBE, MessageTypeNumber);
        if( status == 0) error( 'Subscribe mex-function failed'); end
        
        % Add a record to the subscribed message-types list
        Dragonfly_runtime.Subscribed(end+1,1) = {MessageType};
    end
