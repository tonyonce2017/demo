#ifndef SHARP_H
#define SHARP_H

#include <string>
#include "cstring"

class SharpProtoc
{
public:
    /*
    length：握手响应包数据总长度(4 字节)
    result: 响应码（1 字节，非 0 表示失败 if result =0 then errorinfo length=0)
    Ver: 版本号（1 字节 当前版本：1)
    A: 音频数据包编码( 4bit 1: AMR 2: AAC 6:G711）
    V：视频数据包编码( 4bit 1: H.264 2: H.265）
    E：加密方式(1 字节 0到2:表示不加密 3: AES-128（AES/ECB/PKCS5Padding）(AES-128为版本3所使用的加密算法) 4: 国密SM4（CBC）加密方式 (其中加密key为查询视频推流服务器时返回的stream_en_key值，向量值为sm4_vector)
    start timestamp: 视频起始时间戳（8 字节）
    error info ：失败错误提示字符串(length - 16 字节),result 为 0 时无此节
    */
    struct ShakeResponse
    {
        uint32_t length;
        int result;
        int Ver = 1;
        int A;
        int V;
        int E;
        char start_timestamp[8];
        char error_info[16];
    };

    SharpProtoc();
    ~SharpProtoc();

    static void buildHandsShakeRequest(char *data, int version, int childId, const char *din, const char *stream_token);
    static const SharpProtoc::ShakeResponse parsingHandsShakeResponse(char* recv_buffer);

    static std::string buildDataBlock_v2(char *frameData, size_t size, int type, int frame, char *timestamp,
                                  uint32_t sequence_number);

    static std::string buildDataBlock_v3v4(char *frameData, size_t size, int type, int frame, uint16_t encryptedLength,
                                           unsigned char *stream_en_key, unsigned char *sm4_vector, char *timestamp,
                                           uint32_t sequence_number);
};


#endif //SHARP_H