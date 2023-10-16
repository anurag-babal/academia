#include "../headers/headers.h"
#include "../headers/config.h"
#include "../headers/read_line.h"

void my_strcpy(char *dest, char *src) {
    while (*src != '\n') {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

void my_itoa(int num, char *str, int base) {
    int i = 0;
    while (num > 0) {
        int digit = num % base;
        str[i++] = digit + '0';
        num = num / base;
    }
    str[i] = '\0';

    // Reverse the string.
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

int getIdFromClient(int client_socket, char *str) {
    char recv_buff[20];
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    return atoi(recv_buff);
}