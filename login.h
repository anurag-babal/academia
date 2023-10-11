#ifndef LOGIN_H
#define LOGIN_H

struct login {
    int id;
    char login_id[50];
    char password[50];
};

void previousNLogin(int fd, int n, int whence) {
    lseek(fd, -(n * (sizeof(struct login))), whence);
}

void openLoginFile(int *fd, int flag) {
    *fd = open("login_details", flag);
    if(*fd == -1) {
        perror("open");
    }
}

int findLoginDetials(int fd, struct login *st, char *login_id, char *password) {
    int status = 0, n;
    while(1) {
        n = read(fd, st, sizeof(struct login));
        if(n < 0) {
            perror("read");
            break;
        }
        if(n == 0) break;
        if((strcmp(st->login_id, login_id) == 0) && (strcmp(st->password, password) == 0)) {
            status = 1;
            break;
        }
    }
    return status;
}
// int saveLoginDetails(int, char *, char *, int);
void saveLoginDetails(int id, char *login_id, char *password, int operation) {
    int fd;
    openLoginFile(&fd, O_RDWR);

    struct login st;
    st.id = id;
    strcpy(st.login_id, login_id);
    strcpy(st.password, password);

    switch(operation) {
        case ADD:
            lseek(fd, 0, SEEK_END);
            break;
        case UPDATE:
            findLoginDetials(fd, &st, login_id, password);
            previousNLogin(fd, 1, SEEK_CUR);
            break;
    }
    write(fd, &st, sizeof(struct login));
}

int checkLoginDetails(char *login_id, char *password) {
    int n, fd, status = 0;
    struct login st;
    openLoginFile(&fd, O_RDONLY);
    status = findLoginDetials(fd, &st, login_id, password);
    if(status) faculty_id = st.id;
    close(fd);
    return status;
}

#endif