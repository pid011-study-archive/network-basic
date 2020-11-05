#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>

void error_handling(const char* message);

int main(int argc, const char** argv) {
    WSADATA wsa_data;
    SOCKET h_socket;
    SOCKADDR_IN serv_addr;

    char message[30];
    int str_len = 0;
    int idx = 0, read_len = 0;

    if (argc != 3) {
        printf("Usage: %s <IP> <PORT>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        error_handling("WSAStartup() error!");
    }

    h_socket = socket(PF_INET, SOCK_STREAM, 0);

    if (h_socket == INVALID_SOCKET) {
        error_handling("socket() error!");
    }

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    serv_addr.sin_port = htons(atoi(argv[2]));

    if (connect(h_socket, (SOCKADDR*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
        error_handling("connect() error!");
    }

    while (read_len = recv(h_socket, &message[idx++], 1, 0)) {
        if (read_len == -1) {
            error_handling("read() error!");
        }

        str_len += read_len;
    }

    printf("Message from server: %s\n", message);
    printf("Function read call count: %d\n", str_len);

    closesocket(h_socket);
    WSACleanup();

    return 0;
}

void error_handling(const char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
