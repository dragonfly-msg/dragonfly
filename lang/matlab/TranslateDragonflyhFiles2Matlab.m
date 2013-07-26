function TranslateDragonflyhFiles2Matlab( Dragonfly_BaseDir, MessageDefFile)
% TranslateDragonflyhFiles2Matlab( Dragonfly_BaseDir, MessageDefFile)
%
% Translates C header files to Matlab, saves the result in a mat file
% with the same base file name as the input MessageDefFile.
%
% Dragonfly Base directory (Dragonfly_BaseDir) needs to be specified so that it can find the core
% definition files. MessageDefFile specifies the application-specific
% message definition file (e.g. '../../Source/Dragonfly_config.h').

    [ConfigFileDir, ConfigFileBaseName] = fileparts( MessageDefFile);

    if(isempty(ConfigFileDir))
      ConfigFileDir = '.';
    end

    OutputFile = [ConfigFileDir '/' ConfigFileBaseName '.mat'];
    DF = Translate( Dragonfly_BaseDir, MessageDefFile);
    save( OutputFile, 'DF');



function DF = Translate( Dragonfly_BaseDir, varargin)
    
    % Process argument list
    InputFilenames = {};
    for i = 1 : length( varargin)
        arg = varargin{i};
        % Make sure arguments are strings
        if( ~ischar(arg)), error( 'ReadDragonflyConfigFiles() only takes string arguments'); end
        InputFilenames(end+1) = {arg};
    end
    
    FilePaths = {};

    % If Dragonfly base directory provided, then add core definition file to the list
    if( ~isempty( Dragonfly_BaseDir))
        FilePaths(end+1) = {[Dragonfly_BaseDir '/include/Dragonfly_types.h']};
    end

    % Add user defined files to the list
    FilePaths = [FilePaths InputFilenames];
    
    % Parse the file list
    h = ParseHFile( FilePaths{:});

    DF.HID = []; % Host ID-s
    DF.MID = []; % Module ID-s
    DF.MT = [];  % Message Types
    DF.MDF = [];  % Message Data Formats

    % Go through the defines and find the host ID-s (HID), module ID-s (MID)
    % and message types (MT), then find the messgae data formats (MDF) in
    % the tyepdefs
    DF = ExtractFields( h.defines, DF);
    DF = ExtractFields( h.typedefs, DF);

    % Check that all HID, MID and MT values are numeric
    CheckNumericIDs( DF);
    
    % Check for duplicate message types
    CheckDupes( DF.MT);

    % Message header template for the ReadMessage and SendMessage functions
    DF.MESSAGE_HEADER = h.typedefs.DF_MSG_HEADER;

    % Create a lookup table to get Message Type Name by Message Type ID
    DF.MTN_by_MT = {};
    message_names = fieldnames( DF.MT);
    for i = 1 : length( message_names)
        message_name = message_names{i};
        message_type_id = getfield( DF.MT, message_name);
        DF.MTN_by_MT{message_type_id+1,1} = message_name;
    end

    % Create a lookup table to get Message Data Format by Message Type ID
    DF.MDF_by_MT = cell( size( DF.MTN_by_MT));
    message_names = fieldnames( DF.MDF);
    for i = 1 : length( message_names)
        message_name = message_names{i};
        % Make sure there is a matching MT field
        if( ~isfield( DF.MT, message_name))
            error( ['MDF_' message_name ' does not have a matching MT_' message_name ' defined']);
        end
        message_type_id = getfield( DF.MT, message_name);
        DF.MDF_by_MT{message_type_id+1,1} = getfield( DF.MDF, message_name);
    end

    % If Dragonfly base directory provided, then parse mex op-code definition file
    % to get the op-codes for calling the MatlabDragonfly mex file
    if( ~isempty( Dragonfly_BaseDir))
        MatlabDragonfly_h = ParseHFile( [Dragonfly_BaseDir '/lang/matlab/MatlabDragonfly.h']);
        DF.mex_opcode = MatlabDragonfly_h.defines;
    end
    
    % Put the raw parsed h-file content in the Dragonfly struct as well in case
    % we want to access some of the non-Dragonfly-specific defines
    DF.defines = h.defines;
    DF.typedefs = h.typedefs;
    DF.vars = h.vars;
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function DF = ExtractFields( fields, DF)

    names = fieldnames( fields);
    for n = 1 : length( names)
        name = names{n};
        [prefix, remainder] = strtok( name, '_');
        realname = remainder( 2:end);
        value = getfield( fields, name);

        % Check that HID, MID and MT fields are integer values
        switch( prefix)
            case {'HID','MID','MT'}
                if( value ~= int32( value))
                    error('HID_, MID_ and MT_ values should be integers');
                end
        end

        % Put the fields and their values in the appropriate fields of the Dragonfly
        % structure
        switch( prefix)
            case 'HID', DF.HID = setfield( DF.HID, realname, value);
            case 'MID', DF.MID = setfield( DF.MID, realname, value);
            case 'MT', DF.MT = setfield( DF.MT, realname, value);
            case 'MDF', DF.MDF = setfield( DF.MDF, realname, value);
        end
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function CheckNumericIDs( Struct)

    id_types = {'HID', 'MID', 'MT'};

    for t = 1 : length( id_types)
        id_type = id_types{t};
        Names = fieldnames( Struct.(id_type));
        Values = struct2cell( Struct.(id_type));
        for i = 1 : length( Values)
            value = Values{i};
            switch( class( value))
                case 'double'
                otherwise
                    error( ['Value for ' id_type '_' Names{i} ' is not numeric: "' value '"']);
            end
        end
    end 
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function CheckDupes( Struct)

    Names = fieldnames( Struct);
    ValuesCell = struct2cell( Struct);
    Values = [ValuesCell{:}];
    UniqueValues = unique( Values);
    NumValues = length( Values);
    NumUnique = length( UniqueValues);
    if( NumValues ~= NumUnique)
        sortedValues = sort(Values);
        idx = find(diff(sortedValues) == 0);
        error([ 'Duplicate message type ID-s found: ' num2str(sortedValues(idx)) ]);
    end
    
