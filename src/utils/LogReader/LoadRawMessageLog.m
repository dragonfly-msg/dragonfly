function Log = LoadRawMessageLog( Filename, DF)

% Log = LoadRawMessageLog( Filename, DF)
%
% Loads the binary Dragonfly message log in Filename and converts it to a matlab 
% data structure. The returned message log is "raw" in the sense that it is
% just a linear array of messages in the order in which they were recorded.
% Dragonfly is a structure containing Dragonfly message definitions (saved from a
% matlab module at runtime when Filename was recorded.

% Meel Velliste 10/31/2008
% Emrah Diril 5/1/2013

OPEN_FILE           = 1;
CLOSE_FILE          = 2;
READ_HEADERS        = 3;
READ_DATA_BLOCK     = 4;
GET_NUM_BYTES       = 5;


% Load file
if( ~ischar( Filename))
    error( 'LoadMessageLog(): Filename must be a character string containing a file name or a file path');
end
NumMessages = LogReader( OPEN_FILE, Filename);

try
    % Read message headers
    HeaderTemplate = repmat( DF.MESSAGE_HEADER, [1 NumMessages]);
    Log.Headers = LogReader( READ_HEADERS, HeaderTemplate);

    % Read message data
    Log.Data = cell( 1, NumMessages);
    for i = 1 : NumMessages
        % Get data template for current message
        H = Log.Headers(i);
        if( H.msg_type+1 > length( DF.MDF_by_MT))
            DataTemplate = {};
        else
            DataTemplate = DF.MDF_by_MT{H.msg_type+1}; % +1 because message types start from 0, but array index starts from 1
        end
        NumDataBytes = double( H.num_data_bytes);
        % If the message has a data format defined, but the actual message has no data, then issue a warning (this means that
        % a message that has been defined with data was actually sent as a signal)
        if( NumDataBytes == 0 && ~isempty( DataTemplate))
            if( ischar( DataTemplate) && strmatch( 'VARIABLE_LENGTH_ARRAY', DataTemplate))
                % Variable length arrays are allowed to have no data
                % without warning
            else
                % Fixed format messages should not have no data, but not a
                % fatal condition, so issue a warning
                MsgType = DF.MTN_by_MT{H.msg_type+1};
                %warning( 'Dragonfly:LogReader:MissingMessageData', ['Message has a data format defined but no actual data: msg_type = ' num2str(H.msg_type) ' [' MsgType ']']);
            end
        end
        % Get message data if it has any
        if( NumDataBytes > 0)
            % If data is a variable length message, then do tricky stuff,
            % otherwise just load data using the DataTemplate
            if( ischar( DataTemplate) && strmatch( 'VARIABLE_LENGTH_ARRAY', DataTemplate))
                DataTemplate = CreateTemplateForVariableLengthArray( DataTemplate, NumDataBytes, DF);
            end
            try
                
                NumTemplateDataBytes = LogReader( GET_NUM_BYTES, DataTemplate);
                if( NumDataBytes ~= NumTemplateDataBytes)
                    error('Dragonfly:LogReader:NumDataBytesMismatch', ['Number of template data bytes (' num2str( NumTemplateDataBytes) ') is not equal to the number of data bytes in actual message (' num2str(NumDataBytes) '), msg_type = ' num2str(H.msg_type) ' [' DF.MTN_by_MT{H.msg_type+1} ']']);
                else
                    Log.Data{i} = LogReader( READ_DATA_BLOCK, DataTemplate, i-1, NumDataBytes); % i-1 because the internal mex code expects the index as a 0-based C-style index
                end
                
            catch
                err = lasterror( );
                MsgType = DF.MTN_by_MT{H.msg_type+1};
                err.message = sprintf( 'Error while trying to read data block (i = %i, msg_type = %i [%s])\n%s', i, H.msg_type, MsgType, err.message);
                rethrow( err);
            end
        end
    end
catch
    % Close file
    LogReader( CLOSE_FILE);
    % Re-throw error
    disp( 'Caught error in LoadRawMessageLog and cleaned up (do not remove this because the clean-up is quite necessary to avoid a memory leak)');
    le = lasterror();
    rethrow( le);
end

% Close file
LogReader( CLOSE_FILE);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function NewTemplate = CreateTemplateForVariableLengthArray( DataTemplate, NumDataBytes, DF)

    open_parenthesis_idx = find( DataTemplate == '(');
    if( length( open_parenthesis_idx) ~= 1)
        error( ['incorrect format for VARIABLE_LENGTH_ARRAY data template: "' DataTemplate '"']);
    end
    
    close_parenthesis_idx = find( DataTemplate == ')');
    if( length( close_parenthesis_idx) ~= 1)
        error( ['incorrect format for VARIABLE_LENGTH_ARRAY data template: "' DataTemplate '"']);
    end
    
    DataTypeString = DataTemplate(open_parenthesis_idx+1:close_parenthesis_idx-1);
    if( length( DataTypeString) < 1)
        error( ['Data type string not found in VARIABLE_LENGTH_ARRAY data template: "' DataTemplate '"']);
    end
    
    % First assume the data type is a matlab built-in type and try to
    % create an element template based on it
    try
        ElementTemplate = feval( DataTypeString, 0);
    catch
        % If it was not a matlab built-in type, search the list of defined
        % Dragonfly types
        if( isfield( DF.defines, DataTypeString))
            ElementTemplate = DF.defines.(DataTypeString);
        else
            % If it is not a Dragonfly defined type either, then give up
            error( ['Unable to define VARIABLE_LENGTH_ARRAY data template: "' DataTemplate '"']);
        end
    end
    
    % Create a template for the whole array based on the element template
    % and the number of available bytes in the message
    GET_NUM_BYTES = 5;
    ElementNumBytes = LogReader( GET_NUM_BYTES, ElementTemplate);
    NumElements = NumDataBytes / ElementNumBytes;
    if( round( NumElements) ~= NumElements)
        error( ['Number of message data bytes (' num2str(NumDataBytes) ') is not divisible by element size (' num2str(ElementNumBytes) '). Data template is ' DataTemplate]);
    end
    NewTemplate = repmat( ElementTemplate, [1 NumElements]);
    