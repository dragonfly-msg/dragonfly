function Message = ReadMessage( arg)

% Message = ReadMessage( timeout)
% Message = ReadMessage( blocking_or_not)
%
% Tries to read a message from Dragonfly. TIMEOUT is a how long
% (in seconds) we wait for the message before timing out. If timeout
% is -1, then wait forever (i.e. blocking call). If timeout is 0
% then check for message and return immediately (i.e. non-blocking).
% Alternatively, 'blocking' or 'non-blocking' can be specified as
% a string argument.

global DF;

if( ~exist( 'arg', 'var'))
    timeout = -1; % Blocking call
    blocking = true;
else
    if( ischar( arg))
        switch( arg)
            case 'blocking', blocking = true; timeout = -1;
            case 'non-blocking', blocking = false; timeout = 0;
            otherwise, error( 'The blocking_or_not argument to ReadMessage should be either ''blocking'' or ''non-blocking''');
        end
    else
        % By default, input argument is a timeout value in seconds
        timeout = arg;
        if( timeout == -1)
            blocking = true;
        else
            blocking = false;
        end
    end
end

% Read the message header
if( blocking) % If infinitely blocking, then keep doing reads with timeout to allow Ctrl-C
    Message = [];
    while( isempty( Message))
        Message = MatlabDragonfly( DF.mex_opcode.READ_MESSAGE_HDR, DF.MESSAGE_HEADER, .2);
    end
else % If non-blocking or specified timeout, then use timeout value directly
    Message = MatlabDragonfly( DF.mex_opcode.READ_MESSAGE_HDR, DF.MESSAGE_HEADER, timeout);
end

if( isempty( Message))
    Message = []; % Just in case MatlabDragonfly returns a different kind of empty
else
    % If there is data in the message, then read it
    if( Message.is_dynamic )
       Message.data = MatlabDragonfly( DF.mex_opcode.READ_MESSAGE_DD);
    elseif( Message.num_data_bytes > 0)
        DataTemplate = GetMDF_by_MT( Message.msg_type);
        if( ischar( DataTemplate) & strmatch( 'VARIABLE_LENGTH_ARRAY', DataTemplate))
            Message.data = DataTemplate;
        else
            %try
                Message.data = MatlabDragonfly( DF.mex_opcode.READ_MESSAGE_DATA, DataTemplate);
            %catch
                %disp('ReadMessage: Error while reading message: Message=');
                %Message
            %end
        end
    else
        Message.data = [];
    end

    % Replace numeric Message Type ID with a string Message Type Name
    % if one is available (i.e. it is a registered message)
    MessageTypeName = GetMTN_by_MT( Message.msg_type);
    if( isempty( MessageTypeName))
        error(['ReadMessage: failed to find MTN for message type ' num2str(Message.msg_type)]);
    end        
    Message.msg_type = MessageTypeName;

end
