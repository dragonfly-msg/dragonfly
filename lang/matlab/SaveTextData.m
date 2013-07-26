% SaveTextData( FILENAME, VAR1, VAR2, ...)
%
% Save the matlab variables VAR1, VAR2 etc. in a structured text file
% FILENAME. This function is kind of like the matlab "save" function,
% except it writes the data in text format. The variables
% can be (almost) any type of matlab variable, including numeric matrix,
% char string, struct or cell array. Struct and cell arrays can have any
% level of nesting.
%
% Up to 2-dimensional numeric arrays, 1-dimensional char arrays, and
% 1-dimensional cell arrays are currently supported.
%
% Meel Velliste
% 5/12/2006

function SaveTextData( filename, varargin)

    appending = 0;
    options_arg_idx = [];
    % Look for options arguments
    num_varargs = length( varargin);
    for i = 1 : num_varargs
        arg = varargin{i};
        switch( arg)
            case '-begin_var' % Begin a variable, but do not close it
                options_arg_idx = [options_arg_idx i];
            case '-append'
                appending = 1;
                options_arg_idx = [options_arg_idx i];
            case '-end_var' % Close the variable that was begun using -begin_var
                options_arg_idx = [options_arg_idx i];
            case '-index'
                index = varargin{i+1};
                options_arg_idx = [options_arg_idx i i+1];
        end
    end
    % Delete options arguments, so we are left with just variable names in
    % varargin
    varargin(options_arg_idx) = [];


    %
    % Find out if we are appending or writing a new file
    %
    if( appending)
        f = fopen( filename, 'at');
    else
        f = fopen( filename, 'wt');
        % If not appending then write header
        FORMAT_VERSION = 'v1.2';
        WriteHeader( f, FORMAT_VERSION);
    end
    if( f < 0)
        error( ['SaveTextData: Could not open the file "' filename '"']);
    end
    
    %
    % Get the data from the caller's workspace and write it
    %
    recursion_level = 0;
    num_variables = length( varargin);
    data = [];
    for i = 1 : num_variables
        variable_name = varargin{i};
        variable = evalin( 'caller', variable_name);
        if( appending)
            indexed_variable_name = [variable_name '_' num2str(index)];
            data.(indexed_variable_name) = variable;
        else
            data.(variable_name) = variable;
        end
    end
    WriteStruct( f, data, recursion_level);
    
    fclose( f);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Write a header that specifies the version of the data format. This will
% help future versions of the LoadTextData function to be backward
% compatible with older data files
function WriteHeader( f, FORMAT_VERSION)

    fprintf( f, 'StructuredTextData  %s\n', FORMAT_VERSION);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Figure out what kind of data we are dealing with, then call the
% appropriate function to format and write the data
function [recursion_level] = WriteData( f, data, recursion_level)

    if( isnumeric( data))

        if( ndims( data) == 2 & size( data, 1) == 1)
            WriteNumericRow( f, data);
        else
            WriteNumericMatrix( f, data, recursion_level);
        end

    else

        data_class = class( data);
        switch( data_class)
            case 'char',   WriteString( f, data);
            case 'struct', recursion_level = WriteStruct( f, data, recursion_level);
            case 'cell',   recursion_level = WriteCell( f, data, recursion_level);
            otherwise, error( ['Data of type "' data_class '" not supported']);
        end
    end
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Write a numeric row
function WriteNumericRow( f, data)

    data_class = class( data);
    switch( data_class)
        case {'single','double'}
            if( round( data) == data)
                format = '%.0f ';
            else
                format = '%.2f ';
            end
        case {'int8','uint8','int16','uint16','int32','uint32'}
            format = '%i ';
        otherwise
            error( ['Unsupported numeric datatype: "' data_class '"']);
    end
    fprintf( f, format, data);
    fprintf( f, '\n');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Write a string
function WriteString( f, data)

    if( ndims( data) > 2 | size( data, 1) > 1)
        error( '"char" data is only supported in the form of 1xN strings');
    end
    
    fprintf( f, '%s\n', data);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Write a numeric matrix row-by-row
function WriteNumericMatrix( f, data, recursion_level)

    if( ndims( data) > 2)
        error( 'N-dimensional numeric arrays not supported');
    end
    
    fprintf( f, ' [\n');
    num_rows = size( data, 1);
    for r = 1 : num_rows
        PrintIndent( f, recursion_level);
        fprintf( f, '   ');
        row = data(r,:);
        WriteNumericRow( f, row);
    end
    PrintIndent( f, recursion_level);
    fprintf( f, ']\n');
    

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Write a struct
function [recursion_level] = WriteStruct( f, data, recursion_level)

    if( ndims( data) > 2 | size( data, 1) > 1 | size( data, 2) > 1)
        error( 'Struct arrays are not supported, only 1x1 structs are supported');
    end

    recursion_level = recursion_level + 1;
    namespace = 18;

    % Make a special case for the first-level structure because the data as
    % a whole is considered to be a structure of variables. By not
    % requiring the delimiting ":" and ";" at the beginning and end of the
    % structure, we gain the ability to append new variables (fields) to the
    % top-level structure without having to worry about the ";" at the end
    if( recursion_level > 1)
        fprintf( f, ':\n');
    end
    
    names = fieldnames( data);
    values = struct2cell( data);
    numfields = length( names);
    for i = 1 : numfields
        %
        % Print appropriate level of indenting
        %
        PrintIndent( f, recursion_level);
        %
        % Print field name
        %
        name = names{i};
        fprintf( f, '%s ', name);
        num_padding_spaces = namespace - length( name);
        fprintf( f, repmat( ' ', [1 num_padding_spaces]));
        %
        % Print field value
        %
        value = values{i};
        recursion_level = WriteData( f, value, recursion_level);
    end
    
    if( recursion_level > 1)
        PrintIndent( f, recursion_level-1);
        fprintf( f, ';\n');
    end
    
    recursion_level = recursion_level - 1;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Write a cell array
function [recursion_level] = WriteCell( f, data, recursion_level)

    if( ndims( data) > 2 | (size( data, 1) > 1 & size( data, 2) > 1))
        error( 'Cell arrays are only supported in the form of Nx1 or 1xN');
    end
    
    recursion_level = recursion_level + 1;

    fprintf( f, '{\n');
    
    num_elements = length( data);
    for i = 1 : num_elements
        PrintIndent( f, recursion_level);
        element = data{i};
        recursion_level = WriteData( f, element, recursion_level);
    end
    
    PrintIndent( f, recursion_level-1);
    fprintf( f, '}\n');
    
    recursion_level = recursion_level - 1;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Print the correct amount of indenting for the given recursion level
function PrintIndent( f, recursion_level)

indent_size = 4 * (recursion_level - 1);
indent = repmat( ' ', [1 indent_size]);
fprintf( f, indent);
