#include <unistd.h>
#include "websocket_handler.h"

Websocket_Handler::Websocket_Handler(int fd):
        mBuff(),
        mFd(fd),
        wsp(new WS_Protocol)
{
}

Websocket_Handler::~Websocket_Handler(){
    delete wsp;
}

int Websocket_Handler::process(){
    int size = 0;
    for(int i=2047; i>=0; --i){
        if(mBuff[i] == '\0')
            continue;
        size = i+1;
        break;
    }
    if(size == 0)
        return -1;

	if(wsp->handleRecv(mBuff, size)){
        const std::vector<std::string> &send = wsp->getSendPkg();
        if(send.size()>0){
            for(auto it : send){
                write(mFd, it.data(), strlen(it.data()));
                printf("回应请求!\n");
            }
            wsp->clearSendPkg();
        }else{
            //handle receive
            const std::vector<std::string> &recv = wsp->getRecvPkg();
            if(recv.size()>0){
                for(auto it : recv){
                    printf("收到请求: %s \n", it.data());
                    //要向客户端发送请求使用write(mFd, wsp->buildPkg(), size_t);
                }
                wsp->clearRecvPkg();
            }
        }
    }
	memset(mBuff, 0, sizeof(mBuff));
	return 0;
}
