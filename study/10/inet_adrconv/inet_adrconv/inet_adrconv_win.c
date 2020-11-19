#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include <WinSock2.h>
#include <stdio.h>
void error_handling(const char* message);

void inet_addr_test();
void inet_ntoa_test();

int main(int argc, const char** argv) {
    WSADATA wsa_data;
    if (WSAStartup(MAKEWORD(2, 2), &wsa_data) != 0) {
        error_handling("WSAStartup() error!");
    }

    inet_addr_test();
    inet_ntoa_test();

    WSACleanup();
    return 0;
}

void error_handling(const char* message) {
    fputs(message, stderr);
    fputc('\n', stderr);
    exit(1);
}

void inet_addr_test() {
    const char* addr = "127.212.124.78";
    unsigned long conv_addr = inet_addr(addr);
    if (conv_addr == INADDR_NONE) {
        printf("Error occured!\n");
    } else {
        printf("Network ordered integer addr: %#lx\n", conv_addr);
    }
}

void inet_ntoa_test() {
    struct sockaddr_in addr;
    char* str_ptr;
    char str_arr[20];

    addr.sin_addr.s_addr = htonl(0x1020304);
    str_ptr = inet_ntoa(addr.sin_addr);
    strcpy(str_arr, str_ptr);
    printf("Dotted-Decimal notation3 %s\n", str_arr);
}
