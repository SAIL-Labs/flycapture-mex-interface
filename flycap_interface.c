/*/*==========================================================
 * flycap_interface - Mex interface to comm with fly cap camera.
 *
 * [image]=flycap_interface('')
 * Copyright 2016 Chris Betters
 *
 */

#include <mex.h>
#include <stdint.h>
#include "C/FlyCapture2_C.h"
#include "flycap_interface.h"

fc2Context context;
fc2PGRGuid guid;

static void CloseComms(void) {
    fc2Context cleanContext;
    fc2Error error;
    
    error = fc2StopCapture( context );
    if ( error == FC2_ERROR_OK | error == FC2_ERROR_ISOCH_NOT_STARTED){
        mexPrintf("Capture Stopped \n");
    }
    else {
        mexPrintf( "Error in CloseComms:fc2StopCapture: %d\n", error );
        mexErrMsgTxt("CloseComms:fc2StopCapture Failed");
    }
    
    mexPrintf("Closing connection to flycap2. \n");
    error = fc2DestroyContext( context );
    if ( error != FC2_ERROR_OK )
    {
        
        mexPrintf( "Error in fc2DestroyContext: %d\n", error );
        mexErrMsgTxt("fc2DestroyContext Failed");
    }
    context=cleanContext;
}

void mexFunction( int nlhs, mxArray *plhs[],
        int nrhs, const mxArray *prhs[] )
{
    char *command_name;
    
    if (nrhs < 1)
        mexErrMsgTxt("Not enough input arguments");
    
    command_name = (char *) mxArrayToString(prhs[0]);
    
    if (!strcmp(command_name, "init_camera"))
        command_init_camera(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(command_name, "start_capture"))
        command_start_capture(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(command_name, "stop_capture"))
        command_stop_capture(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(command_name, "enableEmbededInfo"))
        command_enableEmbededInfo(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(command_name, "Setfmt7ImageSettings"))
        command_Setfmt7ImageSettings(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(command_name, "GetImage"))
        command_GetImage(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(command_name, "GetProperty"))
        command_GetProperty(nlhs, plhs, nrhs, prhs);
    else if (!strcmp(command_name, "SetProperty"))
        command_SetProperty(nlhs, plhs, nrhs, prhs);
    else
        mexErrMsgTxt("No such command");
}

void command_init_camera(int nlhs, mxArray * plhs[],int nrhs, const mxArray * prhs []){
    fc2Error error;
    unsigned int numCameras = 0;
    unsigned int serial;
    
    if (nrhs != 2)
        mexErrMsgTxt("Insufficient arguments");
    
    serial=(unsigned int)mxGetScalar(prhs[1]);
    
    if (context==NULL){
        error = fc2CreateContext( &context );
        if ( error != FC2_ERROR_OK )
        {
            mexPrintf( "Error in fc2CreateContext: %d\n", error );
            mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2CreateContext","Error in fc2CreateContext");
        }
        mexAtExit(CloseComms);
        
        error = fc2GetNumOfCameras( context, &numCameras );
        if ( error != FC2_ERROR_OK )
        {
            mexPrintf( "Error in fc2GetNumOfCameras: %d\n", error );
            mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2GetNumOfCameras","Error in fc2GetNumOfCameras");
        }
        
        if ( numCameras == 0 )
        {
            // No cameras detected
            mexErrMsgIdAndTxt("chrislib:flycapInterface:numCamerasIsZero", "No cameras detected.");
        }
        
        // Get the 0th camera
        //error = fc2GetCameraFromIndex( context, 0, &guid );
        error = fc2GetCameraFromSerialNumber( context, serial, &guid);
        if ( error != FC2_ERROR_OK )
        {
            mexPrintf( "Error in fc2GetCameraFromIndex: %d\n", error );
            mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2GetCameraFromIndex","Error in fc2GetCameraFromIndex");
        }
        
        error = fc2Connect( context, &guid );
        if ( error != FC2_ERROR_OK )
        {
            mexPrintf( "Error in fc2Connect: %d\n", error );
            mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2Connect","Error in fc2Connect");
        }
        
        error =fc2WriteRegister(context,0x1050,0x80000001); //BAYER_MONO_CTRL: turn on
        error =fc2WriteRegister(context,0x808,0x80000000); //Sharpness turn off
        error =fc2WriteRegister(context,0x818,0xC0000400); //Gamma: turn off
        
        
    }
    else {
        //mexPrintf( "Already connected to camera: %d\n", serial );
        mexWarnMsgIdAndTxt("chrislib:flycapInterface:AlreadyConnected","Already connected to a camera.");
        PrintCameraInfo();
    }
}

