#ifndef HU_BUFFERS_H
#define HU_BUFFERS_H

#define sd_buf_aud_len 2 + 4 + 6 + 8 + 2 + 4 + 6 + 7 + 2 + 4 + 6 + 7 // 58
// Channels ( or Service IDs)
#define AA_CH_CTR                                                              \
  0 // Sync with hu_tra.java, hu_aap.h and hu_aap.c:aa_type_array[]
#define AA_CH_SEN 1
#define AA_CH_VID 2
#define AA_CH_TOU 3
#define AA_CH_AUD 4
#define AA_CH_AU1 5
#define AA_CH_AU2 6
#define AA_CH_MIC 7
#define AA_CH_MAX 7

byte sd_buf[] = {
    0,    6, // 8, 0};
    // Svc Disc Rsp = 6

    // CH 1 Sensors:                      //cq/co[]
    //*
    0x0A, 4 + 4 * 1, // co: int, cm/cn[]
    0x08, AA_CH_SEN,  0x12, 4 * 1,     0x0A, 2,     0x08, 11, // SENSOR_TYPE_DRIVING_STATUS
                                                              // 12
    0x0A, 2,          0x08, 10, // SENSOR_TYPE_NIGHT_DATA 10

    //*/
    /*  Requested Sensors: 10, 9, 2, 7, 6:
                      0x0A, 4 + 4*6,     //co: int, cm/cn[]
                      0x08, AA_CH_SEN,  0x12, 4*6,
                      0x0A, 2,
                                0x08, 11, // SENSOR_TYPE_DRIVING_STATUS 12
                      0x0A, 2,
                                0x08,  3, // SENSOR_TYPE_RPM            2
                      0x0A, 2,
                                0x08,  8, // SENSOR_TYPE_DIAGNOSTICS    7
                      0x0A, 2,
                                0x08,  7, // SENSOR_TYPE_GEAR           6
                      0x0A, 2,
                                0x08,  1, // SENSOR_TYPE_COMPASS       10
                      0x0A, 2,
                                0x08, 10, // SENSOR_TYPE_LOCATION       9
                      */
    // CH 2 Video Sink:
    0x0A, 4 + 4 + 11, 0x08, AA_CH_VID,
    // 800f
    0x1A, 4 + 11, // Sink: Video
    0x08, 3,      // int (codec type) 3 = Video
    // 0x10, 1,    // int (audio stream type)
    //                                                  0x1a, 8,    // f
    //                                                  //I44100 = 0xAC44 = 10
    //                                                  10 1  100 0   100 0100
    //                                                  :  -60, -40, 2
    // 48000 = 0xBB80 = 10    111 0111   000 0000     :  -128, -9, 2
    // 16000 = 0x3E80 = 11 1110 1   000 0000          :  -128, -3

    0x22, 11, // cz // Res        FPS, WidMar, HeiMar, DPI
    // DPIs:    (FPS doesn't matter ?)
    0x08, 1,          0x10, 1,         0x18, 0,     0x20, 0,  0x28,
    -96,  1, // 0x30, 0,     //  800x 480, 30 fps, 0, 0, 160 dpi    0xa0 //
             // Default 160 like 4100NEX
    // 0x30, 0,     // 1280x 720, 30 fps, 0, 0, 160 dpi    0xa0

    // 0x08, 1, 0x10, 1, 0x18, 0, 0x20, 0, 0x28, -128, 1,   //0x30, 0,     //
    // 800x 480, 30 fps, 0, 0, 128 dpi    0x80 // 160-> 128 Small, phone/music
    // close to outside
    // 0x08, 1, 0x10, 1, 0x18, 0, 0x20, 0, 0x28,  -16, 1,   //0x30, 0,     //
    // 800x 480, 30 fps, 0, 0, 240 dpi    0xf0 // 160-> 240 Big, phone/music
    // close to center

    // 60 FPS makes little difference:
    // 0x08, 1, 0x10, 2, 0x18, 0, 0x20, 0, 0x28,  -96, 1,   //0x30, 0,     //
    // 800x 480, 60 fps, 0, 0, 160 dpi    0xa0

    // Higher resolutions don't seem to work as of June 10, 2015 release of AA:
    // 0x22, 11,
    // 0x08, 2, 0x10, 1, 0x18, 0, 0x20, 0, 0x28,  -96, 1,   //0x30, 0,     //
    // 1280x 720, 30 fps, 0, 0, 160 dpi    0xa0
    // 0x08, 3, 0x10, 1, 0x18, 0, 0x20, 0, 0x28,  -96, 1,   //0x30, 0,     //
    // 1920x1080, 30 fps, 0, 0, 160 dpi    0xa0
    //*/
    //* Crashes on null Point reference without:
    // CH 3 TouchScreen/Input:
    0x0A, 4 + 2 + 6, //+2+16,
    0x08, AA_CH_TOU,
    //                                                              0x08, -128,
    //                                                              -9, 2,
    //                                                              0x10, 16,
    //                                                              0x18, 2,
    // 0x28, 0, //1,   boolean
    0x22, 2 + 6, //+2+16, // ak  Input
    // 0x0a, 16,   0x03, 0x54, 0x55, 0x56, 0x57, 0x58, 0x7e, 0x7f,   -47, 1,
    // -127, -128, 4,    -124, -128, 4,
    0x12, 6, // no int[], am      // 800 = 0x0320 = 11 0    010 0000 :
             // 32+128(-96), 6
    // 480 = 0x01e0 = 1 1     110 0000 =  96+128 (-32), 3
    0x08, -96,        6,    0x10,      -32,  3, //  800x 480
    // 0x08, -128, 10,    0x10, -48, 5,        // 1280x 720     0x80, 0x0a
    // 0xd0, 5
    // 0x08, -128, 15,    0x10, -72, 8,        // 1920x1080     0x80, 0x0f
    // 0xb8, 8
    //*/
    //*
    // CH 7 Microphone Audio Source:
    0x0A, 4 + 4 + 7,  0x08, AA_CH_MIC, 0x2A, 4 + 7, // Source: Microphone Audio
    0x08, 1, // int (codec type) 1 = Audio
    0x12, 7, // AudCfg   16000hz         16bits        1chan
    // 0x08, 0x80, 0x7d,         0x10, 0x10,   0x18, 1,
    0x08, -128,       0x7d, 0x10,      0x10, 0x18,  1,

    0x12, 4,          'V',  'o',       'l',  'v', // 1, 'A', // Car Manuf
                                                  // Part of "remembered car"
    0x1A, 4,          'X',  'C',       '9',  '0', // 1, 'B', // Car Model
    0x22, 4,          '2',  '0',       '1',  '6', // 1, 'C', // Car Year
                                                  // Part of "remembered car"
    0x2A, 4,          '0',  '0',       '0',  '1', // 1, 'D', // Car Serial
                                                  // Not Part of "remembered
                                                  // car" ?? (vehicleId=null)
    0x30, 0, // 0,                          // driverPosition
    0x3A, 4,          'G',  'i',       'n',  'o', // 1, 'E', // HU  Make / Manuf
    0x42, 4,          'H',  'U',       '1',  '6', // 1, 'F', // HU  Model
    0x4A, 4,          'S',  'W',       'B',  '1', // 1, 'G', // HU SoftwareBuild
    0x52, 4,          'S',  'W',       'V',  '1', // 1, 'H', // HU
                                                  // SoftwareVersion
    0x58, 0, // 1,//1,//0,//1,       // ? bool (or int )
             // canPlayNativeMediaDuringVr
    0x60, 0, // 1,//0,//0,//1        // mHideProjectedClock     1 = True = Hide
    // 0x68, 1,
    //*/

    // 04-22 03:43:38.049 D/CAR.SERVICE( 4306): onCarInfo
    // com.google.android.gms.car.CarInfoInternal[dbId=0,manufacturer=A,model=B,headUnitProtocolVersion=1.1,modelYear=C,vehicleId=null,
    // bluetoothAllowed=false,hideProjectedClock=false,driverPosition=0,headUnitMake=E,headUnitModel=F,headUnitSoftwareBuild=G,headUnitSoftwareVersion=H,canPlayNativeMediaDuringVr=false]

    //*
    // CH 4 Output Audio Sink:
    0x0A, 4 + 6 + 8,  0x08, AA_CH_AUD, 0x1A, 6 + 8, // Sink: Output Audio
    0x08, 1, // int (codec type) 1 = Audio
    0x10, 3, // Audio Stream Type = 3 = MEDIA
    0x1A, 8, // AudCfg   48000hz         16bits        2chan
    // 0x08, 0x80, 0xF7, 0x02,   0x10, 0x10,   0x18, 02,
    0x08, -128,       -9,   0x02,      0x10, 0x10,  0x18, 02,
    //*/
    //*
    // CH 5 Output Audio Sink1:
    0x0A, 4 + 6 + 7,  0x08, AA_CH_AU1, 0x1A, 6 + 7, // Sink: Output Audio
    0x08, 1, // int (codec type) 1 = Audio
    0x10, 1, // Audio Stream Type = 1 = TTS
    0x1A, 7, // AudCfg   16000hz         16bits        1chan
    // 0x08, 0x80, 0x7d,         0x10, 0x10,   0x18, 1,
    0x08, -128,       0x7d, 0x10,      0x10, 0x18,  1,
    //*/
    ////*
    // CH 6 Output Audio Sink2:
    0x0A, 4 + 6 + 7,  0x08, AA_CH_AU2, 0x1A, 6 + 7, // Sink: Output Audio
    0x08, 1, // int (codec type) 1 = Audio
    0x10, 2, // Audio Stream Type = 2 = SYSTEM
    0x1A, 7, // AudCfg   16000hz         16bits        1chan
    // 0x08, 0x80, 0x7d,         0x10, 0x10,   0x18, 1,
    0x08, -128,       0x7d, 0x10,      0x10, 0x18,  1,
    //*/

};

