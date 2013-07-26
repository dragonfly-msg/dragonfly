% H = ParseHFile( FILENAME)
%
% Parses the '#define'-s, 'typedef'-s and variable declarations
% from a .h file. The output, H, is a structure containing the fields
% 'defines', 'typedefs' and 'vars', which in turn contain structures
% with a field for each defined constant, variable or datatype.
% Numeric or string values are supported for '#define'-d constants.
% Simple datatypes as well as structures are supported for variable
% declarations and 'typedef'-s.
%
% Meel Velliste 4/11/2006
% Emrah Diril 5/1/2013
%
% Copyright (c) 2006 by Meel Velliste, University of Pittsburgh
% All rights reserved.

function h = ParseHFile( varargin)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Here's how it works: First, lines that end with the continuation
% character '\' are concatenated, then comments eliminated. After that,
% the .h file is considered to contain two types of entries:
% 1) Compiler directives: #<tag> [<text>]\n
% 2) Expressions: <tag> [<text1>] [{<text2>}] <text3>;
%
% Compiler directives are broken down as:
% 1) Defines: #define <tag> [<text>]\n
% 2) Other - ignore
%
% Expressions are broken down as:
% 1) Type definition: typedef <declaration>;
% 2) Declaration: <type_specifier> [*] <name_tag> [<array_specifier>];
%
% Type specifiers are broken down as:
% 1) Structure: struct [<tag>] { <declaration>; [<declaration>;...]}
% 2) Simple type: [<signedness_tag>] <simpletype_tag>
% 3) Previously typedef-d type: use lookup table of typedefs

    % Initialize output structures
    if( isstruct( varargin{1}))
        h = varargin{1};
        saved_state = SaveState( h);
        h.recursion_level = h.recursion_level + 1;
        if( h.recursion_level > 10), error( 'Recursion level > 10'); end
        recursive = true;
        first_filename_arg = 2;
    else
        % Output fields of "h"
        h.defines = [];
        h.define_strings.names = {};
        h.define_strings.values = {};
        h.typedefs = [];
        h.vars = [];
        % Local variables
        recursive = false;
        first_filename_arg = 1;
    end

    % Temporary state variables are kept in "h", and removed before
    % outputting "h".
    h = InitializeStateVariables( h);

    % For each input file
    for n = first_filename_arg : nargin

        % Read the .h file(s) into a char array
        filepath = varargin{n};

        % The ReadText function makes sure that only
        % linefeed characters are used for newlines.
        % This makes it easier for regular expressions, linefeed
        % is matched by \n
        h_file_text = ReadText( filepath);
        if isempty(h_file_text), continue;     end

        % Make sure to keep the file's path in the h structure so that
        % included files can be included relative to this file's directory
        h.filepath = filepath;

        % The "text" field will represent the remaining text to be parsed.
        % Each parsing function will update this field by removing the part
        % that it parsed.
        h.text = h_file_text;

        try
            % Preprocess to concatenate continued lines and remove comments
            h = PreProcess( h);

            % If this h-file is being processes as an include from another
            % file, replace defined contants in this file that came from
            % the other file
            if( recursive)
                h = ReplaceAllDefined( h);
            end

            % Parse the text
            h = DoParsing( h);

        catch
            L = lasterror;
            L.message = sprintf( '%s\nError in parsing %s\n', L.message, filepath);
            rethrow( L);
        end
    end

    if( recursive)
        % The state variables in h are only local to this level of recursion,
        % so restore the state fields from the previous level of recursion
        h = RestoreState( h, saved_state);
    else
        % Clear the fields in "h" that hold state information because they
        % are not meant for output
        h = ClearStateVariables( h);
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = InitializeStateVariables( h)

    h.filepath = [];
    h.text = [];
    if( ~isfield( h, 'recursion_level'))
        h.recursion_level = 0;
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = ClearStateVariables( h)

    h = rmfield( h, 'filepath');
    h = rmfield( h, 'text');
    h = rmfield( h, 'recursion_level');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function s = SaveState( h)

    s.filepath = h.filepath;
    s.text = h.text;
    s.recursion_level = h.recursion_level;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = RestoreState( h, s)

    h.filepath = s.filepath;
    h.text = s.text;
    h.recursion_level = s.recursion_level;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = ReplaceAllDefined( h)

    % Replace defined constants in the text
    for i = 1 : length( h.define_strings.names)
        define = [];
        define.name = h.define_strings.names{i};
        define.value = h.define_strings.values{i};
        h.text = replace_defined( h.text, define);
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = PreProcess( h)

    % Concatenate lines continued using \
    concat_text = regexprep( h.text, '\\\n', '');

    % Get rid of text that is commented out
    cpp_style_comments = '//.*?\n'; % i.e. anything between // and \n
    c_style_comments = '/\*.*?\*/'; % i.e. anything between /* and */
    no_comment_text = regexprep( concat_text, ...
        [cpp_style_comments '|' c_style_comments], '\n');
    h.text = [no_comment_text sprintf( '\n')];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = DoParsing( h, expected_directives)

    if( ~exist( 'expected_directives', 'var'))
        expected_directives = {};
    end

    found_directive = [];
    while( ~isempty( h.text))

        token = regexp( h.text, '\S+', 'match', 'once');
        if( isempty( token)), break; end
        context = get_context( h);
        try

            [h, found_directive] = ParseNextItem( h, token, expected_directives);

            % If we found an expected directive, then we need to quit this level of
            % recursion
            if( ~isempty( found_directive)) break; end

        catch
            fprintf( '\n--------------- Error context in %s: -------------\n%s\n', h.filepath, context);
            rethrow( lasterror);
        end

    end

    if( ~isempty( expected_directives))
        if( isempty( found_directive))
            error( 'End of file reached while looking for an #else or an #endif');
        end
    end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [h, found_directive] = ParseNextItem( h, token, expected_directives)

    if( strcmp( token, 'using'))
        NotYetImplemented( 'using');
    end
    if( token(1) == '#')
        [h, found_directive] = ParseCompilerDirective( h, expected_directives);
    else
        h = ParseExpression( h);
        found_directive = [];
    end


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = ParseIfDirective( tag, condition, h)

    % Figure out whether the "if" condition is true
    switch( tag)
        case {'ifdef','ifndef'}
            if( strmatch( condition, h.define_strings.names, 'exact'))
                name_defined = true;
            else
                name_defined = false;
            end
        case 'if',
            % Check of "if" conditions not implemented yet. Assume always
            % false
            condition_satisfied = false;
        otherwise, error( 'Unknown type of "if" directive');
    end

    switch( tag)
        case 'ifdef', condition_satisfied = name_defined;
        case 'ifndef', condition_satisfied = ~name_defined;
    end

    % Look for the matching #else and/or #endif, parsing the code in
    % between depending of whether the condition above was satisfied
    if( condition_satisfied)
        h = DoParsing( h, {'else','endif'});
    else
        h = IgnoreUptoMatchingElseOrEndif( h, 'expect_else');
    end



