#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>

void ErrorHandling(char* message);

int main(int argc, char** argv) {
    WSADATA wsaData;
    SOCKET hSocket;
    SOCKADDR_IN servAddr;

    char message[30]; // �������� ���� �޽����� ���� ������ ����
    int strLen;
    if (argc != 3) {
        printf("Usage: %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) { // ���� ���̺귯�� �ʱ�ȭ
        ErrorHandling("WSAStartup() error!");
    }

    hSocket = socket(PF_INET, SOCK_STREAM, 0); // 1. ���� ����
    if (hSocket == INVALID_SOCKET) {
        ErrorHandling("socket() error");
    }

    memset(&servAddr, 0, sizeof(servAddr));        // �ּ� ���� �ʱ�ȭ
    servAddr.sin_family = AF_INET;                 // IPv4 ���
    servAddr.sin_addr.s_addr = inet_addr(argv[1]); // IP�ּ�
    servAddr.sin_port = htons(atoi(argv[2]));      // Port��ȣ

    if (connect(hSocket, (SOCKADDR*)&servAddr, sizeof(servAddr)) == SOCKET_ERROR) { // 2. connect�� ������ ���� ��û
        ErrorHandling("connect() error");
    }

    strLen = recv(hSocket, message, sizeof(message) - 1, 0); // recv�Լ��� ���� �����κ��� ������ ����
    if (strLen == -1) {
        ErrorHandling("read() error!");
    }
    printf("Message from server: %s\n", message); // ������ ���� �޽��� ���

    closesocket(hSocket);
    WSACleanup(); // �ʱ�ȭ�� ���� ���̺귯�� ����
    return 0;
}

void ErrorHandling(char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}