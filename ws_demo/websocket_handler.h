#ifndef __WEBSOCKET_HANDLER__
#define __WEBSOCKET_HANDLER__

#include <iostream>
#include "ws_protocol.h"

class Websocket_Handler{
public:
	Websocket_Handler(int fd);
	~Websocket_Handler();
	int process();
	inline char *getBuff();
private:
	char mBuff[2048];
	int mFd;
    WS_Protocol *wsp;
};

inline char *Websocket_Handler::getBuff(){
	return mBuff;
}

#endif