void command_start_capture(int nlhs, mxArray * plhs[],int nrhs, const mxArray * prhs []){
    fc2Error error;
    
    if (nrhs != 1)
        mexErrMsgTxt("Too many arguments");
    
    error = fc2StartCapture( context );
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2StartCapture: %d\n", error );
        mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2StartCapture","Error in fc2StartCapture");
    }
}

void command_stop_capture(int nlhs, mxArray * plhs[],int nrhs, const mxArray * prhs []){
    fc2Error error;
    
    if (nrhs != 1)
        mexErrMsgTxt("Too many arguments");
    
    error = fc2StopCapture( context );
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2StopCapture: %d\n", error );
        mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2StopCapture","Error in fc2StopCapture");
    }
}

void command_enableEmbededInfo(int nlhs, mxArray * plhs[],int nrhs, const mxArray * prhs []){
    
    fc2Error error;
    fc2EmbeddedImageInfo embeddedInfo;
    BOOL enableEmbeddedInfo;
    
    if (nrhs != 2)
        mexErrMsgTxt("Insufficient arguments");
    
    enableEmbeddedInfo=mxIsLogicalScalarTrue(prhs[1]);
    
    error = fc2GetEmbeddedImageInfo( context, &embeddedInfo );
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2GetEmbeddedImageInfo: %d\n", error );
        mexErrMsgTxt("fc2GetEmbeddedImageInfo");
    }
    
    if ( embeddedInfo.timestamp.available != 0 )
    {
        embeddedInfo.timestamp.onOff = enableEmbeddedInfo;
        embeddedInfo.gain.onOff = false;
        embeddedInfo.shutter.onOff = false;
        embeddedInfo.brightness.onOff = false;
        embeddedInfo.exposure.onOff = false;
        embeddedInfo.whiteBalance.onOff = false;
        embeddedInfo.frameCounter.onOff = false;
        embeddedInfo.strobePattern.onOff = false;
        embeddedInfo.GPIOPinState.onOff = false;
        embeddedInfo.ROIPosition.onOff = false;
    }
    
    error = fc2SetEmbeddedImageInfo( context, &embeddedInfo );
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2SetEmbeddedImageInfo: %d\n", error );
        mexErrMsgTxt("fc2SetEmbeddedImageInfo");
    }
}

void command_Setfmt7ImageSettings(int nlhs, mxArray * plhs[],int nrhs, const mxArray * prhs []){
    
    fc2Error error;
    BOOL settingsAreValid;
    fc2Format7PacketInfo packetInfo;
    fc2Format7ImageSettings fmt7ImageSettings;
    double *roiDbl;
    unsigned int roi[4];
    int i;
    BOOL captureWasRunning;
    
    
    roiDbl = mxGetPr(prhs[1]);
    
    for (i=0; i<4; i++ ) {
        roi[i]=(unsigned int)roiDbl[i]; //convert double to uint32
    }
    
    //mexPrintf("x %d y %d w %d h %d",roi[0],roi[1],roi[2],roi[3]);
    if (nrhs == 2){
        fmt7ImageSettings.mode = FC2_MODE_0;
    }
    else if (nrhs == 3) {
        fmt7ImageSettings.mode = (int)mxGetScalar(prhs[2]);
    }
    else {
        mexErrMsgTxt("command_Setfmt7ImageSettings:Wrong number of arguments");
    }

    fmt7ImageSettings.offsetX = roi[0];
    fmt7ImageSettings.offsetY = roi[1];
    fmt7ImageSettings.width = roi[2];
    fmt7ImageSettings.height = roi[3];
    fmt7ImageSettings.pixelFormat = FC2_PIXEL_FORMAT_MONO16; //FC2_PIXEL_FORMAT_MONO16;
    
    error=fc2ValidateFormat7Settings( context,&fmt7ImageSettings,&settingsAreValid, &packetInfo );
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2ValidateFormat7Settings: %d\n", error );
        mexErrMsgTxt("fc2ValidateFormat7Settings");
    }
    
    //first check capture is stopped and stopp if needed
    error = fc2StopCapture( context );
    if ( error == FC2_ERROR_OK ){
        captureWasRunning=true;
    } else if ( error == FC2_ERROR_ISOCH_NOT_STARTED) {
        captureWasRunning=false;
    }
    else {
        mexErrMsgTxt("waht");
    }
    
    error=fc2SetFormat7ConfigurationPacket( context,&fmt7ImageSettings,packetInfo.recommendedBytesPerPacket);
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2SetFormat7ConfigurationPacket: %d\n", error );
        mexErrMsgTxt("fc2SetFormat7ConfigurationPacket");
    }
    
    if (captureWasRunning)
    {
        error = fc2StartCapture( context );
        if ( error != FC2_ERROR_OK )
        {
            mexPrintf( "Error in fc2StartCapture: %d\n", error );
            mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2StartCapture","Error in fc2StartCapture");
        }
    }
}

