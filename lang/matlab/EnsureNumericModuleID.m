function ModuleID = EnsureNumericModuleID( ModuleID)

    % If ModuleID is a string, converts it to a numeric module ID
    % based on the lookup table in the Dragonfly structure

    global DF;

    if( ischar( ModuleID))
        if( isfield( DF.MID, ModuleID))
            ModuleID = DF.MID.(ModuleID);
        else
            error( ['Unrecognized ModuleID: ' ModuleID]);
        end
    end
