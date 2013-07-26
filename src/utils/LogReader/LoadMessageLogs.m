function [BigLog, IncrState] = LoadMessageLogs( DirName, FileNames, DF, IncrState)

% BigLog = LoadMessageLogs( DirName, FileNames, DF)
%
% Loads the binary Dragonfly message logs for each file name in the FileNames
% cell array (where these files are in the directory DirName) and converts
% them to a matlab data structure, organized by message type, so it is easy
% to look at data associated with any particular message type.
% Dragonfly is a structure containing Dragonfly message definitions (saved from a
% matlab module at runtime when message logs were recorded.

% Meel Velliste 12/29/2008
% Emrah Diril  10/14/2011

	% make the function backwards compatible
    if (nargin < 4)
        IncrState = struct('LastSequenceNo', 0, ...
                           'LastFileIndex', 0);
	end
						   
    % If there is a slash at the end of DirName, remove it, so that the
    % code below can treat the directory name in a uniform manner
    if( DirName(end) == '/')
        DirName(end) = [];
    end

    % Process all input files and gather individual message logs
    % into an array of message logs
    Logs = [];
    LastSequenceNo = IncrState.LastSequenceNo;
    num_chars_disp = 0;
    for i = 1 : length( FileNames)
        % Show progress indicator
        fprintf('%s', [sprintf('%c',repmat( char(8), [1 num_chars_disp])) sprintf('%3.0f%%',(100*i/(length(FileNames)+2)))]);
        num_chars_disp = 4;
        % Load message log from the binary file
        input_file = FileNames{i};
        input_file_path = [DirName '/' input_file];
        Log = LoadMessageLog( input_file_path, DF);
        % Offset message sequence numbers so that they form one continuous
        % sequence in the concatenated log (instead of starting over from 1
        % for each file)
        [Log.SequenceNo, LastSequenceNo] = OffsetSequenceNos( Log.SequenceNo, LastSequenceNo);
        % Create an index that allows us to track which file each message
        % came from
        Log.FileIndex = CreateFileIndex( Log.SequenceNo, IncrState.LastFileIndex+i);
        % Put log into a big array of logs
        Logs = [Logs Log];
    end

    % save the new LastSequenceNo for later incremental processing
    IncrState.LastSequenceNo = LastSequenceNo;
    
    % Concatenate the fields in all the logs, so it becomes
    % a single log (a single struct where the fields at the
    % deepest level are concatenated)
    BigLog = CatStructFields( Logs, 'horizontal', 'merge-fields');
    fprintf('%s', [sprintf('%c',repmat( char(8), [1 num_chars_disp])) sprintf('%3.0f%%',100)]);
    fprintf('\n');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function Index = CreateFileIndex( Template, FileNo)

    Index = Template;
    MsgNames = fieldnames( Index);
    for i = 1 : length( MsgNames)
        Index.(MsgNames{i})(:) = FileNo;
    end
    
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
function [SeqNos, LastSeqNo] = OffsetSequenceNos( SeqNos, LastSeqNo)
    
    MsgNames = fieldnames( SeqNos);
    last_seq_no = 0;
    for i = 1 : length( MsgNames)
        msg_name = MsgNames{i};
        lsn = SeqNos.(msg_name)(end); % Last sequence number of current message type
        % Look for the last sequence number locally in the current log
        if( lsn > last_seq_no)
            last_seq_no = lsn;
        end
        % Increment sequence numbers by the global last sequence number, so
        % that the current log continues where the previous one left off
        SeqNos.(msg_name) = SeqNos.(msg_name) + LastSeqNo;
    end
    % Increment the global last sequence number by the local one, so that
    % the next log may continue where this one left off
    LastSeqNo = LastSeqNo + last_seq_no;
    