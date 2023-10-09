/*
============================================================================
Name : server.c
Author : Anurag Babal
Description : Write a program to communicate between two machines using socket.
Date: 05th Oct, 2023.
============================================================================
*/

#include "server.h"

void login(int client_socket, char *user_name, char *password) {
    char *str;
    int n;
    char recv_buff[20];
    str = "Enter user-name: ";
    send(client_socket, str, strlen(str), 0);

    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    write(STDOUT_FILENO, recv_buff, n);

    str = "Enter password: ";
    send(client_socket, str, strlen(str), 0);
    memset(recv_buff, 0, sizeof(recv_buff));

    n = read_line(client_socket, recv_buff, sizeof(recv_buff));
    write(STDOUT_FILENO, recv_buff, n);

    if(strncmp(password, recv_buff, n-1) == 0)
        str = "login successful";
    else 
        str = "login not successful";
    send(client_socket, str, strlen(str), 0);
}

void handleAdmin(int client_socket) {
    login(client_socket, "admin", "password");
}

void handleStudent(int client_socket) {
    login(client_socket, "anurag", "password");
}

void handleProfessor(int client_socket) {
    login(client_socket, "abc", "pass");
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

            // read(client_socket, buff, sizeof(buff));
            // printf("Message from client: %s\n", buff);
            char welcome_msg[] = "Welcome to Academia :: Course Registration\nLogin Type\
                1.Admin\
                2.Professor\
                3.Student\nEnter your choice: ";

            char recv_buff[20], send_buff[1024];
            int n, bytes_to_send;
            char* move_ptr;
            int status = 0;
            char *str;

            while(1) {
                while(!status) {
                    send(client_socket, welcome_msg, strlen(welcome_msg), 0);
                    memset(recv_buff, 0, sizeof(recv_buff));
                    n = recv(client_socket, recv_buff, sizeof(recv_buff), 0);
                    switch (atoi(recv_buff)) {
                        case 1:
                            handleAdmin(client_socket);
                            status = 1;
                            break;
                        case 2:
                            handleProfessor(client_socket);
                            status = 1;
                            break;
                        case 3:
                            handleStudent(client_socket);
                            status = 1;
                            break;
                        default:
                            str = "Enter correct choice\n";
                            send(client_socket, str, strlen(str), 0);
                    }
                }
                memset(send_buff, 0, sizeof(send_buff));
                n = read(STDIN_FILENO, send_buff, sizeof(send_buff));
                send_buff[n] = '\0';
                // scanf("%s", send_buff);
                move_ptr = send_buff;
                bytes_to_send = strlen(send_buff);
                // while ((n = send(client_socket, move_ptr, bytes_to_send, 0)) > 0) {
                //     move_ptr = move_ptr + n;
                //     bytes_to_send = bytes_to_send - n;
                // }
                send(client_socket, send_buff, bytes_to_send, 0);
            }
            exit(0);
        }
    }

    return 0;
}