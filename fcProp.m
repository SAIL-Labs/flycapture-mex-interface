%flycappropertytypes

classdef (Sealed) fcProp
    properties (Constant)
        FC2_BRIGHTNESS=0
        FC2_AUTO_EXPOSURE=1,
        FC2_SHARPNESS=2,
        FC2_WHITE_BALANCE=3,
        FC2_HUE=4,
        FC2_SATURATION=5,
        FC2_GAMMA=6,
        FC2_IRIS=7,
        FC2_FOCUS=8,
        FC2_ZOOM=9,
        FC2_PAN=10,
        FC2_TILT=11,
        FC2_SHUTTER=12,
        FC2_GAIN=13,
        FC2_TRIGGER_MODE=14,
        FC2_TRIGGER_DELAY=15,
        FC2_FRAME_RATE=16,
        FC2_TEMPERATURE=17,
    end
    
    methods (Access = private)    % private so that you cant instantiate
        function out = fcProp
        end
    end
end