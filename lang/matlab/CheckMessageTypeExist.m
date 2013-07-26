function CheckMessageTypeExist( msg_type)
% Ensure that the specified Message type exists. If not, generate an error.

    global DF;
    
    if( ~isfield( DF.MT, msg_type))
        error_message = sprintf( 'The message type (%s) does not exist', msg_type);
        error( error_message);
    end
