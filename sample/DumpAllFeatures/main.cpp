#include "../common/common.hpp"

static char     buffer[1024*1024];
static int32_t  n;

void dumpFeature(TY_DEV_HANDLE handle, TY_COMPONENT_ID compID, TY_FEATURE_ID featID, const char* name)
{
    TY_FEATURE_INFO featInfo;
    ASSERT_OK(TYGetFeatureInfo(handle, compID, featID, &featInfo));

    if(featInfo.isValid && (featInfo.accessMode&TY_ACCESS_READABLE)){
        LOGD("===         %s: comp(0x%x) feat(0x%x) name(%s) access(%d) bindComponent(0x%x) bindFeature(0x%x)"
                , name, featInfo.componentID, featInfo.featureID, featInfo.name
                , featInfo.accessMode, featInfo.bindComponentID, featInfo.bindFeatureID);

        if(TYFeatureType(featID) == TY_FEATURE_INT){
            ASSERT_OK(TYGetInt(handle, compID, featID, &n));
            LOGD("===         %14s: %d", "", n);
        }
        if(TYFeatureType(featID) == TY_FEATURE_FLOAT){
            float v;
            ASSERT_OK(TYGetFloat(handle, compID, featID, &v));
            LOGD("===         %14s: %f", "", v);
        }
        if(TYFeatureType(featID) == TY_FEATURE_ENUM){
            ASSERT_OK(TYGetEnumEntryCount(handle, compID, featID, &n));
            LOGD("===         %14s: entry count %d", "", n);
            if(n > 0){
                TY_ENUM_ENTRY* pEntry = new TY_ENUM_ENTRY[n];
                ASSERT_OK(TYGetEnumEntryInfo(handle, compID, featID, pEntry, n, &n));
                for(int i = 0; i < n; i++){
                    LOGD("===         %14s:     value(%d), desc(%s)", "", pEntry[i].value, pEntry[i].description);
                }
                delete [] pEntry;
            }
        }
        if(TYFeatureType(featID) == TY_FEATURE_BOOL){
            bool v;
            ASSERT_OK(TYGetBool(handle, compID, featID, &v));
            LOGD("===         %14s: %d", "", v);
        }
        if(TYFeatureType(featID) == TY_FEATURE_STRING){
            ASSERT_OK(TYGetStringBufferSize(handle, compID, featID, &n));
            LOGD("===         %14s: length(%d)", "", n);
            ASSERT_OK(TYGetString(handle, compID, featID, buffer, sizeof(buffer)));
            LOGD("===         %14s: content(%s)", "", buffer);
        }
#if 0
        if(TYFeatureType(featID) == TY_FEATURE_BYTEARRAY){
            ASSERT_OK(TYGetByteArraySize(handle, compID, featID, &n));
            LOGD("===         %14s: size(%d)", "", n);
            n = sizeof(buffer);
            ASSERT_OK(TYGetByteArray(handle, compID, featID, (uint8_t*)buffer, n, &n));
            LOGD("===         %14s: size %d", "", n);
        }
#endif
        if(TYFeatureType(featID) == TY_FEATURE_STRUCT){
            switch(featID){
                case TY_STRUCT_CAM_INTRINSIC:{
                    TY_CAMERA_INTRINSIC* p = (TY_CAMERA_INTRINSIC*)buffer;
                    ASSERT_OK(TYGetStruct(handle, compID, featID, (void*)p
                                , sizeof(TY_CAMERA_INTRINSIC)));
                    LOGD("===%23s%f %f %f", "", p->data[0], p->data[1], p->data[2]);
                    LOGD("===%23s%f %f %f", "", p->data[3], p->data[4], p->data[5]);
                    LOGD("===%23s%f %f %f", "", p->data[6], p->data[7], p->data[8]);
                    return;
                }
                case TY_STRUCT_EXTRINSIC_TO_LEFT_IR:
                case TY_STRUCT_EXTRINSIC_TO_LEFT_RGB: {
                    TY_CAMERA_EXTRINSIC* p = (TY_CAMERA_EXTRINSIC*)buffer;
                    ASSERT_OK(TYGetStruct(handle, compID, featID, (void*)p
                                , sizeof(TY_CAMERA_EXTRINSIC)));
                    LOGD("===%23s%f %f %f %f", "", p->data[0], p->data[1], p->data[2], p->data[3]);
                    LOGD("===%23s%f %f %f %f", "", p->data[4], p->data[5], p->data[6], p->data[7]);
                    LOGD("===%23s%f %f %f %f", "", p->data[8], p->data[9], p->data[10], p->data[11]);
                    LOGD("===%23s%f %f %f %f", "", p->data[12], p->data[13], p->data[14], p->data[15]);
                    return;
                }
                default:
                    LOGD("===         %s: Unknown struct", name);
                    return;
            }
        }
    }
}

