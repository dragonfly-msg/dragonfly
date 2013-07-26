function send_time = UnsafeSendMessage(MT, Data)
% send_time = UnsafeSendMessage(MT, Data)
% Unsafe (but much faster) SendMessage.  MT must be a number, not a name,
% and Data is not checked for correctness.

global DF;

[status, send_time, msg_count] = MatlabDragonfly(DF.mex_opcode.SEND_MESSAGE, MT, Data, 0, 0);
if( status == 0) error( 'Could not send message'); end