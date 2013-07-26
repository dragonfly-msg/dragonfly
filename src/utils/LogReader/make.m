function make()
% make
%
% Builds mex file for reading binary message logs.
base_dir = '../../..'
if( isunix)
    options = '-D_MEX_FILE_ -DUSE_LINUX';
else
    options = '-D_MEX_FILE_';
end
options = [options ' -I' base_dir '/src/core' ' -I' base_dir '/include' ' -I' base_dir '/include/internal'];
options = [options ' -I../../modules/QuickLogger' ' -I' base_dir '/lang/matlab'];

additional_source_files = [base_dir '/src/core/MyCException.cpp ' base_dir '/src/core/MyCString.cpp'];

cmd = ['mex ' options ' LogReader.cpp ' additional_source_files]
eval( cmd);
