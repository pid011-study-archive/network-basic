#include <WinSock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define BUF_SIZE 1024

const size_t PROBLEM_COUNT = 7U;

const char* problems[] = {
    "여름마다 일어나는 전쟁은?",
    "오리도 지랄하면 날 수 있다를 4글자로 하면?",
    "식인종에게 세상에서 제일 빠른 선수 우사인볼트는 무엇인가?",
    "세상에서 가장 가난한 왕은?",
    "돌 잔치를 영어로 하면?",
    "방금 화장실에서 나온 사람은 어느나라 사람인가?",
    "우유가 쓰러지면서 하는 말은?",
};

const char* correct_answers[] = {
    "더워",
    "오리지날",
    "패스트푸드",
    "최저임금",
    "락페스티벌",
    "일본사람",
    "아야",
};

enum Input {
    NEED_PROBLEM,
    ANSWER_PROBLEM
} typedef input_t;

char _buffer[BUF_SIZE];

/**
 * @brief 문제번호를 랜덤으로 반환한다.
 * @return 랜덤한 문제번호
*/
size_t get_problem_number();

/**
 * @brief 문제의 답이 맞는 지 확인하고 비교결과를 반환한다.
 * @param problem_number 문제번호
 * @param answer 유저가 제출한 답
 * @return 맞으면 1, 틀리면 0을 반환
*/
int check_answer(size_t problem_number, const char* answer);

void init_buffer();

/**
 * @brief 에러 핸들링 함수
 * @param message 
*/
void error_handling(const char* message);

int main(int argc, char** argv) {
    WSADATA wsa_data;
    SOCKET server_socket, client_socket;
    SOCKADDR_IN server_adress, client_adress;

    if (argc != 2) {
        printf("Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    // 소켓 라이브러리 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data)) {
        error_handling("WSAStartup() error!");
    }

    // 소켓 생성
    server_socket = socket(PF_INET, SOCK_STREAM, 0);
    memset(&server_adress, 0, sizeof(server_adress));
    server_adress.sin_family = AF_INET;
    server_adress.sin_addr.s_addr = htonl(INADDR_ANY);
    server_adress.sin_port = htons(atoi(argv[1]));

    // IP, PORT 할당
    if (bind(server_socket, (SOCKADDR*)&server_adress, sizeof(server_adress)) == SOCKET_ERROR) {
        error_handling("bind() eror!");
    }

    // 연결요청 수락 설정
    if (listen(server_socket, 5) == SOCKET_ERROR) {
        error_handling("listen() error!");
    }

    TIMEVAL timeout;
    fd_set reads, copy_reads;

    FD_ZERO(&reads);
    FD_SET(server_socket, &reads);

    srand(time(NULL));

    fputs("Server started!\n", stdout);

    while (1) {
        copy_reads = reads;
        timeout.tv_sec = 5;
        timeout.tv_usec = 5000;

        int fd_num = select(0, &copy_reads, 0, 0, &timeout);
        if (fd_num == SOCKET_ERROR) {
            break;
        }

        if (fd_num == 0) {
            continue;
        }

        for (size_t i = 0; i < reads.fd_count; i++) {
            if (FD_ISSET(reads.fd_array[i], &copy_reads)) {
                if (reads.fd_array[i] == server_socket) {
                    int adress_size = sizeof(client_adress);

                    // 연결요청 수락
                    client_socket = accept(server_socket, (SOCKADDR*)&client_adress, &adress_size);
                    FD_SET(client_socket, &reads);
                    printf("connected client: %d\n", client_socket);
                } else {
                    char input_buffer[1];

                    // INPUT 값 수신
                    int str_len = recv(reads.fd_array[i], input_buffer, sizeof(char), 0);
                    if (str_len == 0) {
                        FD_CLR(reads.fd_array[i], &reads);
                        closesocket(copy_reads.fd_array[i]);
                        printf("closed client: %d\n", copy_reads.fd_array[i]);
                    } else {
                        input_t user_input = (input_t)input_buffer[0];
                        size_t problem_number;

                        switch (user_input) {
                        case NEED_PROBLEM:
                            // 문제 번호 송신
                            init_buffer();
                            problem_number = get_problem_number();
                            sprintf(_buffer, "%d", problem_number);
                            send(reads.fd_array[i], _buffer, BUF_SIZE, 0);

                            init_buffer();
                            // 문제 내용 송신
                            strcpy(_buffer, problems[problem_number]);
                            send(reads.fd_array[i], _buffer, BUF_SIZE, 0);
                            break;

                        case ANSWER_PROBLEM:
                            // 문제 번호 수신
                            init_buffer();
                            recv(reads.fd_array[i], _buffer, BUF_SIZE, 0);
                            problem_number = atoi(_buffer);

                            init_buffer();
                            recv(reads.fd_array[i], _buffer, BUF_SIZE, 0);
                            int check = check_answer(problem_number, _buffer);

                            init_buffer();
                            if (check) {
                                strcat(_buffer, "맞았습니다!");
                            } else {
                                strcat(_buffer, "틀렸습니다... 정답은 \"");
                                strcat(_buffer, correct_answers[problem_number]);
                                strcat(_buffer, "\" 입니다.");
                            }
                            send(reads.fd_array[i], _buffer, BUF_SIZE, 0);
                            break;

                        default:
                            break;
                        }
                    }
                }
            }
        }
    }

    closesocket(server_socket);
    WSACleanup();
    return 0;
}

size_t get_problem_number() {
    size_t num = rand() % PROBLEM_COUNT;
    return num;
}

int check_answer(size_t problem_number, const char* answer) {
    int compare = strcmp(correct_answers[problem_number], answer);
    return compare == 0 ? 1 : 0;
}

void init_buffer() {
    memset(_buffer, '\0', BUF_SIZE);
}

void error_handling(const char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}