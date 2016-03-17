% make.m
% Attempt to build flycap_interface for current platform.

if strcmp(mfilename('fullpath'),fullfile(pwd,mfilename))
    switch computer
        case 'PCWIN64'
            mex -I"C:\Program Files\Point Grey Research\FlyCapture2\include" -L"C:\Program Files\Point Grey Research\FlyCapture2\lib64\C" -lFlyCapture2_C  flycap_interface.c
        case 'PCWIN'
            mex -I"C:\Program Files (x86)\Point Grey Research\FlyCapture2\include" -L"C:\Program Files\Point Grey Research\FlyCapture2\lib\C" -lFlyCapture2_C  flycap_interface.c
        case 'MACI64'
            error('FlyCapture API not avalaible on Mac OS X.')
        case 'GLNXA64'
            error('Compile not yet tested. Code should be similar.')
        otherwise
            error('Unknown System: %s',computer)
    end
else
    error('Mex:FlyCapInterface:WrongFolder','This Makefile is on your path put you are not in the correct folder. Please cd %s',p)
end