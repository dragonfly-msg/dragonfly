function build_matlab_message_defs(header_filename)

% build_matlab_message_defs(header_filename)
%
% header_filename is the name of the C header file to be translated and
% includes full path address.
%
% Meel Velliste 8/10/2008
% Emrah Diril 9/10/2011

    % Parse h-file definitions
    DragonflyPath = getenv('DRAGONFLY');
    addpath( [DragonflyPath '/lang/matlab/']);
    TranslateDragonflyhFiles2Matlab(DragonflyPath, header_filename)
