function [send_time, msg_count] = SendSignal( MessageType, DestModuleID, DestHostID)

global DF;

% If destination not specified, then a destination of 0 is used by default,
% meaning "broadcast"
if( ~exist( 'DestModuleID', 'var'))
    DestModuleID = 0;
end
if( ~exist( 'DestHostID', 'var'))
    DestHostID = 0;
end

% If MessageType is a string, convert it to numeric Message Type ID
% Also do the same for module and host ID
MessageType = EnsureNumericMessageType( MessageType);
DestModuleID = EnsureNumericModuleID( DestModuleID);
DestHostID = EnsureNumericModuleID( DestHostID);

% Make sure that the specified message type is a signal (i.e. has no data)
if( isfield( DF.MDF, MessageType))
    error( ['MessageType ' MessageType ' is supposed to have data and cannot be sent using SendSignal']);
end

[status, send_time, msg_count] = MatlabDragonfly( DF.mex_opcode.SEND_SIGNAL, MessageType, DestModuleID, DestHostID);
if( status == 0) error( 'Could not send signal'); end
