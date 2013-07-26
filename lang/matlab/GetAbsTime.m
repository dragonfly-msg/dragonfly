function timestamp = GetAbsTime

global DF;
timestamp = MatlabDragonfly( DF.mex_opcode.GET_TIME);
