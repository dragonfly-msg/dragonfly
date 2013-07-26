function MessageTypeID = MessageTypeID_from_String( MessageTypeString)

    global DF;

    switch( class( MessageTypeString))
        case 'char'
            if( isfield( DF.MT, MessageTypeString))
                MessageTypeID = DF.MT.(MessageTypeString);
            else
                error( ['Unrecognized MessageType: ' MessageTypeString]);
            end
        otherwise
            error( 'MessageType expected to be a string value');
    end
