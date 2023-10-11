#ifndef STUDENT_H
#define STUDENT_H

#include "login.h"

struct student {
    int id;
    char name[50];
    int age;
    char email[50];
    char address[50];
    char roll_no[10];
    int status;
};

int findStudent(int client_socket, struct student *st, int fd) {
    char recv_buff[20];
    int n, status = 0, id;
    char *str;

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Enter Studetn Id: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    // my_strcpy(roll_no, recv_buff);
    id = atoi(recv_buff);
    while(1) {
        n = read(fd, st, sizeof(struct student));
        if(n < 0) {
            perror("read");
            break;
        }
        if(n == 0) break;
        if(st->id == id) {
            status = 1;
            break;
        }
    }
    return status;
}

void previousNStudent(int fd, int n, int whence) {
    lseek(fd, -(n * (sizeof(struct student))), whence);
}

void writeStudent(int fd, struct student *st, int operation) {
    switch(operation) {
        case ADD:
            lseek(fd, 0, SEEK_END);
            break;
        case UPDATE:
            previousNStudent(fd, 1, SEEK_CUR);
            break;
    }
    write(fd, st, sizeof(struct student));
}

void openStudentFile(int *fd, int flag) {
    *fd = open("student_details", flag);
    if(*fd == -1) {
        perror("open");
    }
}

void getStudentDetails(int client_socket, struct student *st) {
    char *str;
    char recv_buff[50];

    memset(buff, 0, sizeof(recv_buff));
    str = "Name: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->name, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Age: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    st->age = atoi(recv_buff);

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

int addStudent(int client_socket) {
    struct student st, tmp;
    int fd = open("student_details", O_RDWR);
    char roll_no[10] = "MT";
    int n, status = 0;
    char *str;

    getStudentDetails(client_socket, &st);
    if(read(fd, buff, 1) == 0) {
        st.id = 1;
    } else {
        previousNStudent(fd, 1, SEEK_END);
        read(fd, &tmp, sizeof(tmp));
        st.id = tmp.id + 1;
    }
    st.status = 1;

    char abc[20];
    my_itoa(st.id, abc, 10);
    strcat(roll_no, abc);
    strcpy(st.roll_no, roll_no);
    saveLoginDetails(st.id, st.roll_no, default_password, ADD);

    writeStudent(fd, &st, ADD);
    close(fd);

    str = "Student Created Successfully\nLogin-id -> ";
    send(client_socket, str, strlen(str), MSG_DONTWAIT);
    send(client_socket, st.roll_no, strlen(st.roll_no), MSG_DONTWAIT);
    str = "\nStudent Id -> ";
    send(client_socket, str, strlen(str), MSG_DONTWAIT);
    memset(buff, 0, sizeof(buff));
    my_itoa(st.id, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_DONTWAIT);
    write(client_socket, "\n", 1);
}

void viewStudent(int client_socket) {
    struct student st;
    char *str;
    int fd;
    openStudentFile(&fd, O_RDONLY);

    if(findStudent(client_socket, &st, fd)) {
        str = "Name: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            send(client_socket, st.name, strlen(st.name), MSG_DONTWAIT);

            str = "\nAge: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            printf("%d\n", st.age);
            my_itoa(st.age, buff, 10);
            printf("%s\n", buff);
            send(client_socket, buff, strlen(buff), MSG_DONTWAIT);

            str = "\nEmail: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            send(client_socket, st.email, strlen(st.email), MSG_DONTWAIT);

            str = "\nAddress: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            send(client_socket, st.address, strlen(st.address), MSG_DONTWAIT);

            str = "\nLogin-id: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            send(client_socket, st.roll_no, strlen(st.roll_no), MSG_DONTWAIT);

            write(client_socket, "\n", 1);
    } else {
        str = "Student not found\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}

int modifyStudent(int client_socket, int action) {
    struct student st;
    char *str;
    int fd;
    openStudentFile(&fd, O_RDWR);
    if(findStudent(client_socket, &st, fd)) {
        switch(action) {
            case 1:
                st.status = 0;
                break;
            case 2:
                st.status = 1;
                break;
            case 3:
                getStudentDetails(client_socket, &st);
                break;
        }
        writeStudent(fd, &st, UPDATE);
    } else {
        str = "Student not found\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}

#endif