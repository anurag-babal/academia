#include "../headers/headers.h"
#include "../headers/config.h"
#include "../headers/constant.h"
#include "../headers/login.h"
#include "../headers/read_line.h"

void previousNLoginRecord(int fd, int n, int whence) {
    lseek(fd, -(n * (sizeof(struct login))), whence);
}

void openLoginFile(int *fd, int flag) {
    *fd = open("login_details", flag | O_CREAT, 0600);
    if(*fd == -1) {
        perror("open");
    }
}

int findLoginDetails(int fd, struct login *st, char *login_id, char *password, int type) {
    int status = 0, n;
    lockFile(fd, READ);
    while(1) {
        n = read(fd, st, sizeof(struct login));
        if(n < 0) {
            perror("read");
            break;
        }
        if(n == 0) break;
        if((strcmp(st->login_id, login_id) == 0) && (strcmp(st->password, password) == 0) && (st->type == type)) {
            status = 1;
            break;
        }
    }
    lockFile(fd, UNLOCK);
    return status;
}

void saveLoginDetails(int id, char *login_id, char *password, int type, int operation) {
    int fd;
    openLoginFile(&fd, O_RDWR);

    struct login st;
    st.id = id;
    st.type = type;
    strcpy(st.login_id, login_id);
    strcpy(st.password, password);

    switch(operation) {
        case ADD:
            lseek(fd, 0, SEEK_END);
            break;
        case UPDATE:
            findLoginDetails(fd, &st, login_id, password, type);
            previousNLoginRecord(fd, 1, SEEK_CUR);
            break;
    }
    lockFile(fd, WRITE);
    write(fd, &st, sizeof(struct login));
    lockFile(fd, UNLOCK);
}

int checkLoginDetails(char *login_id, char *password, int type) {
    int faculty_id;
    int n, fd, status = 0;
    struct login st;
    openLoginFile(&fd, O_RDONLY);
    status = findLoginDetails(fd, &st, login_id, password, type);
    close(fd);
    return status;
}

void changePassword(int client_socket, int type, char *login_id) {
    char *str;
    char recv_buff[50], password[50];
    struct login st;
    int fd;
    openLoginFile(&fd, O_RDWR);

    str = "Old Password: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(password, recv_buff);

    if(findLoginDetails(fd, &st, login_id, password, type)) {
        str = "New Password: ";
        write(client_socket, str, strlen(str));
        read_line(client_socket, recv_buff, sizeof(recv_buff));
        my_strcpy(st.password, recv_buff);
        previousNLoginRecord(fd, 1, SEEK_CUR);
        write(fd, &st, sizeof(struct login));
        str = "==========Password changed==========\n";
    } else {
        str = "==========Password incorrect==========\n";
    }
    send(client_socket, str, strlen(str), MSG_MORE);

    close(fd);
}