%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = IgnoreUptoMatchingElseOrEndif( h, how_to_treat_else)

    endif_found = false;
    while( ~isempty( h.text))
        token = regexp( h.text, '#[ \t]*\S+', 'match', 'once');
        if( isempty( token))
            break;
        else
            query = '#[ \t]*(?<tag>\w*).*?\n';
            [tokens, endidx] = regexp( h.text, query, 'names', 'end', 'once');
            h = consume_text( h, endidx);
            if( isempty( tokens)), error( 'Invalid compiler directive'); end
            switch( tokens.tag)
                case {'ifdef','ifndef','if'}
                    h = IgnoreUptoMatchingElseOrEndif( h, 'ignore_else');
                case 'elif'
                    NotYetImplemented( '#elif');
                case 'else'
                    switch( how_to_treat_else)
                        case 'ignore_else'
                        case 'expect_else', h = DoParsing( h, {'endif'}); endif_found = true; break;
                        case 'else_already_found', error( 'Found second #else within the same #ifdef/#ifndef block');
                        otherwise, error( ['Unexpected value for "how_to_treat_else": [' how_to_treat_else ']']);
                    end
                case 'endif'
                    endif_found = true;
                    break;
            end
        end
    end

    if( ~endif_found)
        error( 'End of file reached while looking for an #endif');
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Compiler directive: #<tag> [<text>]\n
function [h, found_directive] = ParseCompilerDirective( h, expected_directives)

    % A compiler directive consists of a # sign followed by optional white-
    % space, a tag consisting of alphanumeric characters, some more optional
    % whitespace, then some text (e.g. the name and value of a #define),
    % followed by more optional whitespace, and finally a newline
    query = '#[ \t]*(?<tag>\w*)[ \t]*(?<text>.*?)[ \t]*\n';
    [tokens, endidx] = regexp( h.text, query, 'names', 'end', 'once');
    if( isempty( tokens)), error( 'Invalid compiler directive'); end
    h = consume_text( h, endidx);
    found_directive = [];
    switch( tokens.tag)
        case 'define',
            h = ParseDefineText( tokens.text, h);
        case 'include',
            h = ParseInclude( tokens.text, h);
        case {'ifdef','ifndef','if'},
            h = ParseIfDirective( tokens.tag, tokens.text, h);
        case 'elif', NotYetImplemented( '#elif');
        case 'else',
            VerifyExpectedDirective( 'else', expected_directives);
            h = IgnoreUptoMatchingElseOrEndif( h, 'else_already_found');
            found_directive = 'endif';
        case 'endif',
            VerifyExpectedDirective( 'endif', expected_directives);
            found_directive = 'endif';
        case 'pragma', NotYetImplemented( '#pragma');
        case 'error', error( ['#error: ' tokens.text]);
        otherwise
            error_msg = ['Invalid compiler directive tag <#' tokens.tag '>'];
            error( error_msg);
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function VerifyExpectedDirective( directive, expected_directives)

    if( isempty( expected_directives))
        error( 'Unexpected #%s (not preceded by #ifdef or #ifndef or #if)', directive);
    else
        if( strmatch( directive, expected_directives, 'exact'))
        else
            error( 'The found directive "%s" does not match expected directives', directive);
        end
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = ParseDefineText( text, h)

    % Look for a name followed by a value string
    % Make sure the name uses valid alphanumeric characters that can be
    % used as a field name in matlab (i.e. has to start with a letter).
    % Names starting with _ are allowed in C but not in matlab, so
    % they will not be allowed
    query = ['(?<name>' ID ')\s*(?<value>\s*.*\s*)'];
    [define, endidx] = regexp( text, query, 'names', 'end', 'once');
    if( isempty( define) || isempty( define.name))
        error( ['Identifier expected after #define, instead found: ', text]);
    end

    % Replace defined constant in the following text
    h.text = replace_defined( h.text, define);
    % Put the name and string value of the defined constant in the output array h
    match_idx = strmatch( define.name, h.define_strings.names, 'exact');
    if( isempty( match_idx))
        h.define_strings.names(end+1,1) = {define.name};
        h.define_strings.values(end+1,1) = {define.value};
    else
        % If constant already defined, replace existing one instead of
        % adding new one
        h.define_strings.names(match_idx,1) = {define.name};
        h.define_strings.values(match_idx,1) = {define.value};
    end


    % If name starts with underscore, then we cannot put it in the h.defines
    % structure because field names are not allowed to start with underscore.
    if( define.name(1) ~= '_')
        % Turn the value into a number if it is numeric or if it is an arithmetic
        % expression. If it is not numeric, then it will remain a string value
        value = ConvertToNumber( define.value);
        % Put the defined name and the converted value in the output array h
        h.defines.(define.name) = value;
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function value = ConvertToNumber( str)

    if( regexp( str, '^0x[0-9A-Fa-f]+$')) %If hexadecimal
        value = sscanf( str, '%x');
    else
        value = str2double( str); % If a decimal number
        if( isnan( value)),
            % If an arithmetic expression, evaluate and turn into a number
            if( regexp( str, '[A-Za-z]', 'once')) % If it has letters, it is not an arithmetic expression
                value = str;
            else
                value = str2num( str);
                if( isempty( value)) % If expression did not evaluate to a number, leave as a string
                    value = str;
                end
            end
        end
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Replace defined identifiers in the text with their defined values
function text = replace_defined( text, define)

    % Pad the text with spaces to make it easier for the regular
    % expressions
    padded_text = [' ' text ' '];

    % First protect the defined names in #ifdef and #ifndef directives
    % by replacing them with something else so they won't be replaced by
    % the value of the defined constant
    search_pattern = ['(#ifn?def|#define)[ \t]+(' define.name '\s)'];
    replace_pattern = ['$1_$2'];
    reptext = regexprep( padded_text, search_pattern, replace_pattern);

    % Then replace the defined names with their value
    search_pattern = ['(\W)' define.name '(\W)'];
    replace_pattern = ['$1 ' define.value '$2'];
    reptext = regexprep( reptext, search_pattern, replace_pattern);

    % Restore the names in #ifdef and #ifndef directives
    search_pattern = ['(#ifn?def|#define)_(' define.name '\s)'];
    replace_pattern = ['$1 $2'];
    reptext = regexprep( reptext, search_pattern, replace_pattern);

    % Remove the padding spaces
    text = reptext(2:end-1);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = ParseInclude( text, h)

    include = regexp( text, '"(?<filename>.+)"', 'names', 'once');
    if( isempty( include)), error( 'Invalid filename in #include directive'); end
    % Figure out if the path name is absolute or relative
    if( regexp( include.filename, '^[A-Za-z]:[\\/]|^[\\/]'))
        % File path is absolute
        filepath = include.filename;
    else
        % Figure out the current file's directory name so that #include-d
        % files can be read from the correct directory relative to this file
        dirname = fileparts( h.filepath);
        if( ~isempty( dirname))
            dirname = [dirname '/'];
        end
        filepath = [dirname include.filename];
    end

    % Parse the included file recursively
    h = ParseHFile( h, filepath);
    h = ReplaceAllDefined( h);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Expression: <tag> <text>;
function h = ParseExpression( h)

    % Expressions are broken down as:
    % 1) Type definition: typedef <declaration>;
    % 2) Declaration: <declaration>;
    endidx = regexp( h.text, '\s*typedef', 'end', 'once');
    if( isempty( endidx))
        typedef = 0;
    else
        typedef = 1;
        h = consume_text( h, endidx);
    end
    h = ParseDeclaration( h, typedef);


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Declaration: <type_specifier> [*] <name_tag> [<array_specifier>];
function h = ParseDeclaration( h, typedef)

    % First look for a type identifier
    query = ['^\s*(?<type>' ID ')'];
    [decl, endidx] = regexp( h.text, query, 'names', 'end', 'once');
    if( isempty( decl)), error( 'Type identifier not found in declaration'); end
    h = consume_text( h, endidx);
    type = decl.type;
    switch( decl.type)
        % If it is a 'signed' or 'unsigned', figure out if there is a
        % secondary type specifier, such as 'char', 'short' or 'int'.
        % if not, then assume 'int'.
        case {'signed','unsigned'}
            query = ['^\s*(?<type>' ID ')'];
            [decl2, endidx] = regexp( h.text, query, 'names', 'end', 'once');
            if( isempty( decl2)), error( 'Secondary type identifier or identifier name not found in declaration'); end
            if( strmatch( decl2.type, {'char','short','int','long'}))
                h = consume_text( h, endidx);
                type = [type ' ' decl2.type];
            else
                type = [type ' int'];
            end
    end
    switch( type)
        case 'struct'
            [h, value] = ParseStruct( h);
            [h, name, value] = ParseDeclarationTail( h, type, value);
        case 'union'
            NotYetImplemented( 'union');
        case 'enum'
            [h, value] = ParseEnum( h);
            [h, name, value] = ParseDeclarationTail( h, type, value);
        otherwise
            value = '';
            [h, name, value] = ParseDeclarationTail( h, type, value);
    end

    if( typedef)
        h.typedefs.(name) = value;
    else
        h.vars.(name) = value;
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Look for the remainder of the type declaration: [*] <name_tag> [<extra>];
function [h, name, value] = ParseDeclarationTail( h, type, value)

    query = ['^\s*(?<ptr>\*?)\s*(?<name>' ID ')\s*(?<extra>[^;]*)\s*;'];
    [decl, endidx] = regexp( h.text, query, 'names', 'end', 'once');
    if( isempty( decl)), error( 'Identifier name not found in declaration'); end
    h = consume_text( h, endidx);
    name = decl.name;
    if( isempty( name))
        error( 'No identifier token found while expecting variable, type or field name');
    end
    if( name(1) == '_')
        error( 'Names starting with underscore are not allowed in matlab');
    end
    switch( decl.ptr)
        % A pointer variable is just a 4-byte value, so use unsigned int
        case '*', type = 'unsigned int';
    end
    value = typecast( value, type, h);
    % Look for an array specifier
    if( ~isempty( decl.extra))
        query = '^\s*\[\s*(?<size>.*?)\s*\]';
        array_spec = regexp( decl.extra, query, 'names', 'once');
        if( ~isempty( array_spec))
            if( isempty( array_spec.size))
                value = ['VARIABLE_LENGTH_ARRAY(' class( value) ')'];
            else
                array_size = ConvertToNumber( array_spec.size);
                if( ~isnumeric( array_size)), error(['Invalid array size specifier: [' array_spec.size ']']); end
                value = repmat( value, [1 array_size]);
            end
        else
            error( 'Expected ";" after "%s"\nInstead found "%s"', name, decl.extra);
        end
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Structure: struct [<tag>] { <declaration>; [<declaration>;...]}
function [h, structure] = ParseStruct( h)

    % First, find and consume the opening {
    endidx = regexp( h.text, '\{', 'end', 'once');
    if( isempty( endidx)), error( '"{" not found after "struct"'); end
    h = consume_text( h, endidx);

    % Then keep processing declarations until }
    end_of_struct = 0;
    s.vars = [];
    s.typedefs = h.typedefs;
    s.text = h.text;
    while( ~end_of_struct)
        % Parse a struct field declatation
        s = ParseDeclaration( s, 0);

        % See if it is the end of struct yet
        [match, endidx] = regexp( s.text, '\s*(?<token>\S)', 'names', 'end', 'once');
        if( isempty( match)), error( 'Unexpected end of file while parsing struct'); end
        switch( match.token)
            case '}', end_of_struct = 1;
        end
    end
    s = consume_text( s, endidx);
    h.text = s.text;
    structure = s.vars;

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Enum: enum [<tag>] { <declaration>, [<declaration>,...]}
function [h, enum] = ParseEnum( h)

    % First, find and consume the opening {
    endidx = regexp( h.text, '\{', 'end', 'once');
    if( isempty( endidx)), error( '"{" not found after "enum"'); end
    h = consume_text( h, endidx);

    %for now- just ignore enums
    endidx = regexp( h.text, '\}', 'end', 'once');
    if( isempty( endidx)), error( '"}" not found after "enum"'); end
    h = consume_text( h, endidx);
    enum = '';

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Typecast value to appropriate matlab type based on the C type
function value = typecast( value, type, h)

    switch( type)
        case 'struct', return;
        case 'enum', return;
    end

    if( isempty( value))
        value = 0;
    else
        value = str2double( value);
        if( isnan( value)), error( 'value is not numeric'); end
    end

    switch( type)
        case {'char', 'signed char'}, value = int8( value);
        case {'unsigned char'}, value = uint8( value);
        case {'short', 'signed short'}, value = int16( value);
        case {'unsigned short'}, value = uint16( value);
        case {'int', 'signed int'}, value = int32( value);
        case {'unsigned int'}, value = uint32( value);
        case {'double'}, value = double( value);
        case {'float'}, value = single( value);
        otherwise,
            % Look for a defined type
            if( isfield( h.typedefs, type))
                type_value = h.typedefs.(type);
                if( ischar( type_value) && strmatch( 'VARIABLE_LENGTH_ARRAY', type_value))
                    value = type_value;
                else
                    if( isnumeric( type_value))
                        type_class = class( type_value);
                        value = feval( type_class, value);
                    else
                        value = type_value;
                    end
                end
            else
                error( ['Undefined type name: ' type]);
            end
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Get initialization value of a variable, e.g. '= 34'
function strvalue = get_initialization( initialization_str)

    init = regexp( initialization_str, '^\s*(?<equals>=?)\s*(?<value>[0-9]*)', 'names', 'once');
    if( isempty( init.equals) && isempty( init.value))
        strvalue = '';
    else
        switch( init.equals)
            case '=',
                if( isempty( init.value))
                    error( 'initialization value expected');
                else
                    strvalue = init.value;
                end
            otherwise, error( 'Equal sign expected');
        end
    end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Read a text file into an array of characters. Each line gets read into a
% separate array first, and then they get re-assembled into a single text.
% This makes sure that only Line Feed characters (ASCII value 10) are used
% for newlines.

function text = ReadText( filename)

    if( ~exist( filename, 'file'))
        %error(['ParseHFile: Could not find file: ' filename '. Current dir is ' pwd]);
        disp(['ParseHFile: Could not find file: ' filename '. Current dir is ' pwd]);
        text = '';
        return;
    end

    f = fopen( filename, 'rt');
    lines = {};
    while( ~feof( f))
        line = fgetl( f);
        if( line == -1)
            line = '';
        end
        lines{end+1,1} = line;
    end
    fclose( f);
    text = sprintf( '%s\n', lines{:});

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function NotYetImplemented( str)

    error( '"%s" not yet implemented', str);

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Get context for error printouts
function context = get_context( h)

    endidx = length( h.text);
    context = h.text(1:min(100,endidx));

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function h = consume_text( h, endidx)

    h.text(1:endidx) = [];

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function identifier_pattern = ID

    identifier_pattern = '[a-zA-Z_]\w*';
