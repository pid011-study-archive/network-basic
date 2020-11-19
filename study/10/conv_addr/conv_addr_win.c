#undef UNICODE
#undef _UNICODE

#include <stdio.h>
#include <winsock2.h>

int main(int argc, char** argv) {
    const char* str_addr = "203.211.218.102:9190";

    char str_addr_buf[50];
    SOCKADDR_IN serv_addr;
    int size;

    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);

    size = sizeof(serv_addr);
    WSAStringToAddress(str_addr, AF_INET, NULL, (SOCKADDR*)&serv_addr, &size);
    size = sizeof(str_addr_buf);
    WSAAddressToString((SOCKADDR*)&serv_addr, sizeof(serv_addr), NULL, str_addr_buf, &size);
    printf("Second conv result: %s\n", str_addr_buf);
    WSACleanup();
    return 0;
}