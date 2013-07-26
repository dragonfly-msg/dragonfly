function Log = LoadMessageLog( Filename, DF)

% Log = LoadMessageLog( Filename, DF)
%
% Loads the binary Dragonfly message log in Filename and converts it to a matlab 
% data structure, organized by message type, so it is easy to look at
% data associated with any particular message type.
% Dragonfly is a structure containing Dragonfly message definitions (saved from a
% matlab module at runtime when Filename was recorded.

% Meel Velliste 12/29/2008

RawLog = LoadRawMessageLog( Filename, DF);
Log = OrganizeLogByMsgType( RawLog, DF);
