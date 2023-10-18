#include "../headers/headers.h"
#include "../headers/config.h"
#include "../headers/faculty.h"
#include "../headers/constant.h"
#include "../headers/read_line.h"
#include "../headers/student.h"
#include "../headers/login.h"

int readFacultyRecordFromFile(int fd, struct faculty *st) {
    int status = 1;
    lockFile(fd, READ);
    int n = read(fd, st, sizeof(struct faculty));
    if(n < 0) {
        perror("read");
        status = -1;
    }
    if(n == 0) status = 0;
    lockFile(fd, UNLOCK);
    return status;
}

int findFacultyById(int fd, struct faculty *st, int id) {
    int status = 0;
    while(readFacultyRecordFromFile(fd, st) > 0) {
        if(st->id == id) {
            status = 1;
            break;
        }
    }
    return status;
}

int getFaculty(int client_socket, struct faculty *ft, int fd) {
    int id = getIdFromClient(client_socket, "Enter Faculty Id: ");
    return findFacultyById(fd, ft, id);
}

void previousNFaculty(int fd, int n, int whence) {
    lseek(fd, -(n * (sizeof(struct faculty))), whence);
}

int writeFaculty(int fd, struct faculty *st, int operation) {
    switch(operation) {
        case ADD:
            lseek(fd, 0, SEEK_END);
            break;
        case UPDATE:
            previousNFaculty(fd, 1, SEEK_CUR);
            break;
    }
    lockFile(fd, WRITE);
    write(fd, st, sizeof(struct faculty));
    lockFile(fd, UNLOCK);
}

void openFacultyFile(int *fd, int flag) {
    *fd = open("faculty_details", flag | O_CREAT, 0600);
    if(*fd == -1) {
        perror("open");
    }
}

void displayFacultyDetails(int client_socket, struct faculty *st) {
    char *str;
    str = "\n====================\n";
    send(client_socket, str, strlen(str), MSG_MORE);
    str = "Name: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->name, strlen(st->name), MSG_MORE);

    str = "\nDepartment: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->department, strlen(st->department), MSG_MORE);

    str = "\nDesignation: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->designation, strlen(st->designation), MSG_MORE);

    str = "\nEmail: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->email, strlen(st->email), MSG_MORE);

    str = "\nAddress: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->address, strlen(st->address), MSG_MORE);

    str = "\nLogin-id: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->login_id, strlen(st->login_id), MSG_MORE);

    str = "\n====================\n";
    send(client_socket, str, strlen(str), MSG_MORE);
}

void getFacultyDetails(int client_socket, struct faculty *st) {
    char *str;
    char recv_buff[50], buff[50];

    memset(buff, 0, sizeof(recv_buff));
    str = "Name: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->name, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Department: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->department, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Designation: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->designation, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Email: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->email, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Address: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->address, recv_buff);
}

int addFaculty(int client_socket) {
    struct faculty st, tmp;
    int fd;
    openFacultyFile(&fd, O_RDWR);
    char login_id[10] = "iiitb";
    int n, status = 0;
    char *str;
    char buff[50];

    getFacultyDetails(client_socket, &st);
    lockFile(fd, READ);
    if(read(fd, buff, 1) <= 0) {
        st.id = 1;
    } else {
        lseek(fd, -sizeof(struct faculty), SEEK_END);
        read(fd, &tmp, sizeof(struct faculty));
        st.id = tmp.id + 1;
    }
    lockFile(fd, UNLOCK);

    printf("%d\n", st.id);
    my_itoa(st.id, buff, 10);
    strcat(login_id, buff);
    my_strcpy(st.login_id, login_id);
    saveLoginDetails(st.id, st.login_id, DEFAULT_PASS, FACULTY, ADD);

    writeFaculty(fd, &st, ADD);
    close(fd);

    str = "Faculty Created Successfully\nLogin-id -> ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st.login_id, strlen(st.login_id), MSG_MORE);
    str = "\nFaculty Id -> ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, buff, strlen(buff), MSG_MORE);
    memset(buff, 0, sizeof(buff));
    str = "\n====================\n";
    send(client_socket, str, strlen(str), MSG_MORE);
}

void viewFaculty(int client_socket) {
    struct faculty st;
    char *str;
    int fd;
    openFacultyFile(&fd, O_RDONLY);
    if(getFaculty(client_socket, &st, fd)) {
        displayFacultyDetails(client_socket, &st);
    } else {
        str = "==========Faculty not found==========\n";
        send(client_socket, str, strlen(str), MSG_MORE);
    }
    close(fd);
}

int modifyFaculty(int client_socket) {
    struct faculty st;
    char *str;
    int fd;
    openFacultyFile(&fd, O_RDWR);
    if(getFaculty(client_socket, &st, fd)) {
        getFacultyDetails(client_socket, &st);
        writeFaculty(fd, &st, UPDATE);
    } else {
        str = "==========Faculty not found==========\n";
        send(client_socket, str, strlen(str), MSG_MORE);
    }
    close(fd);
}