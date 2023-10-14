/*
============================================================================
Name : server.c
Author : Anurag Babal
Description : Write a program to communicate between two machines using socket.
Date: 05th Oct, 2023.
============================================================================
*/

#include "headers.h"
#include "config.h"

int login(int client_socket, int type) {
    char *str;
    int n, status;
    char recv_buff[20], login_id[50], password[50];
    str = "Enter user-name: ";
    write(client_socket, str, strlen(str));

    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(login_id, recv_buff);

    str = "Enter password: ";
    write(client_socket, str, strlen(str));
    memset(recv_buff, 0, sizeof(recv_buff));

    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(password, recv_buff);

    int fd;
    struct login st;
    switch(type) {
        case ADMIN:
            if((strcmp(login_id, "admin") == 0) && (strcmp(password, "pass") == 0))
                status = 1;
            break;
        case FACULTY:
        case STUDENT:
            openLoginFile(&fd, O_RDONLY);
            while(1) {
                n = read(fd, &st, sizeof(struct login));
                if(n < 0) {
                    perror("read");
                    break;
                }
                if(n == 0) break;
                if(checkLoginDetails(login_id, password, type)) {
                    status = 1;
                    break;
                }
            }
    }

    if(status)
        str = "login successful\n";
    else
        str = "login not successful\n";
    write(client_socket, str, strlen(str));
    return status;
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
                addFaculty(client_socket);
                break;
            case 4:
                viewFaculty(client_socket);
                break;
            case 5:
                modifyStudent(client_socket, 2);
                break;
            case 6:
                modifyStudent(client_socket, 1);
                break;
            case 7:
                modifyStudent(client_socket, 3);
                break;
            case 8:
                modifyFaculty(client_socket);
                break;
            case 9:
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

void facultyMenu(int client_socket) {
    char recv_buff[20];
    int n, status = 0;
    char *str;
    char welcome_msg[250] = "...Welcome to Faculty Menu...\n";
    char *menu = "1. View Offering Courses\n2. Add new course\n3. Remove course\n4. Update Course\n5. Change password\n\
6. Logout\nEnter your choice: ";
    strcat(welcome_msg, menu);
    // n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    while(!status) {
        send(client_socket, welcome_msg, strlen(welcome_msg), MSG_DONTWAIT);
        read_line(client_socket, recv_buff, sizeof(recv_buff));
        switch(atoi(recv_buff)) {
            case 1:
                viewAllCoursesForFaculty(client_socket);
                break;
            case 2:
                addCourse(client_socket);
                break;
            case 3:
                removeCourse(client_socket);
                break;
            case 4:
                modifyCourse(client_socket);
                break;
            case 5:
                changePassword(client_socket, FACULTY);
                break;
            case 6:
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

void studentMenu(int client_socket) {
    char recv_buff[20];
    int n, status = 0;
    char *str;
    char welcome_msg[250] = "...Welcome to Student Menu...\n";
    char *menu = "1. View All Courses\n2. Enroll new course\n3. Drop course\n4. View enrolled course details\n5. Change password\n\
6. Logout\nEnter your choice: ";
    strcat(welcome_msg, menu);
    while(!status) {
        send(client_socket, welcome_msg, strlen(welcome_msg), MSG_DONTWAIT);
        read_line(client_socket, recv_buff, sizeof(recv_buff));
        switch(atoi(recv_buff)) {
            case 1:
                viewAllCourses(client_socket);
                break;
            case 2:
                registerStudent(client_socket);
                break;
            case 3:
                dropCourse(client_socket);
                break;
            case 4:
                viewEnrolledCourses(client_socket);
                break;
            case 5:
                changePassword(client_socket, STUDENT);
                break;
            case 6:
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
    while(!login(client_socket, ADMIN));
    adminMenu(client_socket);
}

void handleFaculty(int client_socket) {
    while(!login(client_socket, FACULTY));
    facultyMenu(client_socket);
}

void handleStudent(int client_socket) {
    // while(!login(client_socket, STUDENT));
    studentMenu(client_socket);
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