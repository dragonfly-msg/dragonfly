function TranslateDragonflyhFiles2Unity( Dragonfly_BaseDir, MessageDefFile)
% TranslateDragonflyhFiles2dotNET( Dragonfly_BaseDir, MessageDefFile)
%
% Translates C header file and associated
% h files to C# and other .NET languages.
%
% Dragonfly Base directory (Dragonfly_BaseDir) needs to be specified so that it can find the core
% definition files. 
%
% MessageDefFile specifies the application-specific
% message definition file (e.g. '../../Source/Dragonfly_config.h').

    this_file_path = mfilename( 'fullpath');
    this_dir_path = fileparts( this_file_path);
    PATH = {[this_dir_path '/../matlab']};

    addpath( PATH{:});
    
    [ConfigFileDir, ConfigFileBaseName] = fileparts( MessageDefFile);
    if(isempty(ConfigFileDir))
      ConfigFileDir='.';
    end
    ConfigFile_BasePath = [ConfigFileDir '/' ConfigFileBaseName];
    Output_Namespace = 'Dragonfly';
    Translate( Dragonfly_BaseDir, ConfigFile_BasePath, Output_Namespace);

    rmpath( PATH{:});


function Translate( Dragonfly_BaseDir, config_file_basename, output_namespace)

    global IndentLevel;
    global IndentNumSpaces;

    IndentLevel = 0;
    IndentNumSpaces = 4;

    DF = ReadDragonflyhFiles( Dragonfly_BaseDir, [config_file_basename '.h']);

    Languages = {'C#'};
    FileExtensions = {'_Unity.cs'};
    for i = 1 : length( Languages)
        L = Languages{i};
        E = FileExtensions{i};
        f = fopen( [config_file_basename E], 'wt');
        WriteInclude(L, f);
        BeginNamespace( output_namespace, L, f);
        use_const = true;
        force_int = true;
        WriteClass( DF.MT, 'MT', L, f, use_const, force_int);
        WriteClass( DF.MID, 'MID', L, f, use_const, force_int);
        WriteClass( DF.HID, 'HID', L, f, use_const, force_int);
        WriteClass( DF.MDF, 'MDF', L, f);
        WriteClass( DF.MESSAGE_HEADER, 'MESSAGE_HEADER', L, f);
        WriteClass( DF.defines, 'defines', L, f, use_const);
        WriteClass( DF.typedefs, 'typedefs', L, f);
        %DF.MTN_by_MT
        %DF.MDF_by_MT
        EndNamespace( L, f);
        fclose( f);
    end


% ReadDragonflyConfigFiles( Dragonfly_BaseDir, File1, File2, ...)
%
% Reads core message definition files from the Dragonfly base directory (Dragonfly_BaseDir)
% plus optionally any additional user specified files to get the host and module id-s, and
% message type id-s, and message header and data structures. If Dragonfly_BaseDir is empty,
% then only reads the specified files.
%
% Meel Velliste
% 4/19/2006
% Amended 9/8/2008 to add flexibility to remove dependency on Dragonfly being installed
% in a fixed relative location to application modules. MV
function DF = ReadDragonflyhFiles( Dragonfly_BaseDir, varargin)
    
    % Process argument list
    InputFilenames = {};
    for i = 1 : length( varargin)
        arg = varargin{i};
        % Make sure arguments are strings
        if( ~ischar(arg)), error( 'ReadDragonflyhFiles() only takes string arguments'); end
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
    
    
function WriteInclude( language, f)
    global Indent;
    if ( strcmp( language, 'C#'))
        fprintf( f, [Indent 'using System.Runtime.InteropServices;\n' Indent '\n']);
    end
        
function BeginNamespace( name, language, f)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% BeginNamespace( name, language, f) - write a begin namespace
%
    global Indent;
    switch( language)
        case 'C++'
            fprintf( f, [Indent 'namespace ' name ' {\n' ...
                         Indent '\n']);
        case 'C#'
            fprintf( f, [Indent 'namespace ' name '\n' ...
                         Indent '{\n']);
        case 'VB', error( 'Sorry, VB output not implemented yet');
        otherwise, error( 'Unrecognized language');
    end
    IncreaseIndent( );


function EndNamespace( language, f)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% EndNamespace( language, f) - write an end namespace
%
    global Indent;
    DecreaseIndent( );
    switch( language)
        case 'C++', fprintf( f, [Indent '}\n\n']);
        case 'C#',  fprintf( f, [Indent '}\n\n']);
        case 'VB', error( 'Sorry, VB output not implemented yet');
        otherwise, error( 'Unrecognized language');
    end


