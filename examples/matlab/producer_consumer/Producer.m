function Producer(mm_ip)

    global DF;

    Dragonfly_BaseDir = '../../..';
    addpath([Dragonfly_BaseDir '/lang/matlab']);

    MessageTypes =  { 'REQUEST_TEST_DATA' };

    ConnectArgs = {'PRODUCER', Dragonfly_BaseDir, ['./message_defs.mat']};
    if exist('mm_ip','var') && ~isempty(mm_ip)
        ConnectArgs{end+1} = ['-server_name ' mm_ip];
    end

    ConnectToMMM(ConnectArgs{:});
    Subscribe( MessageTypes{:})
    
    disp 'Producer running..'
    
    a = 0;

    while( 1)
            
        msg = DF.MDF.TEST_DATA;
        msg.a = int32(a);
        msg.b = int32(-3);
        msg.x = 1.234;
        SendMessage( 'TEST_DATA', msg);
        
        fprintf('Sent message  Data = [a: %d, b: %d, x: %f]\n', msg.a, msg.b, msg.x);

        a = a + 1;

        pause(1);
    end

    DisconnectFromMMM

