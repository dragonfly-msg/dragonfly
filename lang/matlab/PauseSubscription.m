function PauseSubscription( varargin)

    global DF;
    global Dragonfly_runtime;

    for i = 1 : nargin
    
        MessageType = varargin{i};
        
        % convert Message Type String to numeric Message Type ID
        MessageTypeNumber = MessageTypeID_from_String( MessageType);
        
        % tell MM to pause sending this message type
        status = MatlabDragonfly( DF.mex_opcode.PAUSE_SUBSCRIPTION, MessageTypeNumber);
        if( status == 0), error( 'PauseSubscription mex-function failed'); end
        
        % put message type on paused message types list
        Dragonfly_runtime.Paused(end+1,1) = {MessageType};

    end
