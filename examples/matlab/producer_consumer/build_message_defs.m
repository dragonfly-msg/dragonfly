dragonfly_path = getenv('DRAGONFLY');
addpath( [dragonfly_path '/tools/']);
build_matlab_message_defs([ dragonfly_path '/examples/matlab/producer_consumer/message_defs.h']);