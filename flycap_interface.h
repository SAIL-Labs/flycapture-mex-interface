

void command_init_camera(int , mxArray * [], int, const mxArray * []);
void command_start_capture(int , mxArray * [], int, const mxArray * []);
void command_stop_capture(int , mxArray * [], int, const mxArray * []);
void command_enableEmbededInfo(int , mxArray * [], int, const mxArray * []);
void command_Setfmt7ImageSettings(int , mxArray * [], int, const mxArray * []);
void command_GetImage(int , mxArray * [], int, const mxArray * []);
void command_GetProperty(int , mxArray * [], int, const mxArray * []);
void command_SetProperty(int , mxArray * [], int, const mxArray * []);


void PrintCameraInfo();
float GetFrameRate();
float SetFrameRate(float newFrameRate);