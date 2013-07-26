function Producer(mm_ip)

    global DF;

    Dragonfly_BaseDir = '../../..';
    addpath([Dragonfly_BaseDir '/lang/matlab']);

    MessageTypes =  { ...
                        'REQUEST_TEST_DATA' ...
                        'EXIT' ...
                    };

    ConnectArgs = {'PRODUCER', Dragonfly_BaseDir, ['./message_defs.mat']};
    if exist('mm_ip','var') && ~isempty(mm_ip)
        ConnectArgs{end+1} = ['-server_name ' mm_ip];
    end

    ConnectToMMM(ConnectArgs{:});
    Subscribe( MessageTypes{:})
    SendModuleReady();
    
    disp 'Producer running..'
    
    a = 0;
    b = 0;
    x = 0.0;

    while( 1)
            
        msg = DF.MDF.TEST_DATA;
        msg.a = int32(a);
        msg.b = int32(b);
        msg.x = x;
        SendMessage( 'TEST_DATA', msg);
        
        fprintf('Sent reply\n');

        a = a + 1;
        b = b - 3;
        x = x + 1.234;

        pause(1);
    end

    DisconnectFromMMM

