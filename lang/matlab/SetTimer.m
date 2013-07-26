function TimerID = SetTimer( SnoozeTime)

global DF;

% Check if Snooze Time is an integer
if( isscalar( SnoozeTime))
    SnoozeTimeMilliSeconds = round( SnoozeTime);
else
    error( 'Snooze Time argument has to be a scalar');
end

TimerID = MatlabDragonfly( DF.mex_opcode.SET_TIMER, SnoozeTimeMilliSeconds);
if( TimerID < 0) error( 'Could not set timer'); end
