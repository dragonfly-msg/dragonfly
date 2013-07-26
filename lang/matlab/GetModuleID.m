function module_id = GetModuleID()

global DF;
module_id = MatlabDragonfly( DF.mex_opcode.GET_MODULE_ID);
