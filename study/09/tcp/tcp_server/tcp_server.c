#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>

void ErrorHandling(char* message);

int main(int argc, char** argv) {
    WSADATA wsaData;
    SOCKET hServSock, hClntSock;
    SOCKADDR_IN servAddr, clntAddr;

    int szClntAddr;
    char message[] = "202013128 ���ʰ�"; // Ŭ���̾�Ʈ�� ���� �޽���

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // ���� ���̺귯�� �ʱ�ȭ
        ErrorHandling("WSAStartup() error!");
    }

    hServSock = socket(PF_INET, SOCK_STREAM, 0); // 1. ���� ����,  PF_INET(IPv4), SOCK_STREAM(���������� TCP)
    if (hServSock == INVALID_SOCKET) {
        ErrorHandling("socket() error!");
    }

    memset(&servAddr, 0, sizeof(servAddr)); // ���� �ּ����� �ʱ�ȭ
    servAddr.sin_family = AF_INET;          // IPv4 �Ҵ�
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[1])); // ��Ʈ

    if (bind(hServSock, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) { // 2. ���ε�, IP�ּҿ� Port��ȣ �Ҵ�
        ErrorHandling("bind() error!");
    }

    if (listen(hServSock, 5) == SOCKET_ERROR) { // 3. listen ���������� ���� �������� �ϼ�. Ŭ���̾�Ʈ ������
        ErrorHandling("listen() error!");
    }

    szClntAddr = sizeof(clntAddr);
    hClntSock = accept(hServSock, (SOCKADDR*)&clntAddr, &szClntAddr); // 4. accept Ŭ���̾�Ʈ �����û ����
    if (hClntSock == INVALID_SOCKET) {
        ErrorHandling("accept() error!");
    }

    // Ŭ���̾�Ʈ�� ���� ����
    send(hClntSock, message, sizeof(message), 0); // send �Լ� ȣ��� ����� Ŭ���̾�Ʈ���� ������ ����
    closesocket(hClntSock);
    closesocket(hServSock);
    WSACleanup(); // �ʱ�ȭ�� ���� ���̺귯���� ����
    return 0;
}

void ErrorHandling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}
