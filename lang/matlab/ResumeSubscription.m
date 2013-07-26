function ResumeSubscription( varargin)
    
    global DF;
    global Dragonfly_runtime;

    for i = 1 : nargin
    
        MessageType = varargin{i};
        
        % convert Message Type String to numeric Message Type ID
        MessageTypeNumber = MessageTypeID_from_String( MessageType);
        
        % tell MM to resume sending this message type
        status = MatlabDragonfly( DF.mex_opcode.RESUME_SUBSCRIPTION, MessageTypeNumber);
        if( status == 0), error( 'ResumeSubscription mex-function failed'); end

        % Remove the record from the paused subscriptions list
        idx = strmatch( MessageType, Dragonfly_runtime.Paused, 'exact');
        Dragonfly_runtime.Paused(idx,:) = [];
        
    end
