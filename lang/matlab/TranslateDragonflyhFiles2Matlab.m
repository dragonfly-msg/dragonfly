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
    DF = ReadConfigFiles( Dragonfly_BaseDir, MessageDefFile);
    save( OutputFile, 'DF');


