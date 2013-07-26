function CheckSubscribed( MessageType)

    global Dragonfly_runtime;

    if( strmatch( MessageType, Dragonfly_runtime.Subscribed, 'exact'))
    else
        error_message = ['MessageType "' MessageType '" not subscribed'];
        error( error_message);
    end
