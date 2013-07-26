function CancelStateTimeOut

    global Dragonfly_runtime;
        
    if(~isempty(Dragonfly_runtime.StateTimeout_TimerID))
        %disp( ['Canceling StateTimeOut: TimerID = ' num2str(Dragonfly_runtime.StateTimeout_TimerID)]);
        CancelTimer( Dragonfly_runtime.StateTimeout_TimerID );
        Dragonfly_runtime.StateTimeout_TimerID = [];
    end
    