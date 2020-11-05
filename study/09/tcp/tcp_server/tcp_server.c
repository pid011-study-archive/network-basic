#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>

void ErrorHandling(char* message);

int main(int argc, char** argv) {
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;

    int szClntAddr;
    char message[] = "202013128 곽필경"; // 클라이언트로 보낼 메시지

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // 소켓 라이브러리 초기화
        ErrorHandling("WSAStartup() error!");
    }

    hServSock = socket(PF_INET, SOCK_STREAM, 0); // 1. 소켓 생성,  PF_INET(IPv4), SOCK_STREAM(연결지향형 TCP)
    if (hServSock == INVALID_SOCKET) {
        ErrorHandling("socket() error!");
    }

    memset(&servAddr, 0, sizeof(servAddr)); // 서버 주소정보 초기화
    servAddr.sin_family = AF_INET;          // IPv4 할당
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1])); // 포트

    if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) { // 2. 바인드, IP주소와 Port번호 할당
        ErrorHandling("bind() error!");
    }

    if (listen(hServSock, 5) == SOCKET_ERROR) { // 3. listen 생성소켓을 서버 소켓으로 완성. 클라이언트 대기상태
        ErrorHandling("listen() error!");
    }

    szClntAddr = sizeof(clntAddr);
    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr); // 4. accept 클라이언트 연결요청 수락
    if (hClntSock == INVALID_SOCKET) {
        ErrorHandling("accept() error!");
    }

    // 클라이언트가 접속 성공
    send(hClntSock, message, sizeof(message), 0); // send 함수 호출로 연결된 클라이언트에게 데이터 전송
    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup(); // 초기화한 소켓 라이브러리를 해제
    return 0;
}

void ErrorHandling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
