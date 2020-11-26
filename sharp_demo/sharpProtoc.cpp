#include "sharpProtoc.h"
#include "stdio.h"
#include "sm4.h"

SharpProtoc::SharpProtoc() {

}

SharpProtoc::~SharpProtoc() {

}

void SharpProtoc::buildHandsShakeRequest(char *data, int version, int childId, const char *din,
                                         const char *stream_token) {
    char shakeData[24];
    memset(&shakeData, 0, 24);
    //0~2 字节固定为 0x7f
    shakeData[0] |= 0x7f;
    shakeData[1] |= 0x7f;
    shakeData[2] |= 0x7f;
    //第 3 字节为版本号，协议版本号：0到2，为非加密版本，3，为AES（AES/ECB/PKCS5Padding）128加密版本，4为国密SM4（CBC）加密版本
    shakeData[3] = version;
    //第 4~6 字节为子视频流 ID，非网关设备此字段填 0
    memcpy(shakeData + 4, &childId, 3);
    //第 7 字节为码流 ID，0 为主码流。（当前版本仅支持主码流）
    shakeData[7] = 0;
    //8~15 字节为所请求视频的摄像头 din
    if (strlen(din) > 0)
        memcpy(shakeData + 8, din, 8);
    else {
        printf("din is empty!");
        data = NULL;
    }
    //16~23 字节为 stream_token
    if (strlen(stream_token) > 0)
        memcpy(shakeData + 16, stream_token, 8);
    else {
        printf("stream_token is empty!");
        data = NULL;
    }
    memcpy(data, &shakeData, 24);
}

const SharpProtoc::ShakeResponse SharpProtoc::parsingHandsShakeResponse(char *recv_buffer) {
    ShakeResponse response;
    //length：握手响应包数据总长度(4 字节)
    //TODO: 修改字节顺序
    uint32_t length = 0;
    memcpy(&length, recv_buffer, 4);
    response.length = length;
    //result: 响应码（1 字节，非 0 表示失败 if result =0 then errorinfo length=0)
    int result = 0;
    memcpy(&result, recv_buffer + 4, 1);
    response.result = result;
    //Ver: 版本号（1 字节 当前版本：1)
    int Ver = 0;
    memcpy(&Ver, recv_buffer + 5, 1);
    response.Ver = Ver;
    //A: 音频数据包编码( 4bit 1: AMR 2: AAC 6:G711）
    uint8_t AV = 0;
    memcpy(&AV, recv_buffer + 6, 1);
    response.A = AV >> 4 & 0x0f;
    response.V = AV & 0x0f;
    //E：加密方式(1 字节
    // 0到2:表示不加密
    // 3: AES-128（AES/ECB/PKCS5Padding）(AES-128为版本3所使用的加密算法)
    // 4: 国密SM4（CBC）加密方式 (其中加密key为查询视频推流服务器时返回的stream_en_key值，向量值为sm4_vector)
    uint8_t E = 0;
    memcpy(&E, recv_buffer + 7, 1);
    response.E = E;
    //start timestamp: 视频起始时间戳（8 字节）
    memcpy(&response.start_timestamp, recv_buffer + 8, 8);
    //error info ：失败错误提示字符串(length - 16 字节),result 为 0 时无此节
    if (response.result != 0) {
        memcpy(&response.error_info, recv_buffer + 16, 16);
    }
    return response;
}

std::string SharpProtoc::buildDataBlock_v2(char *frameData, size_t size, int type, int frame, char *timestamp,
                                           uint32_t sequence_number) {
    std::string total;
    //length：每一帧视频数据和包头总长（长度由前 3 个字节表示)
    uint32_t length = size + 12;
    total.append((const char *) (&length), 3);
    //type：(4 bit 1：视频 2: 音频 0xF：结束帧，表示音视频结束，连接可以断开）
    //frame: 帧类型 (4bit：1:表示是关键 I 帧)
    char t = 0;
    t = t | type << 4;
    t = t | frame;
    total.append(&t, 1);
    //timestamp：时间戳(4 字节) – 这里指的是偏移量
    total.append(timestamp, 4);
    //sequence number: 序列号(4 字节），后跟视频裸帧数据，长度为：length-12
    total.append((const char *) (&sequence_number), 4);
    //视频帧数据
    total.append(frameData, size);
//    printf("\n");
//    for(int i=0; i<total.length(); ++i){
//        printf("%02x ", total[i]);
//    }
    return total;
}

std::string
SharpProtoc::buildDataBlock_v3v4(char *frameData, size_t size, int type, int frame, uint16_t encryptedLength,
                                 unsigned char *stream_en_key, unsigned char *sm4_vector, char *timestamp,
                                 uint32_t sequence_number) {
    std::string total;
    //length：每一帧视频数据和包头总长（长度由前 3 个字节表示)
    uint32_t length = size + 16;
    total.append((const char *) (&length), 3);
    //type：(4 bit 1：视频 2: 音频 0xF：结束帧，表示音视频结束，连接可以断开）
    //frame: 帧类型 (4bit：1:表示是关键 I 帧)
    char t = 0;
    t = t | type << 4;
    t = t | frame;
    total.append(&t, 1);
    //encryptedLength: 加密数据字节长度(16bit：
    // 0表示该帧无加密数据，即为原始数据；
    // 如果加密数据长度大于零时，密钥为获取服务器IP地址时返回的stream_en_key和sm4_vector,
    // 还原原始帧数据方法为：加密数据解密后的数据，加上除去加密数据后的数据，
    // 如：收到帧数据为data[encryptedLength]+data[length-16- encryptedLength]
    // 则解密后的数据为：decrypt（data[encryptedLength]）+ data[length-16- encryptedLength])
    total.append((const char *) (&encryptedLength), 2);
    if (encryptedLength > 0) {
        //加密
//        if(encryptedLength%16 > 0)

        char ciphertext[encryptedLength];
        memset(ciphertext, 0, encryptedLength);
        memcpy(ciphertext, ciphertext, encryptedLength);
//        unsigned char out;
        sm4_context ctx;
        sm4_setkey_dec(&ctx, stream_en_key);
//        sm4_crypt_cbc(&ctx, 0, 16, sm4_vector, ciphertext);

    }

    //timestamp：时间戳(4 字节) – 这里指的是偏移量
    total.append(timestamp, 4);
    //sequence number: 序列号(4 字节），后跟视频裸帧数据，长度为：length-12
    total.append((const char *) (&sequence_number), 4);
    //视频帧数据
    total.append(frameData, size);
//    printf("\n");
//    for(int i=0; i<total.length(); ++i){
//        printf("%02x ", total[i]);
//    }
    return total;
}