typedef int (*aa_type_ptr_t)(int chan, byte *buf, int len);

aa_type_ptr_t aa_type_array[AA_CH_MAX + 1][3][32] = {
    // 0 - 31, 32768-32799, 65504-65535
    // Sync with hu_tra.java, hu_aap.h and hu_aap.c:aa_type_array[]
    // Channel 0 Ctr Control:
    aa_pro_ctr_a00, aa_pro_ctr_a01, aa_pro_ctr_a02, aa_pro_ctr_a03,
    aa_pro_ctr_a04, aa_pro_ctr_a05, aa_pro_ctr_a06, aa_pro_all_a07,
    aa_pro_ctr_a08, aa_pro_ctr_a09, aa_pro_ctr_a0a, aa_pro_ctr_a0b,
    aa_pro_ctr_a0c, aa_pro_ctr_a0d, aa_pro_ctr_a0e, aa_pro_ctr_a0f,
    aa_pro_ctr_a10, aa_pro_ctr_a11, aa_pro_ctr_a12, NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,

    // Channel 1 Sen Sensor:
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           aa_pro_all_a07,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           aa_pro_sen_b01, NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,

    // Channel 2 Vid Video:
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           aa_pro_all_a07,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    aa_pro_snk_b00, aa_pro_vid_b01, NULL,           NULL,
    NULL,           NULL,           NULL,           aa_pro_vid_b07,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,

    // Channel 3 Tou TouchScreen:
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           aa_pro_all_a07,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           aa_pro_tou_b02, NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,

    // Channel 4 Output Audio:
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           aa_pro_all_a07,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    aa_pro_snk_b00, aa_pro_aud_b01, aa_pro_aud_b02, NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,

    // Channel 5 Output Audio1:
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           aa_pro_all_a07,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    aa_pro_snk_b00, aa_pro_aud_b01, aa_pro_aud_b02, NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,

    // Channel 6 Output Audio2:
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           aa_pro_all_a07,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    aa_pro_snk_b00, aa_pro_aud_b01, aa_pro_aud_b02, NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,

    // Channel 7 Mic Audio:
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           aa_pro_all_a07,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           aa_pro_mic_b01, NULL,           NULL,
    aa_pro_mic_b04, aa_pro_mic_b05, NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,
    NULL,           NULL,           NULL,           NULL,

};

byte aud_ack[] = {0x80, 0x04, 0x08,
                  0,    0x10, 1}; // Global Ack: 0, 1     Same as video ack ?

#endif // HU_BUFFERS_H