void command_GetImage(int nlhs, mxArray * plhs[],int nrhs, const mxArray * prhs []){
    
    fc2Error error;
    fc2Image rawImage;
    uint16_t *ImagePointer;
    fc2TimeStamp ts;
    double *timestamp; 
    int NumPixels;
    
    if (nlhs != 2)
        mexErrMsgTxt("Insufficient output arguments");
    
    error = fc2CreateImage( &rawImage );
    if ( error != FC2_ERROR_OK )
    {
        printf( "Error in fc2CreateImage: %d\n", error );
        mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2CreateImage","Error in fc2CreateImage");
    }
    
    // Retrieve the image
    error = fc2RetrieveBuffer( context, &rawImage );
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in retrieveBuffer: %d\n", error);
        mexErrMsgIdAndTxt("chrislib:flycapInterface:retrieveBuffer","Error in retrieveBuffer");
    }
    else
    {
                
        //pass image
        //plhs[0] = mxCreateNumericMatrix(rawImage.cols*rawImage.rows, 1, mxUINT16_CLASS, mxREAL);
        plhs[0] = mxCreateNumericMatrix(rawImage.cols, rawImage.rows, mxUINT16_CLASS, mxREAL);
        ImagePointer = mxGetData(plhs[0]);
        NumPixels=(rawImage.cols*rawImage.rows);
        
        memcpy(ImagePointer,rawImage.pData,rawImage.dataSize);//seemed easiet way to get correct output format.
        
       // Get and pass timestamps
        plhs[1]=mxCreateDoubleMatrix(5, 1, mxREAL);
        timestamp=mxGetPr(plhs[1]);
        
        ts = fc2GetImageTimeStamp( &rawImage);
        timestamp[0]=(double)ts.seconds;
        timestamp[1]=(double)ts.microSeconds;
        timestamp[2]=(double)ts.cycleSeconds;
        timestamp[3]=(double)ts.cycleCount;
        timestamp[4]=(double)ts.cycleOffset;
        
//         plhs[1]=mxCreateNumericMatrix(5, 1, mxINT8_CLASS, mxREAL);
//         timestamp=mxGetPr(plhs[1]);
//         
//         //ts = fc2GetImageTimeStamp( &rawImage);
//         timestamp[0]=rawImage.pData[0];
//         timestamp[1]=rawImage.pData[1];
//         timestamp[2]=rawImage.pData[2];
//         timestamp[3]=rawImage.pData[3];
//         timestamp[4]=(0);
        
    }
    
    error = fc2DestroyImage( &rawImage );
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2DestroyImage: %d\n", error );
        mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2DestroyImage","Error in fc2DestroyImage");
    }
}

void command_GetProperty(int nlhs, mxArray * plhs[],int nrhs, const mxArray * prhs []){
    
    fc2Error error;
	fc2PropertyInfo propInfo;
	fc2Property prop;
    float propValueOut;
    
    if (nrhs != 2)
        mexErrMsgTxt("Insufficient input arguments, need 2");
    if (nlhs != 1)
        mexErrMsgTxt("Insufficient output arguments, need 1");
    
    propInfo.type = (int)mxGetScalar(prhs[1]);
    error = fc2GetPropertyInfo(context, &propInfo);
    if (error != FC2_ERROR_OK)
    {
        propValueOut=0.0f;
    } else {
        if (propInfo.present)
        {
            // Get the frame rate
            prop.type = (int)mxGetScalar(prhs[1]);
            error = fc2GetProperty(context, &prop);
            if (error != FC2_ERROR_OK)
            {
                propValueOut=0.0f;
            }
            
            // Set the frame rate.
            // Note that the actual recording frame rate may be slower,
            // depending on the bus speed and disk writing speed.
            propValueOut=prop.absValue;
        } else {
            propValueOut=0.0f;
        }
    }
    
    plhs[0]=mxCreateDoubleScalar(propValueOut);       
}

