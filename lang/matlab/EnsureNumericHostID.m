function HostID = EnsureNumericHostID( HostID)

    % If HostID is a string, converts it to a numeric host ID
    % based on the lookup table in the Dragonfly structure

    global DF;

    if( ischar( HostID))
        if( isfield( DF.HID, HostID))
            HostID = DF.HID.(HostID);
        else
            error( ['Unrecognized HostID: ' HostID]);
        end
    end
