#include "ws_protocol.h"

uint64_t
ntoh64(const uint64_t *input) {
    uint64_t rval;
    uint8_t *data = (uint8_t *) &rval;

    data[0] = *input >> 56;
    data[1] = *input >> 48;
    data[2] = *input >> 40;
    data[3] = *input >> 32;
    data[4] = *input >> 24;
    data[5] = *input >> 16;
    data[6] = *input >> 8;
    data[7] = *input >> 0;

    return rval;
}

uint64_t
hton64(const uint64_t *input) {
    return (ntoh64(input));
}

WS_Protocol::WS_Protocol() :
        statusWebSocketConnection(0),
        bIsClose(false) {
}

bool WS_Protocol::isClose() const {
    return bIsClose;
}

bool WS_Protocol::isWConnected() const {
    return statusWebSocketConnection == 1;
}

const std::map<std::string, std::string> &WS_Protocol::getParam() const {
    return dictParams;
}

const std::vector<std::string> &WS_Protocol::getSendPkg() {
    return listSendPkg;
}

void WS_Protocol::clearSendPkg() {
    listSendPkg.clear();
}

void WS_Protocol::addSendPkg(const std::string &data) {
    listSendPkg.push_back(data);
}

const std::vector<std::string> &WS_Protocol::getRecvPkg() {
    return listRecvPkg;
}

void WS_Protocol::clearRecvPkg() {
    listRecvPkg.clear();
}

void WS_Protocol::addRecvPkg(const std::string &data) {
    if (data.size() > 0) {
        listRecvPkg.push_back(data);
    }
}

int WS_Protocol::strSplit(const std::string &str, std::vector<std::string> &ret_, std::string sep) {
    if (str.empty()) {
        return 0;
    }

    std::string tmp;
    std::string::size_type pos_begin = str.find_first_not_of(sep);
    std::string::size_type comma_pos = 0;

    while (pos_begin != std::string::npos) {
        comma_pos = str.find(sep, pos_begin);
        if (comma_pos != std::string::npos) {
            tmp = str.substr(pos_begin, comma_pos - pos_begin);
            pos_begin = comma_pos + sep.length();
        } else {
            tmp = str.substr(pos_begin);
            pos_begin = comma_pos;
        }

        if (!tmp.empty()) {
            ret_.push_back(tmp);
            tmp.clear();
        }
    }
    return 0;
}

bool WS_Protocol::handleRecv(const char *buff, size_t len) {
    if (statusWebSocketConnection == -1) {
        return false;
    }
    if (dictParams.empty()) {
        //握手
        return parsingShakeHands(buff, len);
    }
    //接收请求
    cacheRecvData.append(buff, len);
    int nFIN = ((cacheRecvData[0] & 0x80) == 0x80) ? 1 : 0;
    int nOpcode = cacheRecvData[0] & 0x0F;
    //int nMask = ((cacheRecvData[1] & 0x80) == 0x80) ? 1 : 0; //!this must be 1
    uint64_t nPayload_length = cacheRecvData[1] & 0x7F;
    int nPlayLoadLenByteNum = 1;
    int nMaskingKeyByteNum = 4;

    if (nPayload_length == 126) {
        nPlayLoadLenByteNum = 3;
    } else if (nPayload_length == 127) {
        nPlayLoadLenByteNum = 9;
    }
    if ((int) cacheRecvData.size() < (1 + nPlayLoadLenByteNum + nMaskingKeyByteNum)) {
        return true;
    }
    printf("payload_length: %d\n", nPayload_length);
    if (nPayload_length == 126) {
        uint16_t tmpLen = 0;
        memcpy(&tmpLen, cacheRecvData.c_str() + 2, 2);
        nPayload_length = ntohs((int16_t) tmpLen);
    } else if (nPayload_length == 127) {
        uint64_t tmpLen = 0;
        memcpy(&tmpLen, cacheRecvData.c_str() + 2, 8);
        nPayload_length = ntoh64(&tmpLen);
    }

    //数据长度不够, 分多包接收
    if ((int) cacheRecvData.size() < (1 + nPlayLoadLenByteNum + nMaskingKeyByteNum + nPayload_length)) {
        return true;
    }

    std::string aMasking_key;
    aMasking_key.assign(cacheRecvData.c_str() + 1 + nPlayLoadLenByteNum, nMaskingKeyByteNum);
    std::string aPayload_data;
    aPayload_data.assign(cacheRecvData.c_str() + 1 + nPlayLoadLenByteNum + nMaskingKeyByteNum, nPayload_length);
    int nLeftSize = cacheRecvData.size() - (1 + nPlayLoadLenByteNum + nMaskingKeyByteNum + nPayload_length);

    //判断是否粘包
    if (nLeftSize > 0) {
        std::string leftBytes;
        leftBytes.assign(cacheRecvData.c_str() + 1 + nPlayLoadLenByteNum + nMaskingKeyByteNum + nPayload_length,
                         nLeftSize);
        cacheRecvData = leftBytes;
    } else {
        cacheRecvData.clear();
    }

    for (int i = 0; i < nPayload_length; i++) {
        aPayload_data[i] = (char) (aPayload_data[i] ^ aMasking_key[i % nMaskingKeyByteNum]);
    }

    //是否特殊指令
    if (nOpcode == 8) {
        addSendPkg(buildPkg("", nOpcode));// close
        bIsClose = true;
    } else if (2 == nOpcode || 1 == nOpcode || 0 == nOpcode || 9 == nOpcode) {
        if (9 == nOpcode)//!ping
        {
            addSendPkg(buildPkg("", 0xA));// pong
        }

        if (nFIN == 1) {
            if (dataFragmentation.size() == 0) {
                addRecvPkg(aPayload_data);
            } else {
                dataFragmentation += aPayload_data;
                addRecvPkg(dataFragmentation);
                dataFragmentation.clear();
            }
        } else {
            dataFragmentation += aPayload_data;
        }
    }
    return true;
}