void command_SetProperty(int nlhs, mxArray * plhs[],int nrhs, const mxArray * prhs []){
        
    fc2Error error;
	fc2PropertyInfo propInfo;
	fc2Property prop;
    float propValueOut;
    
    if (nrhs != 3)
        mexErrMsgTxt("Insufficient input arguments, need 2");
    
    propInfo.type = (int)mxGetScalar(prhs[1]);
    error = fc2GetPropertyInfo(context, &propInfo);
    if (error != FC2_ERROR_OK)
    {
        mexPrintf( "Error in command_SetProperty:fc2GetPropertyInfo: %d\n", error );
        mexErrMsgIdAndTxt("chrislib:flycapInterface:command_SetProperty:fc2GetPropertyInfo","Error in command_SetProperty:fc2GetPropertyInfo");
    }
    if (propInfo.present)
    {
        // Get the frame rate
        prop.type = (int)mxGetScalar(prhs[1]);
        prop.present;
        prop.absControl=true;
        prop.onePush=false;
        prop.onOff=true;
        prop.autoManualMode=false;
        prop.absValue=(float)mxGetScalar(prhs[2]);
         
        error = fc2SetProperty(context, &prop);
        if (error != FC2_ERROR_OK)
        {
            mexPrintf( "Error in command_SetProperty:fc2SetProperty: %d\n", error );
            mexErrMsgIdAndTxt("chrislib:flycapInterface:command_SetProperty:fc2SetProperty","Error in command_SetProperty:fc2SetProperty");
        }
    }
}


float GetFrameRate()
{
	fc2Error error;
	fc2PropertyInfo propInfo;
	fc2Property prop;

	// Check if the camera supports the FRAME_RATE property
	//printf( "Detecting frame rate from camera... \n" );
	propInfo.type = FC2_FRAME_RATE;
	error = fc2GetPropertyInfo(context, &propInfo);
	if (error != FC2_ERROR_OK)
	{
		return 0.0f;
	}

	if (propInfo.present)
	{
		// Get the frame rate
		prop.type = FC2_FRAME_RATE;
		error = fc2GetProperty(context, &prop);
		if (error != FC2_ERROR_OK)
		{
			return 0.0f;
		}

		// Set the frame rate.
		// Note that the actual recording frame rate may be slower,
		// depending on the bus speed and disk writing speed.
		return prop.absValue;
	}

	return 0.0f;
}

float SetFrameRate(float newFrameRate)
{
	fc2Error error;
	fc2PropertyInfo propInfo;
	fc2Property prop;

    prop.type=FC2_FRAME_RATE;
    prop.absValue=newFrameRate;
    prop.autoManualMode=false;
            
	error = fc2SetProperty(context, &prop);
	if (error != FC2_ERROR_OK)
	{
		mexPrintf( "Error in SetFrameRate:fc2SetProperty %d\n", error );
        mexErrMsgIdAndTxt("chrislib:flycapInterface:SetFrameRate:fc2SetProperty","Error in SetFrameRate:fc2SetProperty");
	}

	return GetFrameRate();
}

void PrintCameraInfo(){
    fc2CameraInfo camInfo;
    fc2Error error;
    
    error = fc2GetCameraInfo( context, &camInfo );
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2GetCameraInfo: %d\n", error );
        mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2GetCameraInfo","Error in fc2GetCameraInfo");
    }
    
    mexPrintf(
            "\n*** CAMERA INFORMATION ***\n"
            "Serial number - %u\n"
            "Camera model - %s\n"
            "Camera vendor - %s\n"
            "Sensor - %s\n"
            "Resolution - %s\n"
            "Firmware version - %s\n"
            "Firmware build time - %s\n\n",
            camInfo.serialNumber,
            camInfo.modelName,
            camInfo.vendorName,
            camInfo.sensorInfo,
            camInfo.sensorResolution,
            camInfo.firmwareVersion,
            camInfo.firmwareBuildTime );
}