function WriteEnum( enum_definitions, enum_name, language, f)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% WriteEnum( enum_definitions, enum_name, language, f) - Write an enumeration definition
%
% enum_definitions - a struct where field names are enum names and values are enum values
% enum_name - name of the enum in the output file
% language - a string that defines output language ("C++", "C#" or "VB")
% f - file handle of output file

    global Indent;

    % Begin the enum definition
    switch( language)
        case 'C++'
            fprintf( f, [Indent 'public enum class' enum_name '\n' ...
                         Indent '{\n']);
        case 'C#'
            fprintf( f, [Indent 'public enum ' enum_name '\n' ...
                         Indent '{\n']);
        case 'VB', error( 'Sorry, VB output not implemented yet');
        otherwise, error( 'Unrecognized language');
    end

    % Write the defined values
    IncreaseIndent( );
    names = fieldnames( enum_definitions);
    for i = 1 : length( names)
        name = names{i};
        value = enum_definitions.(name);
        if( value ~= int32(value)), error( 'Value not suitable for "enum"'), end
        if( i == length( names)), comma = ''; else comma = ','; end
        switch( language)
            case 'C++', fprintf( f, [Indent name ' = ' num2str(value) comma '\n']);
            case 'C#',  fprintf( f, [Indent name ' = ' num2str(value) comma '\n']);
        end
    end
    DecreaseIndent( );

    % End the enum definition
    switch( language)
        case 'C++', fprintf( f, [Indent '};\n\n']);
        case 'C#', fprintf( f, [Indent '}\n\n']);
    end


function WriteClass( member_definitions, class_name, language, f, use_const, force_int)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% WriteClass( member_definitions, class_name, language, f, const_int) - Write a class definition
%
% member_definitions - a struct whose field names are class member names and values serve as a template for the type definition of the member
% class_name - name of the class in the output file
% language - a string that defines output language ("C++", "C#" or "VB")
% f - file handle of output file
% use_const_int - optional argument. If present, and if "true", then all members will have type "const int" and the value will be written

    global Indent;
    global IndentLevel;

    if( ~exist( 'use_const', 'var'))
        use_const = false;
    end
    if( ~exist( 'force_int', 'var'))
        force_int = false;
    end

    % Begin the class definition
    switch( language)
        case 'C++'
            fprintf( f, [Indent 'ref class ' class_name '\n' ...
                         Indent '{\n' ...
                         Indent 'public:\n']);
        case 'C#'
            fprintf(f, [Indent '[StructLayout(LayoutKind.Sequential)]\n']);
            fprintf( f, [Indent 'public class ' class_name '\n' ...
                         Indent '{\n']);
        case 'VB', error( 'Sorry, VB output not implemented yet');
        otherwise, error( 'Unrecognized language');
    end

    % Write the defined values
    IncreaseIndent( );
    names = fieldnames( member_definitions);
    for i = 1 : length( names)
        name = names{i};
        value = member_definitions.(name);
        if( use_const)
            if( force_int)
                if( value ~= int32(value)), error( 'Value not suitable for "const int"'), end
                type = 'int';
            else
                if( IsInteger( value))
                    type = 'int';
                else
                    type = 'double';
                end
            end
            if( ischar( value))
                value(value=='"') = []; % Eliminate quote characters because they will not work inside C++ or C# source code
                switch( language)
                    case 'C++', fprintf( f, [Indent 'static const char ' name '[] = "' value '";\n']);
                    case 'C#',  fprintf( f, [Indent 'public const string ' name ' = "' value '";\n']);
                end
            else
                switch( language)
                    case 'C++', fprintf( f, [Indent 'static const ' type ' ' name ' = ' num2str(value) ';\n']);
                    case 'C#',  fprintf( f, [Indent 'public const ' type ' ' name ' = ' num2str(value) ';\n']);
                end
            end
        else
            switch( language)
                case 'C++', WriteMemberCPP( name, value, f);
                case 'C#',  WriteMemberCS( name, value, f);
            end
        end
    end
    DecreaseIndent( );

    % End the class definition
    if( IndentLevel <= 2), extra_newline = '\n'; else extra_newline = ''; end
    switch( language)
        case 'C++', fprintf( f, [Indent '};\n' extra_newline]);
        case 'C#',  fprintf( f, [Indent '}\n' extra_newline]);
    end


