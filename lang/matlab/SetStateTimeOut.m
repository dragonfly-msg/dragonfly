function SetStateTimeOut( TimeOut)
% SetStateTimeOut( TimeOut)
%
% This function is for use in conjunction with the "eventmap" function of
% the Dragonfly state machine. It sets a timeout of TimeOut milliseconds for 
% the current state. If none of the events specified in the
% eventmap are received before the timeout, then a TIMED_OUT message is
% delivered. Make sure to include a mapping for the TIMED_OUT message in
% the eventmap.
%
% Note that this function differs from SetTimer in the sense that SetTimer
% is general, whereas SetStateTimeOut is actually built on top of SetTimer,
% but provides a more convenient interface for use with state machines. If
% you use SetTimer directly in a state machine, you have to mess with the
% timer ID-s to make sure you receive the right TIMER_EXPIRED message. With
% SetStateTimeOut you do not have to worry about timer ID-s because they
% are kept track of under the hood and the timeout timer is automatically
% cancelled upon entry to the next state, so you will never end up
% receiving a TIMED_OUT message the resulted from a timeout set in one of
% the previous states (this would be a common pitfall when using SetTimer
% directly).

% Meel Velliste 9/30/2008


    global Dragonfly_runtime;

    if( ~isempty( Dragonfly_runtime.StateTimeout_TimerID)), error( 'SetStateTimeOut(): A state timeout timer has already been set'); end
        
    TimerID = SetTimer( TimeOut);
    Dragonfly_runtime.StateTimeout_TimerID = TimerID;
    %disp( [GetCurrentState() ': Setting State TimeOut: TimerID = ' num2str(TimerID)]);