std::string WS_Protocol::buildPkg(const std::string &dataBody, int opcode) {
    int nBodyLenByteNum = 0;
    if (dataBody.size() >= 65536) {
        nBodyLenByteNum = 8;
    } else if (dataBody.size() >= 126) {
        nBodyLenByteNum = 2;
    }
    std::string ret = "  ";
    ret[0] = 0;
    ret[1] = 0;
    ret[0] |= 0x80;//!_fin
    ret[0] |= (char) opcode;//0x01;//! opcode 1 text 2 binary

    if (nBodyLenByteNum == 0) {
        ret[1] = (char) dataBody.size();
        ret += dataBody;
    } else if (nBodyLenByteNum == 2) {
        ret[1] = 126;

        uint16_t extsize = (uint16_t) dataBody.size();
        int16_t extsizeNet = htons((int16_t) extsize);
        ret.append((const char *) (&extsizeNet), sizeof(extsizeNet));
        ret += dataBody;
    } else {
        ret[1] = 127;
        //Array.Copy(dataBody, 0, ret, 10, dataBody.Length);

        size_t extsize = dataBody.size();
        uint64_t extsizeNet = hton64(&extsize);//System.Net.IPAddress.HostToNetworkOrder((long)extsize);
        ret.append((const char *) (&extsizeNet), sizeof(extsizeNet));
        ret += dataBody;
    }
    return ret;
}

std::string WS_Protocol::sha1andBase64Encode(const std::string &strSrc) {
    SHA1 sha;
    unsigned int message_digest[5];
    sha.Reset();
    sha << strSrc.c_str();

    sha.Result(message_digest);
    for (int i = 0; i < 5; i++) {
        message_digest[i] = htonl(message_digest[i]);
    }
    return base64_encode(reinterpret_cast<const unsigned char *>(message_digest), 20);
}

bool WS_Protocol::parsingShakeHands(const char *buff, size_t len) {
    std::string strRecvData;
    strRecvData.append(buff, len);
    if (strRecvData.size() >= 3) {
        if (strRecvData.find("GET") == std::string::npos) {
            statusWebSocketConnection = -1;
            return false;
        }
    } else if (strRecvData.size() >= 2) {
        if (strRecvData.find("GE") == std::string::npos) {
            statusWebSocketConnection = -1;
            return false;
        }
    } else {
        if (strRecvData.find("G") == std::string::npos) {
            statusWebSocketConnection = -1;
            return false;
        }
    }
    statusWebSocketConnection = 1;
    if (strRecvData.find("\r\n\r\n") == std::string::npos)//!header data not end
    {
        return true;
    }
    if (strRecvData.find("Upgrade: websocket") == std::string::npos) {
        statusWebSocketConnection = -1;
        return false;
    }

    std::vector<std::string> strLines;
    strSplit(strRecvData, strLines, "\r\n");
    for (size_t i = 0; i < strLines.size(); ++i) {
        const std::string &line = strLines[i];
        std::vector<std::string> strParams;
        strSplit(line, strParams, ": ");
        if (strParams.size() == 2) {
            dictParams[strParams[0]] = strParams[1];
        } else if (strParams.size() == 1 && strParams[0].find("GET") != std::string::npos) {
            dictParams["PATH"] = strParams[0];
        }
    }
    if (dictParams.find("Sec-WebSocket-Key") != dictParams.end()) {
        const std::string &Sec_WebSocket_Key = dictParams["Sec-WebSocket-Key"];
        std::string strGUID = Sec_WebSocket_Key + "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        std::string strHashBase64 = sha1andBase64Encode(strGUID);

        char buff[512] = {0};
        snprintf(buff, sizeof(buff),
                 "HTTP/1.1 101 Switching Protocols\r\nUpgrade: websocket\r\nConnection: Upgrade\r\nSec-WebSocket-Accept: %s\r\n\r\n",
                 strHashBase64.c_str());
        addSendPkg(buff);
    } else {
        statusWebSocketConnection = -1;
        return false;
    }
    return true;
}
