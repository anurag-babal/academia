#ifndef LOGIN_H
#define LOGIN_H

struct login {
    int id;
    char login_id[50];
    char password[50];
    int type;
};

void previousNLogin(int fd, int n, int whence) {
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
            previousNLogin(fd, 1, SEEK_CUR);
            break;
    }
    write(fd, &st, sizeof(struct login));
}

int checkLoginDetails(char *login_id, char *password, int type) {
    int n, fd, status = 0;
    struct login st;
    openLoginFile(&fd, O_RDONLY);
    status = findLoginDetails(fd, &st, login_id, password, type);
    if(status) {
        switch(type) {
            case FACULTY:
                faculty_id = st.id;
                strcpy(faculty_login_id, st.login_id);
                break;
            case STUDENT:
                strcpy(student_login_id, st.login_id);
                break;
        }
    }
    close(fd);
    return status;
}

void changePassword(int client_socket, int type) {
    char *str, *login_id;
    char recv_buff[50], password[50];
    struct login st;
    int fd;
    openLoginFile(&fd, O_RDWR);

    switch(type) {
        case FACULTY:
            login_id = faculty_login_id;
            break;
        case STUDENT:
            login_id = student_login_id;
            break;
    }

    memset(buff, 0, sizeof(recv_buff));
    str = "Old Password: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(password, recv_buff);

    if(findLoginDetails(fd, &st, login_id, password, type)) {
        str = "New Password: ";
        write(client_socket, str, strlen(str));
        read_line(client_socket, recv_buff, sizeof(recv_buff));
        my_strcpy(st.password, recv_buff);
        previousNLogin(fd, 1, SEEK_CUR);
        write(fd, &st, sizeof(struct login));
    } else {
        str = "Password incorrect";
        write(client_socket, str, strlen(str));
        read_line(client_socket, recv_buff, sizeof(recv_buff));
    }

    close(fd);
}

#endif