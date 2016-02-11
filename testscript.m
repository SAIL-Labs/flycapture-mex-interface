%testscript
clc
%mex -I"C:\Program Files\Point Grey Research\FlyCapture2\include" -L"C:\Program Files\Point Grey Research\FlyCapture2\lib64\C" -lFlyCapture2_C  flycap_interface.c
flycap_interface('init_camera',12380028)
%flycap_interface('init_camera',12380028)


x=20;
y=20;
width=48;
height=48;
roi=[x y width height];

flycap_interface('Setfmt7ImageSettings',roi)

flycap_interface('start_capture')

flycap_interface('enableEmbededInfo',false)
%data=ones(1400,48,48);
tic
for i=1:100
    [rawdata, ts(:,i)]=flycap_interface('GetImage');
    %datenum([1970 1 1 11 0 ts(1)+ts(2)*1e-6])
end
1/(toc/i)

flycap_interface('stop_capture')

clear flycap_interface