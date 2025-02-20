% build_mex.m
% Script to compile MEX functions for the pulseg.autoseg package

% Define the path to the private folder where the compiled MEX files should go.
privateFolder = fullfile(fileparts(mfilename('fullpath')), 'matlab', '+pulseg', '+autoseg', 'private');

% Compile the MEX files without changing the working directory.
mex('-outdir', privateFolder, fullfile(privateFolder, 'mex_find_periodic_pattern.c'));
mex('-outdir', privateFolder, fullfile(privateFolder, 'mex_count_repetitions.c'));

disp('MEX compilation complete.');
