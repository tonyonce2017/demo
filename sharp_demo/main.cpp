//
// Created by Administrator on 2020/11/19.
//

#include "iostream"
#include "sharpProtoc.h"

int main(int argc, char *argv[]) {
//    SharpProtoc sharp;
    char request[24];
    SharpProtoc::buildHandsShakeRequest(request, 4, 0, "144115194519080010", "2C9A8F42");
    if(request){
        for(int i=0; i< 24; ++i){
            printf("%02x ", request[i]);
        }
        printf("\n");
    }

    char response[32] = {0x00, 0x00, 0x30, 0x30,
                                  0x00,
                                  0x01,
                                  0x21,
                                  0x04,
                                  '0', '5', '7', '5', '5', '3', '0', '3',
                                  'h', 'e', 'l', 'l', 'o', 'w', 'o', 'r', 'l', 'd', 'w', 'h', 'o', 'a', 'r', 'y'};
    SharpProtoc::ShakeResponse res = SharpProtoc::parsingHandsShakeResponse(response);
    printf("result is: %d", res.result);

    char block[6] = {0x00, 0x21, 0x15, 0x11, 0x34, 0x21};
    char * time = (char *)"time";
    SharpProtoc::buildDataBlock_v2(block, 6, 2, 1, time, 15698);
    return 0;
}