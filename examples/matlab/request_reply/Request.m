function Request(mm_ip)

    global DF;

    Dragonfly_BaseDir = '../../..';
    addpath([Dragonfly_BaseDir '/lang/matlab']);

    MessageTypes =  { ...
                        'TEST_DATA' ...
                        'EXIT' ...
                    };

    ConnectArgs = {'REQUEST', Dragonfly_BaseDir, ['./message_defs.mat']};
    if exist('mm_ip','var') && ~isempty(mm_ip)
        ConnectArgs{end+1} = ['-server_name ' mm_ip];
    end

    ConnectToMMM(ConnectArgs{:});
    Subscribe( MessageTypes{:})

    disp 'Request running..'


    while( 1)
        SendSignal REQUEST_TEST_DATA;
        fprintf('\nWaiting for reply\n');

        M = ReadMessage( 'blocking');
        switch(M.msg_type)
            case 'TEST_DATA'
                fprintf('Received  a=%d  b=%d  x=%f\n', M.data.a, M.data.b, M.data.x);
            
            case 'EXIT'
                break; 
        end
        
        pause(1);
    end

    DisconnectFromMMM

