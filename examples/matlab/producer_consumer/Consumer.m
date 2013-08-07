function Consumer(mm_ip)

    global DF;

    Dragonfly_BaseDir = '../../..';
    addpath([Dragonfly_BaseDir '/lang/matlab']);

    MessageTypes =  { 'TEST_DATA' };

    ConnectArgs = {'CONSUMER', Dragonfly_BaseDir, ['./message_defs.mat']};
    if exist('mm_ip','var') && ~isempty(mm_ip)
        ConnectArgs{end+1} = ['-server_name ' mm_ip];
    end

    ConnectToMMM(ConnectArgs{:});
    Subscribe( MessageTypes{:})

    disp 'Consumer running..'

    while( 1)
        M = ReadMessage( 'blocking');
        switch(M.msg_type)
            case 'TEST_DATA'
                fprintf('Received message %s  Data = [a:%d  b:%d  x:%f]\n', M.msg_type, M.data.a, M.data.b, M.data.x);
        end
    end

    DisconnectFromMMM