void dumpFeatures(TY_DEV_HANDLE handle, TY_COMPONENT_ID compID)
{
    dumpFeature(handle, compID, TY_STRUCT_CAM_INTRINSIC         , "TY_STRUCT_CAM_INTRINSIC          ");
    dumpFeature(handle, compID, TY_STRUCT_EXTRINSIC_TO_LEFT_IR  , "TY_STRUCT_EXTRINSIC_TO_LEFT_IR   ");
    dumpFeature(handle, compID, TY_STRUCT_EXTRINSIC_TO_LEFT_RGB , "TY_STRUCT_EXTRINSIC_TO_LEFT_RGB  ");

    dumpFeature(handle, compID, TY_INT_WIDTH_MAX                , "TY_INT_WIDTH_MAX                 ");
    dumpFeature(handle, compID, TY_INT_HEIGHT_MAX               , "TY_INT_HEIGHT_MAX                ");
    dumpFeature(handle, compID, TY_INT_OFFSET_X                 , "TY_INT_OFFSET_X                  ");
    dumpFeature(handle, compID, TY_INT_OFFSET_Y                 , "TY_INT_OFFSET_Y                  ");
    dumpFeature(handle, compID, TY_INT_WIDTH                    , "TY_INT_WIDTH                     ");
    dumpFeature(handle, compID, TY_INT_HEIGHT                   , "TY_INT_HEIGHT                    ");
    dumpFeature(handle, compID, TY_INT_IMAGE_SIZE               , "TY_INT_IMAGE_SIZE                ");
    dumpFeature(handle, compID, TY_ENUM_PIXEL_FORMAT            , "TY_ENUM_PIXEL_FORMAT             ");
    dumpFeature(handle, compID, TY_ENUM_IMAGE_MODE              , "TY_ENUM_IMAGE_MODE               ");
                                                                                                    
    dumpFeature(handle, compID, TY_BOOL_TRIGGER_MODE            , "TY_BOOL_TRIGGER_MODE             ");
    dumpFeature(handle, compID, TY_ENUM_TRIGGER_ACTIVATION      , "TY_ENUM_TRIGGER_ACTIVATION       ");
    dumpFeature(handle, compID, TY_INT_FRAME_PER_TRIGGER        , "TY_INT_FRAME_PER_TRIGGER         ");
                                                                                                    
    dumpFeature(handle, compID, TY_BOOL_AUTO_EXPOSURE           , "TY_BOOL_AUTO_EXPOSURE            ");
    dumpFeature(handle, compID, TY_INT_EXPOSURE_TIME            , "TY_INT_EXPOSURE_TIME             ");
    dumpFeature(handle, compID, TY_BOOL_AUTO_GAIN               , "TY_BOOL_AUTO_GAIN                ");
    dumpFeature(handle, compID, TY_INT_GAIN                     , "TY_INT_GAIN                      ");
                                                                                                    
    dumpFeature(handle, compID, TY_BOOL_UNDISTORTION            , "TY_BOOL_UNDISTORTION             ");

    dumpFeature(handle, compID, TY_INT_LASER_POWER              , "TY_INT_LASER_POWER               ");

    dumpFeature(handle, compID, TY_INT_R_GAIN                   , "TY_INT_R_GAIN                    ");
    dumpFeature(handle, compID, TY_INT_G_GAIN                   , "TY_INT_G_GAIN                    ");
    dumpFeature(handle, compID, TY_INT_B_GAIN                   , "TY_INT_B_GAIN                    ");
}

