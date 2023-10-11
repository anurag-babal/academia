#include "config.h"

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
        str[i] = digit + '0';
        i++;
        num = num / base;
    }
    str[i] = '\0';
}