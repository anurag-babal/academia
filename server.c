/*
============================================================================
Name : server.c
Author : Anurag Babal
Description : Write a program to communicate between two machines using socket.
Date: 05th Oct, 2023.
============================================================================
*/

#include "server.h"

char buff[1024];
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

int login(int client_socket, char *user_name, char *password) {
    char *str;
    int n, status;
    char recv_buff[20], userName[20];
    str = "Enter user-name: ";
    write(client_socket, str, strlen(str));

    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(userName, recv_buff);

    str = "Enter password: ";
    write(client_socket, str, strlen(str));
    memset(recv_buff, 0, sizeof(recv_buff));

    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(userName, recv_buff);

    if(strcmp(password, userName) == 0) {
        str = "login successful\n";
        status = 1;
    } else {
        str = "login not successful\n";
        status = 0;
    }
    write(client_socket, str, strlen(str));
    return status;
}

int addStudent(int client_socket) {
    struct student st, tmp;
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
    write(fd, &st, sizeof(struct student));
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
    struct student st;
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
        n = read(fd, &st, sizeof(struct student));
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

void adminMenu(int client_socket) {
    char recv_buff[20];
    int n, status = 0;
    char *str;
    char welcome_msg[250] = "...Welcome to Admin Menu...\n";
    char *menu = "1. Add Student\n2. View Student Details\n3. Add Faculty\n4. View Faculty Details\n5. Activate Student\n\
6. Block Student\n7. Update Student Details\n8. Update Faculty Details\n9. Logout\nEnter your choice: ";
    strcat(welcome_msg, menu);
    // n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    while(!status) {
        send(client_socket, welcome_msg, strlen(welcome_msg), MSG_DONTWAIT);
        read_line(client_socket, recv_buff, sizeof(recv_buff));
        switch(atoi(recv_buff)) {
            case 1:
                addStudent(client_socket);
                break;
            case 2:
                viewStudent(client_socket);
                break;
            case 3:
                printf("inside 3\n");
                addFaculty(client_socket);
                break;
            case 4:
                printf("inside 4\n");
                break;
            case 5:
                str = "Logged out\nPress Enter to close the connection";
                write(client_socket, str, strlen(str));
                status = 1;
                break;
            default:
                str = "Enter correct choice\n";
                write(client_socket, str, strlen(str));
        }
    }
}

void handleAdmin(int client_socket) {
    while(!login(client_socket, "admin", "password"));
    adminMenu(client_socket);
}

void handleFaculty(int client_socket) {
    while(!login(client_socket, "abc", "pass"));
}

void handleStudent(int client_socket) {
    while(!login(client_socket, "anurag", "password"));
}

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("server <ip-address> <port-number>\n");
        exit(EXIT_FAILURE);
    }

    char *ip_address = argv[1];
    int port_number = atoi(argv[2]);
    struct sockaddr_in server_address, client_address;
    int listening_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listening_socket < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("Listen socket created successfully\n");

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    server_address.sin_addr.s_addr = inet_addr(ip_address);
    if (bind(listening_socket, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("Binding to socket is successful\n");

    if (listen(listening_socket, 5) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port : %d...\n", port_number);

    int client_addr_len;
    int client_socket;
    int pid;
    while (1)
    {
        client_socket = accept(listening_socket, (struct sockaddr *)&client_address, &client_addr_len);
        if (client_socket < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }
        printf("Client connected\n");

        if (fork()) {
            close(client_socket);
        } else {
            close(listening_socket);
            // char client_ip[INET_ADDRSTRLEN];
            // inet_ntop(AF_INET, &client_address.sin_addr, client_ip, sizeof(client_ip));
            // printf("Client details: Ip Address: %s", client_ip);
            // write(STDOUT_FILENO, client_ip, strlen(client_ip));
            char welcome_msg[] = "Welcome to Academia :: Course Registration\nLogin Type\
                1.Admin\
                2.Professor\
                3.Student\nEnter your choice: ";

            char recv_buff[20];
            int status = 0;
            char *str;

            while(!status) {
                write(client_socket, welcome_msg, strlen(welcome_msg));
                memset(recv_buff, 0, sizeof(recv_buff));
                read_line(client_socket, recv_buff, sizeof(recv_buff));
                switch (atoi(recv_buff)) {
                    case 1:
                        handleAdmin(client_socket);
                        status = 1;
                        break;
                    case 2:
                        handleFaculty(client_socket);
                        status = 1;
                        break;
                    case 3:
                        handleStudent(client_socket);
                        status = 1;
                        break;
                    default:
                        str = "Enter correct choice\n";
                        write(client_socket, str, strlen(str));
                }
            }
            close(client_socket);
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}