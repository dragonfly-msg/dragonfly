%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%
% make.m - Builds the matlab wrapper for the Dragonfly interface. Note that this script
%          re-compiles everything from source rather than using the pre-compiled
%          Dragonfly lib or dll file. This is necessary because it malloc and new 
%          calls need to be wrapped to use the Matlab memory manager (see mex_malloc.c and mex_malloc.cpp in src/core)
%
% MV 4/25/2011

if( ~ispc && ~isunix)
    error( 'Unsupported platform');
end

base_dir = '../..';
core_dir = [base_dir '/src/core'];
pipelib_dir = [core_dir '/PipeLib'];

core_sources = [core_dir '/Dragonfly.cpp ' core_dir '/MyCException.cpp ' core_dir '/MyCString.cpp ' core_dir '/mex_hack.cpp '];
pipelib_sources = [pipelib_dir '/UPipe.cpp ' pipelib_dir '/SocketPipe.cpp ' pipelib_dir '/Timing.cpp '];

sources = ['MatlabDragonfly.cpp ' core_sources pipelib_sources];
options = ['-v -D_MEX_FILE_ '];
include_dirs = ['-I' base_dir '/include ' '-I' base_dir '/include/internal '];
libs = [''];

if( ispc)
    libs = [libs 'ws2_32.lib '];
elseif( isunix)
    options = [options '-DUSE_LINUX '];
end

cmd = ['mex ' options include_dirs sources libs]
eval( cmd);