function WriteMemberCS( name, value, f)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% WriteMemberCS( name, value, f) - Write class member in C# notation
%
% name - Member name
% value - the type of the value is used to infer the .NET type

    global Indent;
    global IndentLevel;

    matlab_type = class( value);

    switch( matlab_type)
        case 'char'
            if( ~isempty(strfind( value, 'VARIABLE_LENGTH_ARRAY(')))
                matlab_type = value(23:end-1);
                value = [];
            else
                error( '"char" type not implemented');
            end
    end

    switch( matlab_type)
        case 'int8', type = 'sbyte';
        case 'int16', type = 'short';
        case 'int32', type = 'int';
        case 'int64', type = 'long';
        case 'uint8', type = 'byte';
        case 'uint16', type = 'ushort';
        case 'uint32', type = 'uint';
        case 'uint64', type = 'ulong';
        case 'double', type = 'double';
        case 'single', type = 'float';
        case 'struct'
            if( IndentLevel > 2)
                type = [name '_type'];
                WriteClass( value, type, 'C#', f);
                fprintf( f, [Indent 'public ' type ' ' name ' = new ' type '();\n\n']);
            else
                WriteClass( value, name, 'C#', f);
            end
            return
        otherwise, error( 'Unsupported type');
    end

    if( isempty( value)) % If value is empty, then it designates a variable length array
        fprintf( f, [Indent 'public ' type '[] ' name ';\n']);
    else

        if( ~isvector( value)), error( 'Matrices are not yet supported, value must be scalar or vector'); end
        vector_length = length( value);
        if( vector_length > 1)
            initializing_zeros = repmat('0,',[1 vector_length]);
            initializing_zeros(end) = []; % Eliminate last comma
            fprintf( f, [Indent '[MarshalAs(UnmanagedType.ByValArray, SizeConst = ' ...
                                num2str(vector_length) ')]\n']);
            fprintf( f, [Indent 'public ' type '[] ' name ' = {' initializing_zeros '};\n']);
        else
            fprintf( f, [Indent 'public ' type ' ' name ';\n']);
        end
    end

  
function WriteMemberCPP( name, value, f)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% WriteMemberCPP( name, value, f) - Write class member in C++ notation
%
% name - Member name
% value - the type of the value is used to infer the .NET type

    global Indent;

    matlab_type = class( value);

    switch( matlab_type)
        case 'char'
            if( ~isempty(strfind( value, 'VARIABLE_LENGTH_ARRAY(')))
                matlab_type = value(23:end-1);
                value = [];
            else
                error( '"char" type not implemented');
            end
    end

    switch( matlab_type)
        case 'int8', type = 'System::SByte';
        case 'int16', type = 'System::Int16';
        case 'int32', type = 'System::Int32';
        case 'int64', type = 'System::Int64';
        case 'uint8', type = 'System::Byte';
        case 'uint16', type = 'System::UInt16';
        case 'uint32', type = 'System::UInt32';
        case 'uint64', type = 'System::UInt64';
        case 'double', type = 'System::Double';
        case 'single', type = 'System::Single';
        case 'struct', WriteClass( value, name, 'C++', f); return
        otherwise, error( 'Unsupported type');
    end

    if( isempty( value)) % If value is empty, then it designates a variable length array
        fprintf( f, [Indent 'static array<' type '> ^' name ';\n']);
    else

        if( ~isvector( value)), error( 'Matrices are not yet supported, value must be scalar or vector'); end
        vector_length = length( value);
        if( vector_length > 1)
            initializing_zeros = repmat('0,',[1 vector_length]);
            initializing_zeros(end) = []; % Eliminate last comma
            fprintf( f, [Indent 'static array<' type '> ^' name ' = {' initializing_zeros '};\n']);
        else
            fprintf( f, [Indent 'static ' type ' ' name ';\n']);
        end
    end

        
function IncreaseIndent( )
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

global IndentLevel;
global IndentNumSpaces;
global Indent;

IndentLevel = IndentLevel + 1;
if( IndentLevel > 10), error( 'IndentLevel > 10'); end
Indent = repmat( ' ', [1 IndentLevel*IndentNumSpaces]);


function DecreaseIndent( )
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

global IndentLevel;
global IndentNumSpaces;
global Indent;

IndentLevel = IndentLevel - 1;
if( IndentLevel < 0), error( 'IndentLevel < 0'); end
Indent = repmat( ' ', [1 IndentLevel*IndentNumSpaces]);


function is_integer = IsInteger( value)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

    if( value == int32(value))
        is_integer = true;
    else
        is_integer = false;
    end
    