void dumpAllComponentFeatures(TY_DEV_HANDLE handle, int32_t compIDs)
{
    bool hasFeature;

    LOGD("=== Dump all components and features:");
    if(compIDs & TY_COMPONENT_DEVICE){
        LOGD("===  TY_COMPONENT_DEVICE:");
        dumpFeatures(handle, TY_COMPONENT_DEVICE);
    }
    if(compIDs & TY_COMPONENT_DEPTH_CAM){
        LOGD("===  TY_COMPONENT_DEPTH_CAM:");
        dumpFeatures(handle, TY_COMPONENT_DEPTH_CAM);
    }
    if(compIDs & TY_COMPONENT_POINT3D_CAM){
        LOGD("===  TY_COMPONENT_POINT3D_CAM:");
        dumpFeatures(handle, TY_COMPONENT_POINT3D_CAM);
    }
    if(compIDs & TY_COMPONENT_IR_CAM_LEFT){
        LOGD("===  TY_COMPONENT_IR_CAM_LEFT:");
        dumpFeatures(handle, TY_COMPONENT_IR_CAM_LEFT);
    }
    if(compIDs & TY_COMPONENT_IR_CAM_RIGHT){
        LOGD("===  TY_COMPONENT_IR_CAM_RIGHT:");
        dumpFeatures(handle, TY_COMPONENT_IR_CAM_RIGHT);
    }
    if(compIDs & TY_COMPONENT_RGB_CAM_LEFT){
        LOGD("===  TY_COMPONENT_RGB_CAM_LEFT:");
        dumpFeatures(handle, TY_COMPONENT_RGB_CAM_LEFT);
    }
    if(compIDs & TY_COMPONENT_RGB_CAM_RIGHT){
        LOGD("===  TY_COMPONENT_RGB_CAM_RIGHT:");
        dumpFeatures(handle, TY_COMPONENT_RGB_CAM_RIGHT);
    }
    if(compIDs & TY_COMPONENT_LASER){
        LOGD("===  TY_COMPONENT_LASER:");
        dumpFeatures(handle, TY_COMPONENT_LASER);
    }
    if(compIDs & TY_COMPONENT_IMU){
        LOGD("===  TY_COMPONENT_IMU:");
        dumpFeatures(handle, TY_COMPONENT_IMU);
    }
}

int main(int argc, char* argv[])
{
    const char* IP = NULL;
    TY_DEV_HANDLE handle;

    for(int i = 1; i < argc; i++){
        if(strcmp(argv[i], "-ip") == 0){
            IP = argv[++i];
        }else if(strcmp(argv[i], "-h") == 0){
            LOGI("Usage: SimpleView_Callback [-h] [-ip <IP>]");
            return 0;
        }
    }
    
    // Init lib
    ASSERT_OK(TYInitLib());
    TY_VERSION_INFO* pVer = (TY_VERSION_INFO*)buffer;
    ASSERT_OK( TYLibVersion(pVer) );
    LOGD("=== lib version: %d.%d.%d", pVer->major, pVer->minor, pVer->patch);

    if(IP) {
        LOGD("=== Open device %s", IP);
        ASSERT_OK( TYOpenDeviceWithIP(IP, &handle) );
    } else {
        // Get device info
        ASSERT_OK(TYGetDeviceNumber(&n));
        LOGD("=== device number %d", n);

        TY_DEVICE_BASE_INFO* pBaseInfo = (TY_DEVICE_BASE_INFO*)buffer;
        ASSERT_OK(TYGetDeviceList(pBaseInfo, 100, &n));

        if(n == 0){
            LOGD("=== No device got");
            return -1;
        }

        LOGD("=== get device list %d:", n);
        for(int i = 0; i < n; i++){
            LOGD("===   device %d:", i);
            LOGD("===       interface  : %d", pBaseInfo[i].devInterface);
            LOGD("===       id         : %s", pBaseInfo[i].id);
            LOGD("===       vendor     : %s", pBaseInfo[i].vendorName);
            LOGD("===       model      : %s", pBaseInfo[i].modelName);
            LOGD("===       HW version : %d.%d.%d"
                    , pBaseInfo[i].hardwareVersion.major
                    , pBaseInfo[i].hardwareVersion.minor
                    , pBaseInfo[i].hardwareVersion.patch
                    );
            LOGD("===       FW version : %d.%d.%d"
                    , pBaseInfo[i].firmwareVersion.major
                    , pBaseInfo[i].firmwareVersion.minor
                    , pBaseInfo[i].firmwareVersion.patch
                    );
        }

        // Open device 0
        ASSERT_OK(TYOpenDevice(pBaseInfo[0].id, &handle));
    }

    // List all components
    int32_t compIDs;
    std::string compNames;
    ASSERT_OK(TYGetComponentIDs(handle, &compIDs));
    dumpAllComponentFeatures(handle, compIDs);

    printf("Done!\n");
    TYDeinitLib();
    return 0;
}
