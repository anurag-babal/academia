#ifndef CONFIG_H
#define CONFIG_H

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define ADD 2
#define UPDATE 3
#define ACTIVE 4
#define INACTIVE 5
#define ADMIN 6
#define FACULTY 7
#define STUDENT 8

int faculty_id = 0;
char buff[1024];
char *default_password = "pass";

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

#endif