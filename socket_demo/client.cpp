//
// Created by Administrator on 2020/11/23.
//

#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);

    int sock = socket(AF_INET, SOCK_STREAM, 0);
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    while(1){

        char str[40];
        printf("write msg to server(quit to close):\n");
        scanf("%s", &str);

        //发送请求请求
        write(sock, str, sizeof(str));

        if(strcmp(str, "quit") == 0){
            printf("good bye!\n");
            break;
        }

        //接收服务端的消息
        char buff[40];
        read(sock, buff, sizeof(buff));
        printf("recv msg from server: %s\n", buff);

    }
    close(sock);
    return 0;
}
