#include "../common/common.hpp"
#include <stdio.h>

static char buffer[1024*1024];
static int  n;
static volatile bool exit_main;


struct CallbackData {
    int             index;
    TY_DEV_HANDLE   hDevice;
    DepthRender*    render;
};

void frameCallback(TY_FRAME_DATA* frame, void* userdata)
{
    CallbackData* pData = (CallbackData*) userdata;
    LOGD("=== Get frame %d", ++pData->index);

    for( int i = 0; i < frame->validCount; i++ ){
        // get & show depth image
        if(frame->image[i].componentID == TY_COMPONENT_DEPTH_CAM){
            cv::Mat depth(frame->image[i].height, frame->image[i].width
                    , CV_16U, frame->image[i].buffer);
            cv::Mat colorDepth = pData->render->Compute(depth);
            cv::imshow("ColorDepth", colorDepth);
        }
        // get & show left ir image
        if(frame->image[i].componentID == TY_COMPONENT_IR_CAM_LEFT){
            cv::Mat leftIR(frame->image[i].height, frame->image[i].width
                    , CV_8U, frame->image[i].buffer);
            cv::imshow("LeftIR", leftIR);
        }
        // get & show right ir image
        if(frame->image[i].componentID == TY_COMPONENT_IR_CAM_RIGHT){
            cv::Mat rightIR(frame->image[i].height, frame->image[i].width
                    , CV_8U, frame->image[i].buffer);
            cv::imshow("RightIR", rightIR);
        }
        // get point3D
        if(frame->image[i].componentID == TY_COMPONENT_POINT3D_CAM){
            cv::Mat point3D(frame->image[i].height, frame->image[i].width
                    , CV_32FC3, frame->image[i].buffer);
        }
        // get & show RGB
        if(frame->image[i].componentID == TY_COMPONENT_RGB_CAM){
            cv::Mat rgb(frame->image[i].height, frame->image[i].width
                    , CV_8UC3, frame->image[i].buffer);
            cv::Mat bgr;
            cv::cvtColor(rgb, bgr, cv::COLOR_RGB2BGR);
            cv::imshow("bgr", bgr);
        }
    }

    int key = cv::waitKey(1);
    switch(key){
        case -1:
            break;
        case 'q': case 1048576 + 'q':
            exit_main = true;
            break;
        default:
            LOGD("Pressed key %d", key);
    }

    LOGD("=== Callback: Re-enqueue buffer(%p, %d)", frame->userBuffer, frame->bufferSize);
    ASSERT_OK( TYEnqueueBuffer(pData->hDevice, frame->userBuffer, frame->bufferSize) );
}

int main(int argc, char* argv[])
{
    const char* IP = NULL;
    TY_DEV_HANDLE hDevice;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-ip") == 0){
            IP = argv[++i];
        }else if(strcmp(argv[i], "-h") == 0){
            LOGI("Usage: SimpleView_Callback [-h] [-ip <IP>]");
            return 0;
        }
    }
    
    int nDevices;
    bool needKeyStrike = false;

    LOGD("=== Init lib");
    ASSERT_OK( TYInitLib() );
    TY_VERSION_INFO* pVer = (TY_VERSION_INFO*)buffer;
    ASSERT_OK( TYLibVersion(pVer) );
    LOGD("     - lib version: %d.%d.%d", pVer->major, pVer->minor, pVer->patch);

    if(IP) {
        LOGD("=== Open device %s", IP);
        ASSERT_OK( TYOpenDeviceWithIP(IP, &hDevice) );
    } else {
        LOGD("=== Get device info");
        ASSERT_OK( TYGetDeviceNumber(&n) );
        LOGD("     - device number %d", n);

        TY_DEVICE_BASE_INFO* pBaseInfo = (TY_DEVICE_BASE_INFO*)buffer;
        ASSERT_OK( TYGetDeviceList(pBaseInfo, 100, &n) );

        if(n == 0){
            LOGD("=== No device got");
            return -1;
        }

        LOGD("=== Open device 0");
        ASSERT_OK( TYOpenDevice(pBaseInfo[0].id, &hDevice) );
    }

    int32_t allComps;
    ASSERT_OK( TYGetComponentIDs(hDevice, &allComps) );
    if(allComps & TY_COMPONENT_RGB_CAM){
        // LOGD("=== Has RGB camera, open RGB cam");
        ASSERT_OK( TYEnableComponents(hDevice, TY_COMPONENT_RGB_CAM) );
    }

    LOGD("=== Configure components, open depth cam");
    int32_t componentIDs = TY_COMPONENT_DEPTH_CAM;
    ASSERT_OK( TYEnableComponents(hDevice, componentIDs) );

    LOGD("=== Configure feature, set resolution to 640x480.");
    LOGD("Note: DM460 resolution feature is in component TY_COMPONENT_DEVICE,");
    LOGD("      other device may lays in some other components.");
    int err = TYSetEnum(hDevice, TY_COMPONENT_DEPTH_CAM, TY_ENUM_IMAGE_MODE, TY_IMAGE_MODE_640x480);
    ASSERT(err == TY_STATUS_OK || err == TY_STATUS_NOT_PERMITTED);

    LOGD("=== Prepare image buffer");
    int32_t frameSize;
    ASSERT_OK( TYGetFrameBufferSize(hDevice, &frameSize) );
    LOGD("     - Get size of framebuffer, %d", frameSize);
    ASSERT( frameSize >= 640*480*2 );

    LOGD("     - Allocate & enqueue buffers");
    char* frameBuffer[2];
    frameBuffer[0] = new char[frameSize];
    frameBuffer[1] = new char[frameSize];
    LOGD("     - Enqueue buffer (%p, %d)", frameBuffer[0], frameSize);
    ASSERT_OK( TYEnqueueBuffer(hDevice, frameBuffer[0], frameSize) );
    LOGD("     - Enqueue buffer (%p, %d)", frameBuffer[1], frameSize);
    ASSERT_OK( TYEnqueueBuffer(hDevice, frameBuffer[1], frameSize) );

    LOGD("=== Register callback");
    LOGD("Note: Callback may block internal data receiving,");
    LOGD("      so that user should not do long time work in callback.");
    LOGD("      To avoid copying data, we pop the framebuffer from buffer queue and");
    LOGD("      give it back to user, user should call TYEnqueueBuffer to re-enqueue it.");
    DepthRender render;
    CallbackData cb_data;
    cb_data.index = 0;
    cb_data.hDevice = hDevice;
    cb_data.render = &render;
    ASSERT_OK( TYRegisterCallback(hDevice, frameCallback, &cb_data) );

    LOGD("=== Enable trigger mode");
    ASSERT_OK( TYSetBool(hDevice, TY_COMPONENT_DEVICE, TY_BOOL_TRIGGER_MODE, true) );

    LOGD("=== Start capture");
    ASSERT_OK( TYStartCapture(hDevice) );

    LOGD("=== Loop for send trigger signal");
    exit_main = false;
    while(!exit_main){
        if(needKeyStrike){
            LOGD("--- press any key to trigger:");
            getchar();
        }
        ASSERT_OK( TYSendSoftTrigger(hDevice) );
        MSLEEP(50);
    }

    ASSERT_OK( TYStopCapture(hDevice) );
    ASSERT_OK( TYCloseDevice(hDevice) );
    ASSERT_OK( TYDeinitLib() );
    delete frameBuffer[0];
    delete frameBuffer[1];

    LOGD("=== Main done!");
    return 0;
}
