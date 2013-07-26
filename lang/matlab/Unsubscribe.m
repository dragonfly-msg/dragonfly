function Unsubscribe( varargin)

    global DF;
    global Dragonfly_runtime;

    for i = 1 : nargin
    
        MessageType = varargin{i};
        
        % convert Message Type String to numeric Message Type ID
        MessageTypeNumber = MessageTypeID_from_String( MessageType);

        status = MatlabDragonfly( DF.mex_opcode.UNSUBSCRIBE, MessageTypeNumber);
        if( status == 0) error( 'Unsubscribe mex-function failed'); end
        
        % Remove the record from the subscribed message-types list
        idx = strmatch( MessageType, Dragonfly_runtime.Subscribed, 'exact');
        Dragonfly_runtime.Subscribed(idx,:) = [];
    end
