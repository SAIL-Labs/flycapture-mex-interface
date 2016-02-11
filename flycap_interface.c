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

void command_init_camera(int , mxArray * [], int, const mxArray * []);
void command_start_capture(int , mxArray * [], int, const mxArray * []);
void command_stop_capture(int , mxArray * [], int, const mxArray * []);
void command_enableEmbededInfo(int , mxArray * [], int, const mxArray * []);
void command_Setfmt7ImageSettings(int , mxArray * [], int, const mxArray * []);
void command_GetImage(int , mxArray * [], int, const mxArray * []);
void PrintCameraInfo();

fc2Context context;
fc2PGRGuid guid;

static void CloseComms(void)
{
    fc2Error error;
    mexPrintf("Closing connection to flycap2. \n");
    error = fc2DestroyContext( context );
    if ( error != FC2_ERROR_OK )
    {
        
        mexPrintf( "Error in fc2DestroyContext: %d\n", error );
        mexErrMsgTxt("fc2DestroyContext Failed");
    }
    context=NULL;
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
        embeddedInfo.timestamp.onOff = enableEmbeddedInfo;
        embeddedInfo.gain.onOff = enableEmbeddedInfo;
        embeddedInfo.shutter.onOff = enableEmbeddedInfo;
        embeddedInfo.brightness.onOff = enableEmbeddedInfo;
        embeddedInfo.exposure.onOff = enableEmbeddedInfo;
        embeddedInfo.whiteBalance.onOff = false;
        embeddedInfo.frameCounter.onOff = enableEmbeddedInfo;
        embeddedInfo.strobePattern.onOff = false;
        embeddedInfo.GPIOPinState.onOff = false;
        embeddedInfo.ROIPosition.onOff = enableEmbeddedInfo;
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
    
    roiDbl = mxGetPr(prhs[1]);
    
    for (i=0; i<4; i++ ) {
        roi[i]=(unsigned int)roiDbl[i]; //convert double to uint32
    }
    
    //mexPrintf("x %d y %d w %d h %d",roi[0],roi[1],roi[2],roi[3]);
    if (nrhs != 2)
        mexErrMsgTxt("Insufficient arguments");
    
    fmt7ImageSettings.mode = FC2_MODE_0;
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
    
    error=fc2SetFormat7ConfigurationPacket( context,&fmt7ImageSettings,packetInfo.recommendedBytesPerPacket);
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2SetFormat7ConfigurationPacket: %d\n", error );
        mexErrMsgTxt("fc2SetFormat7ConfigurationPacket");
    }
}

void command_GetImage(int nlhs, mxArray * plhs[],int nrhs, const mxArray * prhs []){
    
    fc2Error error;
    fc2Image rawImage;
    uint16_t *ImagePointer;
    uint16_t *rawdata;
    fc2TimeStamp ts;
    double *timestamp; 
    int i;
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
        plhs[0] = mxCreateNumericMatrix(rawImage.rows, rawImage.cols, mxUINT16_CLASS, mxREAL);
        ImagePointer = mxGetData(plhs[0]);
        NumPixels=(rawImage.cols*rawImage.rows);
        
        memcpy(ImagePointer,rawImage.pData,rawImage.dataSize);//seemed easiet way to get correct output format.
        
        // Get and pass timestamp
        plhs[1]=mxCreateDoubleMatrix(5, 1, mxREAL);
        timestamp=mxGetPr(plhs[1]);
        
        ts = fc2GetImageTimeStamp( &rawImage);
        timestamp[0]=(double)ts.seconds;
        timestamp[1]=(double)ts.microSeconds;
        timestamp[2]=(double)ts.cycleSeconds;
        timestamp[3]=(double)ts.cycleCount;
        timestamp[4]=(double)ts.cycleOffset;
    }
    
    error = fc2DestroyImage( &rawImage );
    if ( error != FC2_ERROR_OK )
    {
        mexPrintf( "Error in fc2DestroyImage: %d\n", error );
        mexErrMsgIdAndTxt("chrislib:flycapInterface:fc2DestroyImage","Error in fc2DestroyImage");
    }
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



