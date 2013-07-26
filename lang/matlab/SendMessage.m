function [send_time, msg_count] = SendMessage( MessageType, Data, DestModuleID, DestHostID)

% [send_time, msg_count] = SendMessage( MessageType, Data, DestModuleID, DestHostID)
%
% Send a message through the Dragonfly system. DestModuleID and DestHostID can
% optionally specify a specific destination module for the message,
% otherwise the message is broadcast.

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
MessageType  = EnsureNumericMessageType( MessageType);
DestModuleID = EnsureNumericModuleID( DestModuleID);
DestHostID   = EnsureNumericHostID( DestHostID);

% Check that Data has the correct format according to MessageType
Format = GetMDF_by_MT( MessageType);
format_correct = CheckFormat( Data, Format);
if( ~format_correct)
    error( 'The format of Data does not conform to the defined format for the given MessageType');
end

% Send the message
[status, send_time, msg_count] = MatlabDragonfly( DF.mex_opcode.SEND_MESSAGE, MessageType, Data, DestModuleID, DestHostID);
if( status == 0) error( 'Could not send message'); end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function correct = CheckFormat( data, format)

correct = 0;

% If it is a variable length data format, then there is nothing to check
if( ischar( format) & strmatch( 'VARIABLE_LENGTH_ARRAY', format))
    correct = 1;
    return;
end

data_class = class( data);
format_class = class( format);

% First check that data and format have the same matlab class
if( ~strcmp( data_class, format_class))
    disp( 'Matlab class of Data is different from that of Format');
    data_class
    format_class
    disp(data)
    return;
end

% Check that the array size is the same
data_size = size( data);
format_size = size( format);
if( length( data_size) ~= length( format_size))
    disp( 'Data and Format have diffent numbers of dimensions');
    return;
end
if( data_size == format_size)
else
    disp( ['Data and Format have different size, data_size = [' num2str(data_size) '], format_size = [' num2str(format_size) ']']);
    return;
end

switch( data_class)
    case 'struct'
        % If data is a struct, then check all fields
        data_fields = fieldnames( data);
        num_data_fields = length( data_fields);
        format_fields = fieldnames( format);
        num_format_fields = length( format_fields);
        % Check that the number of fields is the same
        if( num_data_fields ~= num_format_fields)
            disp( 'Number of fields does not match');
            return;
        end
        for i = 1 : num_data_fields
            % Check if the field names are the same
            data_field_name = data_fields{i};
            format_field_name = format_fields{i};
            if( ~strcmp( data_field_name, format_field_name))
                disp( 'Field names do not match');
                return;
            end
            % Check if the field types are the same
            data_value = getfield( data, data_field_name);
            format_value = getfield( format, format_field_name);
            if( ~CheckFormat( data_value, format_value))
                disp( ['Field types do not match: "' data_field_name '"']);
                return;
            end
        end
end

correct = 1;
