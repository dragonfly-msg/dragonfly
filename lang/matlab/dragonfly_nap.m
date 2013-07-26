function dragonfly_nap( milliseconds)

% dragonfly_nap( milliseconds)
%
% Sleeps for the specified number of milliseconds (with about 10 msec accuracy).
% It does not waste any processor time while sleeping, but is still
% responsive to Dragonfly messages through the EventHook function and will still
% respond to special events like EXIT.
%
% Meel Velliste, Aug 6, 2006

%tic
TimerID = SetTimer( milliseconds);
while( 1)
    Message = WaitFor( 'TIMER_EXPIRED');
    if( Message.data.timer_id == TimerID) break; end
end
%actual_time = toc;
%fprintf( 'dragonfly_nap: %.0f %.0f\n', milliseconds, actual_time*1000);
