function MessageType = EnsureNumericMessageType( MessageType)

    % If Messagetype is a string, converts it to a numeric message type ID
    % based on the lookup table in the Dragonfly structure

    global DF;

    if( ischar( MessageType))
        if( isfield( DF.MT, MessageType))
            MessageType = DF.MT.(MessageType);
        else
            error( ['Unrecognized MessageType: ' MessageType]);
        end
    end
