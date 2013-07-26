function SendModuleReady

global DF;

status = MatlabDragonfly( DF.mex_opcode.SEND_MODULE_READY);
if( status == 0) error( 'Could not send MODULE_READY'); end
