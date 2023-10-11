#ifndef FACULTY_H
#define FACULTY_H

#include "login.h"
struct faculty {
    int id;
    char name[50];
    char department[50];
    char designation[50];
    char email[50];
    char address[50];
    char login_id[50];
};

int getFaculty(int client_socket, struct faculty *ft, int fd) {
    char recv_buff[20], login_id[20];
    int n, status = 0, id;
    char *str;

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Enter faculty id: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(login_id, recv_buff);
    id = atoi(recv_buff);
    while(1) {
        n = read(fd, ft, sizeof(struct faculty));
        if(n < 0) {
            perror("read");
            break;
        }
        if(n == 0) break;
        if(ft->id == id) {
            status = 1;
            break;
        }
    }
    return status;
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
    write(fd, st, sizeof(struct faculty));
}

void openFacultyFile(int *fd, int flag) {
    *fd = open("faculty_details", flag);
    if(*fd == -1) {
        perror("open");
    }
}

void getFacultyDetails(int client_socket, struct faculty *st) {
    char *str;
    char recv_buff[50];

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

    getFacultyDetails(client_socket, &st);
    if(read(fd, buff, 1) == 0) {
        st.id = 1;
    } else {
        previousNStudent(fd, 1, SEEK_END);
        read(fd, &tmp, sizeof(tmp));
        st.id = tmp.id + 1;
    }

    char abc[20];
    my_itoa(st.id, abc, 10);
    strcat(login_id, abc);
    strcpy(st.login_id, login_id);
    saveLoginDetails(st.id, st.login_id, default_password, ADD);

    writeFaculty(fd, &st, ADD);
    close(fd);

    str = "Student Created Successfully\nLogin-id -> ";
    send(client_socket, str, strlen(str), MSG_DONTWAIT);
    send(client_socket, st.login_id, strlen(st.login_id), MSG_DONTWAIT);
    str = "\nFaculty Id -> ";
    send(client_socket, str, strlen(str), MSG_DONTWAIT);
    memset(buff, 0, sizeof(buff));
    my_itoa(st.id, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_DONTWAIT);
    write(client_socket, "\n", 1);
}

void viewFaculty(int client_socket) {
    struct faculty *st;
    char *str;
    int fd;
    openFacultyFile(&fd, O_RDONLY);

    if(getFaculty(client_socket, st, fd)) {
        str = "Name: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st->name, strlen(st->name), MSG_DONTWAIT);

        str = "\nDepartment: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st->department, strlen(st->department), MSG_DONTWAIT);

        str = "\nDesignation: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st->designation, strlen(st->designation), MSG_DONTWAIT);

        str = "\nEmail: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st->email, strlen(st->email), MSG_DONTWAIT);

        str = "\nAddress: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st->address, strlen(st->address), MSG_DONTWAIT);

        str = "\nLogin-id: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st->login_id, strlen(st->login_id), MSG_DONTWAIT);

        write(client_socket, "\n", 1);
    } else {
        str = "Faculty not found\n";
        write(client_socket, str, strlen(str));
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
        str = "Faculty not found\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}

#endif