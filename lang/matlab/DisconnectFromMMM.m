function DisconnectFromMMM

global DF;

if( ~isempty( DF))
    MatlabDragonfly( DF.mex_opcode.DISCONNECT_FROM_MMM);
    DF.Connected = false;
end
