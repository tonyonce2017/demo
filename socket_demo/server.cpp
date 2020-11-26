#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>

int main() {
    int server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serv_addr;
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_addr.sin_port = htons(1234);

    int opt = 1;
    int len = sizeof(opt);
    setsockopt(server_sock, SOL_SOCKET, SO_REUSEADDR, &opt, (socklen_t) len);


    bind(server_sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    listen(server_sock, 20);

    struct sockaddr_in clnt_addr;
    socklen_t clnt_addr_size = sizeof(clnt_addr);

    int clnt_sock = accept(server_sock, (struct sockaddr*)&clnt_addr, &clnt_addr_size);
    printf("socket connected: %d\n", clnt_sock);


    while (1)
    {
        //接收客户端的请求
        char recv[40];
        memset(&recv, 0, sizeof(recv));
        read(clnt_sock, recv, sizeof(recv));
        if(strlen(recv) > 0){
            if(strcmp(recv, "quit") == 0){
                printf("good bye!\n");
                break;
            }

            printf("recv msg from client: %s\n", recv);

            //原样返回
            write(clnt_sock, recv, sizeof(recv));

        }
    }

    close(clnt_sock);
    close(server_sock);

    return 0;
}
