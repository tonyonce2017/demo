#ifndef WS_PROTOCOL_H
#define WS_PROTOCOL_H

#include <string.h>
#include <vector>
#include <map>
#include "base64.h"
#include "sha1.h"
#include <arpa/inet.h>

class WS_Protocol {
public:
    WS_Protocol();
    const std::map<std::string, std::string> &getParam() const;

    bool isClose() const;
    bool isWConnected() const;

    const std::vector<std::string> &getSendPkg();
    void clearSendPkg();
    const std::vector<std::string> &getRecvPkg();
    void clearRecvPkg();

    bool handleRecv(const char *buff, size_t len);
    std::string buildPkg(const std::string &dataBody, int opcode = 0x01);
protected:
    bool parsingShakeHands(const char *buff, size_t len);
    void addSendPkg(const std::string &data);
    void addRecvPkg(const std::string &data);
    int strSplit(const std::string &str, std::vector<std::string> &ret_, std::string sep = ",");
    std::string sha1andBase64Encode(const std::string &strSrc);

private:
    int statusWebSocketConnection;
    bool bIsClose;
    std::string cacheRecvData;
    std::map<std::string, std::string> dictParams;
    std::vector<std::string> listSendPkg;//!需要发送的数据
    std::vector<std::string> listRecvPkg;//!已经接收的
    std::string dataFragmentation;//!缓存分片数据
};

#endif //WS_PROTOCOL_H
