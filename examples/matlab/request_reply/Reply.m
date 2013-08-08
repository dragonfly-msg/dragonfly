function Reply(mm_ip)

    global DF;

    Dragonfly_BaseDir = '../../..';
    addpath([Dragonfly_BaseDir '/lang/matlab']);

    Dragonfly_BaseDir

    MessageTypes =  { ...
                        'REQUEST_TEST_DATA' ...
                        'EXIT' ...
                    };

    ConnectArgs = {'REPLY', Dragonfly_BaseDir, ['./message_defs.mat']};
    if exist('mm_ip','var') && ~isempty(mm_ip)
        ConnectArgs{end+1} = ['-server_name ' mm_ip];
    end

    ConnectToMMM(ConnectArgs{:});
    Subscribe( MessageTypes{:})
    
    disp 'Reply running..'

    cnt = 1;
    while( 1)
        fprintf('\nWaiting for message\n');
        M = ReadMessage( 'blocking');
        
        switch(M.msg_type)
            case 'REQUEST_TEST_DATA'
                fprintf('Received message %s\n', M.msg_type);
            
                msg = DF.MDF.TEST_DATA;
                msg.a = int32(cnt);
                msg.b = int32(-30);
                msg.x = 123.456;
                SendMessage( 'TEST_DATA', msg);
                
                fprintf('Sent reply\n');
                cnt = cnt + 1;
            case 'EXIT'
                break; 

        end

    end

    DisconnectFromMMM

