function build_unity_message_defs(header_filename)

% build_unity_message_defs(header_filename)
%
% header_filename is the name of the C header file to be translated and
% includes full path address.
%
% Meel Velliste 8/10/2008
% Emrah Diril 9/10/2011
% Hongwei Mao 1/4/2019

    DragonflyPath = getenv('DRAGONFLY');
    addpath( [DragonflyPath '/lang/unity/']);
    TranslateDragonflyhFiles2Unity(DragonflyPath, header_filename)
