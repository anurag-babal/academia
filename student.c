#include "student.h"
#include "config.h"
#include "headers.h"

// char buff[1024];
int addStudent(int client_socket) {
    struct faculty st, tmp;
    int fd = open("student_details", O_RDWR);
    char recv_buff[20], roll_no[10] = "MT";
    int n, status = 0;
    char *str;

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Name: ";
    write(client_socket, str, strlen(str));
    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st.name, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Age: ";
    write(client_socket, str, strlen(str));
    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    st.age = atoi(recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Email: ";
    write(client_socket, str, strlen(str));
    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st.email, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Address: ";
    write(client_socket, str, strlen(str));
    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st.address, recv_buff);

    if(read(fd, buff, 1) == 0) {
        st.id = 1;
    } else {
        lseek(fd, -sizeof(st), SEEK_END);
        read(fd, &tmp, sizeof(tmp));
        st.id = tmp.id + 1;
    }
    st.status = 0;
    char abc[20];
    my_itoa(st.id, abc, 10);
    strcat(roll_no, abc);
    strcpy(st.roll_no, roll_no);
    strcpy(st.password, "password");

    lseek(fd, 0, SEEK_END);
    write(fd, &st, sizeof(struct faculty));
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

int viewStudent(int client_socket) {
    struct faculty st;
    int fd = open("student_details", O_RDWR);
    char recv_buff[20], roll_no[10];
    int n, status = 0;
    char *str;
    
    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Roll Number: ";
    write(client_socket, str, strlen(str));
    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(roll_no, recv_buff);
    while(1) {
        n = read(fd, &st, sizeof(struct faculty));
        if(n < 0) {
            perror("read");
            _exit(EXIT_FAILURE);
        }
        if(n == 0) break;
        if(strcmp(st.roll_no, roll_no) == 0) {
            str = "Name: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            send(client_socket, st.name, strlen(st.name), MSG_DONTWAIT);

            str = "\nAge: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            my_itoa(st.age, buff, 10);
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

            status = 1;
            break;
        }
    }
    if(!status) {
        str = "Student not found\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}