function cameratime=interpFC2EmbedTimestamp(rawdata)
    timestamp=swapbytes(typecast(rawdata, 'uint32'));
    
    sec=binvec2dec(fliplr(double(bitget(timestamp,32:-1:26))));
    partsec=binvec2dec(fliplr(double(bitget(timestamp,25:-1:13))));
    cycleoff=binvec2dec(fliplr(double(bitget(timestamp,12:-1:1))));
    
    cameratime=sec+partsec/7999+cycleoff/3071*0.125e-6;