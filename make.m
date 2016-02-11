% make.m
% Attempt to build flycap_interface for current platform.

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