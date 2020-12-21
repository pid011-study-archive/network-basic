#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 1024

enum Input {
    NEED_PROBLEM,
    ANSWER_PROBLEM
} typedef input_t;

char _buffer[BUF_SIZE];

/**
 * @brief 콘솔에서 입력을 받아서 버퍼에 저장한다.
 * @param buffer 입력을 저장할 버퍼
 * @param max 버퍼의 길이
*/
void console_input(char* buffer, int max);
void init_buffer();
void error_handling(const char* message);

int main(int argc, char** argv) {
    WSADATA wsa_data;
    SOCKET client_socket;
    SOCKADDR_IN server_adress;

    if (argc != 3) {
        printf("Usage : %s <IP> <port>\n", argv[0]);
        exit(1);
    }

    // winsock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        error_handling("WSAStartup() error!");
    }

    // 소켓 생성
    client_socket = socket(PF_INET, SOCK_STREAM, 0);
    if (client_socket == INVALID_SOCKET) {
        error_handling("socket() error");
    }

    memset(&server_adress, 0, sizeof(server_adress));
    server_adress.sin_family = AF_INET;                 // IPv4
    server_adress.sin_addr.s_addr = inet_addr(argv[1]); // IP
    server_adress.sin_port = htons(atoi(argv[2]));      // PORT, Endian Conversions

    // 연결요청
    if (connect(client_socket, (SOCKADDR*)&server_adress, sizeof(server_adress)) == SOCKET_ERROR) {
        error_handling("connect() error!");
    } else {
        puts("Connected...........");
    }

    init_buffer(_buffer, BUF_SIZE);

    while (1) {
        char input_buffer[1];
        size_t problem_number;

        // 문제번호, 문제내용 수신 및 출력
        {
            input_buffer[0] = (char)NEED_PROBLEM;
            send(client_socket, input_buffer, sizeof(char), 0);

            recv(client_socket, _buffer, BUF_SIZE, 0);
            problem_number = atoi(_buffer);

            init_buffer();
            recv(client_socket, _buffer, BUF_SIZE, 0);
            printf("문제: %s\n", _buffer);
        }

        // 문제번호, 문제의 답 송신 및 결과 확인
        {
            char user_input[BUF_SIZE];
            memset(user_input, '\0', BUF_SIZE);
            printf("답: ");
            console_input(user_input, BUF_SIZE);

            input_buffer[0] = (char)ANSWER_PROBLEM;
            send(client_socket, input_buffer, sizeof(char), 0);

            init_buffer();
            sprintf(_buffer, "%d", problem_number);
            send(client_socket, _buffer, BUF_SIZE, 0);

            send(client_socket, user_input, BUF_SIZE, 0);

            init_buffer();
            recv(client_socket, _buffer, BUF_SIZE, 0);

            printf("%s\n", _buffer);
        }

        // 계속할건지 확인
        {
            // char ch = getchar(); // 콘솔 버퍼 비우기

            init_buffer();
            printf("그만하실건가요? (\"나가기\" 입력 시 종료): ");
            console_input(_buffer, BUF_SIZE);

            if (!strcmp(_buffer, "나가기")) {
                break;
            }
            fputs("\n", stdout);
        }
    }

    closesocket(client_socket);
    WSACleanup();
    return 0;
}

void console_input(char* buffer, int max) {
    fgets(buffer, max, stdin);
    buffer[strlen(buffer) - 1] = '\0';
}

void init_buffer() {
    memset(_buffer, '\0', BUF_SIZE);
}

void error_handling(const char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}