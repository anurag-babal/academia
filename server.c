/*
============================================================================
Name : server.c
Author : Anurag Babal
Description : Write a program to communicate between two machines using socket.
Date: 05th Oct, 2023.
============================================================================
*/

#include "server.h"

int main(void)
{
    struct sockaddr_in server_address, client_address;

    int listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket < 0)
    {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("Listen socket created successfully\n");

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUMBER);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    int bind_status = bind(listen_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (bind_status < 0)
    {
        perror("bind");
        exit(EXIT_FAILURE);
    }
    printf("Binding to socket is successful\n");

    int listen_status = listen(listen_socket, 5);
    if (listen_status < 0)
    {
        printf("Error listening for incoming connections\n");
        exit(EXIT_FAILURE);
    }
    printf("Server listening on port : %d...\n", PORT_NUMBER);

    int client_addr_len;
    int client_socket;
    while (1)
    {
        client_socket = accept(listen_socket, (struct sockaddr *)&client_address, &client_addr_len);
        if (client_socket < 0) {
            printf("Error accepting incoming connection\n");
            exit(EXIT_FAILURE);
        }
        printf("Client connected\n");
        if (!fork()) {
            close(listen_socket);
            char client_ip[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, &client_address.sin_addr, client_ip, sizeof(client_ip));
            // printf("Client details: Ip Address: %s", client_ip);
            write(STDOUT_FILENO, client_ip, strlen(client_ip));

            // read(client_socket, buff, sizeof(buff));
            // printf("Message from client: %s\n", buff);
            // char welcome_msg[] = "Welcome to Academia :: Course Registration\nLogin Type\
            //     1.Admin\
            //     2.Professor\
            //     3.Student\nEnter your choice: ";
            // send(client_socket, welcome_msg, sizeof(welcome_msg), 0);
            int n, bytes_to_recv = 256;
            char buff[256];
            char* move_ptr;
            move_ptr = buff;
            while ((n = recv(client_socket, move_ptr, bytes_to_recv, 0)) > 0) {
                move_ptr = move_ptr + n;
                bytes_to_recv = bytes_to_recv - n;
            }
            send(client_socket, buff, sizeof(buff), 0);
            exit(0);
            //     printf("%s\n", buff);
            //     switch (buff[0])
            //     {
            //     case '1':
            //         printf("Admin");
            //         break;
            //     case '2':
            //         printf("Faculty");
            //         break;
            //     case '3':
            //         printf("Student");
            //         break;
            //     default:
            //         printf("Enter correct choice");
            //     }
            //     scanf("%s", buff);
            //     send(client_socket, buff, sizeof(buff), 0);
            // }
        }
        close(client_socket);
    }

    return 0;
}