function CancelTimer( TimerID)

global DF;

% Check if TimerID is an integer
if( isscalar( TimerID))
    status = MatlabDragonfly( DF.mex_opcode.CANCEL_TIMER, TimerID);
    %disp(['Cancelled Timer ', num2str(TimerID)]);
%     if( status ~= 1)
%         disp(['WARNING... CancelTimer failed with status ', num2str(status)]);
%     end
else
    error( 'TimerID argument has to be an integer');
end
