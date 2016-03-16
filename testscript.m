%testscript
clc;clear

flycap_interface('init_camera',12380028) % if you need more then camera, you'll need to recomplie with new name at this time.

%flycap_interface('Setfmt7ImageSettings',[0 0 640 512],1) % change 2x2 binning mode
%flycap_interface('Setfmt7ImageSettings',[0 0 1280 1024]) % only changes roi, need to ensure you pick valid roi. 
flycap_interface('Setfmt7ImageSettings',[0 0 1280 1024],0) % works on 13e4m, full frame, mode 0.

flycap_interface('start_capture')

flycap_interface('SetProperty',fcProp.FC2_SHUTTER,1)
flycap_interface('SetProperty',fcProp.FC2_FRAME_RATE,10)

fr=flycap_interface('GetProperty',fcProp.FC2_FRAME_RATE)

flycap_interface('enableEmbededInfo',false)

[rawdata, computerTimeStamp]=flycap_interface('GetImage');

flycap_interface('stop_capture') % if you start flycap be sure to stop capture first and vice versa

clear flycap_interface