%testscript
clc;clear
%mex -I"C:\Program Files\Point Grey Research\FlyCapture2\include" -L"C:\Program Files\Point Grey Research\FlyCapture2\lib64\C" -lFlyCapture2_C  flycap_interface.c
flycap_interface('init_camera',12380028)


%flycap_interface('Setfmt7ImageSettings',[0 0 640 512],1)
%flycap_interface('Setfmt7ImageSettings',[0 0 1280 1024],0)

flycap_interface('start_capture')

%flycap_interface('SetProperty',fcProp.FC2_GAMMA,1.25)

flycap_interface('enableEmbededInfo',false)
%data=ones(1400,48,48);
tic
for i=1:100
    [rawdata, ts(:,i)]=flycap_interface('GetImage');
    rawdata10=bitshift(rawdata,-6);
    rawdata10alt=rawdata/64;
end
1/(toc/i)

flycap_interface('stop_capture')

%clear flycap_interface
% 
% %%
% for i=1:1
%     imagesc(rawdata(:,:,i))
%     drawnow
% end
% %%
% histogram(double((rawdata(:,:,i)))/16,1:2^12)
% %xlim([500 1800])
%  

% flycap_interface('init_camera',12380028)
% flycap_interface('start_capture')
% [rawdata, ts]=flycap_interface('GetImage');
% flycap_interface('stop_capture')
% clear flycap_interface
% %%
% rawdata10=bitshift(rawdata,-6);
% rawdata10=rawdata/64;
% h=histogram(bitshift(rawdata,-6),0:2^10);
% %imagesc(rawdata)
% plot(h.Values)
