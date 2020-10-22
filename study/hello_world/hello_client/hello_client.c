#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>

void ErrorHandling(char* message);

int main(int argc, char** argv) {
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    char message[30]; // 서버에서 오는 메시지를 받을 공간을 선언
    int strLen;
    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // 소켓 라이브러리 초기화
        ErrorHandling("WSAStartup() error!");
    }

    hSocket = socket(PF_INET, SOCK_STREAM, 0); // 1. 소켓 생성
    if (hSocket == INVALID_SOCKET) {
        ErrorHandling("socket() error");
    }

    memset(&servAddr, 0, sizeof(servAddr));        // 주소 정보 초기화
    servAddr.sin_family = AF_INET;                 // IPv4 사용
    servAddr.sin_addr.s_addr = inet_addr(argv[1]); // IP주소
    servAddr.sin_port = htons(atoi(argv[2]));      // Port번호

    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) { // 2. connect로 서버에 연결 요청
        ErrorHandling("connect() error");
    }

    strLen = recv(hSocket, message, sizeof(message) - 1, 0); // recv함수를 통해 서버로부터 데이터 수신
    if (strLen == -1) {
        ErrorHandling("read() error!");
    }
    printf("Message from server: %s\n", message); // 서버로 받은 메시지 출력

    closesocket(hSocket);
    WSACleanup(); // 초기화한 소켓 라이브러리 해제
    return 0;
}

void ErrorHandling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}