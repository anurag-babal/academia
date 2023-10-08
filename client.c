/*
============================================================================
Name : 33b.c
Author : Anurag Babal
Description : Write a program to communicate between two machines using socket.
Date: 20th Sept, 2023.
============================================================================
*/

#include "client.h"

int main(void) {
    struct sockaddr_in server_address;
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("Client socket created successfully\n");

    memset (&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT_NUMBER);
    // server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");
    int connect_status = connect(sd, (struct sockaddr*) &server_address, sizeof(server_address));
    if (connect_status < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    printf("Client to server connection successfully established\n");

    int n, bytes_to_recv;
    char send_buff[256], recv_buff[256];
    char* move_ptr;
    scanf("%s", send_buff);
    send(sd, send_buff, strlen(send_buff), 0);
    bytes_to_recv = strlen(send_buff);
    move_ptr = recv_buff;
    while((n = recv(sd, move_ptr, bytes_to_recv, 0)) > 0) {
        move_ptr = move_ptr + n;
        bytes_to_recv = bytes_to_recv - n;
    }
    recv_buff[bytes_to_recv] = '\0';
    printf("%s", recv_buff);

    // Close the socket
    close(sd);

    return 0;
}