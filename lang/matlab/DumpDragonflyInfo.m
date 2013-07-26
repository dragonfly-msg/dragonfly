function DumpDragonflyInfo( Dragonfly_BaseDir, MessageConfigFile)

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% DumpDragonflyInfo( Dragonfly_BaseDir, MessageConfigFile)
%
%   Connects to MMM and dumps the entire Dragonfly struct as structured text
%   file
%   C modules can read the Dragonfly struct as DD, and make use of it
%
%   Sagi Perel
%   11/21/2006, University of Pittsburgh


%
% Initialization
%
addpath ../../MatlabCommon

DF = ReadDragonflyConfigFiles(Dragonfly_BaseDir, MessageConfigFile);
SaveTextData('../Dragonfly_config_dump.txt', 'DF');
%SaveDD('../Dragonfly_config_dump.txt', 'DF');


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
exit