#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024
void ErrorHandling(char* message);

int main(int argc, char* argv[]) {
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAdr, clntAdr;
    TIMEVAL timeout;
    fd_set reads, cpyReads;

    int adrSz;
    int strLen, fdNum, i;
    char buf[BUF_SIZE];

    if (argc != 2) {
        printf("Usage : %s <port>\n", argv[0]);
        exit(1);
    }

    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        ErrorHandling("WSAStartup() error!");
    }

    // 소켓 생성
    hServSock = socket(PF_INET, SOCK_STREAM, 0);
    memset(&servAdr, 0, sizeof(servAdr));
    servAdr.sin_family = AF_INET;
    servAdr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAdr.sin_port = htons(atoi(argv[1]));

    // IP, PORT 할당
    if (bind(hServSock, (SOCKADDR*)&servAdr, sizeof(servAdr)) == SOCKET_ERROR) {
        ErrorHandling("bind() error");
    }

    // 연결 요청을 받을 수 있도록 함
    if (listen(hServSock, 5) == SOCKET_ERROR) {
        ErrorHandling("listen() error");
    }

    FD_ZERO(&reads);           // 변수 초기화
    FD_SET(hServSock, &reads); // 서버 소켓 등록

    while (1) {
        cpyReads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        if ((fdNum = select(0, &cpyReads, 0, 0, &timeout)) == SOCKET_ERROR) {
            break;
        }
        if (fdNum == 0) {
            continue;
        }

        for (i = 0; i < reads.fd_count; i++) {
            if (FD_ISSET(reads.fd_array[i], &cpyReads)) {
                if (reads.fd_array[i] == hServSock) {
                    adrSz = sizeof(clntAdr);
                    //연결요청 수락
                    hClntSock = accept(hServSock, (SOCKADDR*)&clntAdr, &adrSz);
                    FD_SET(hClntSock, &reads);
                    printf("connected client: %d\n", hClntSock);
                } else {
                    // 데이터 수신
                    strLen = recv(reads.fd_array[i], buf, BUF_SIZE - 1, 0);
                    if (strLen == 0) {
                        FD_CLR(reads.fd_array[i], &reads);
                        closesocket(cpyReads.fd_array[i]);
                        printf("closed client: %d\n", cpyReads.fd_array[i]);
                    } else {
                        // 데이터 송신
                        send(reads.fd_array[i], buf, strLen, 0);
                    }
                }
            }
        }
    }
    closesocket(hServSock);
    WSACleanup();
    return 0;
}

void ErrorHandling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}