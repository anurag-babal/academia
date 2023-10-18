/*
============================================================================
Name : 33b.c
Author : Anurag Babal
Description : Write a program to communicate between two machines using socket.
Date: 20th Sept, 2023.
============================================================================
*/

#include "../headers/headers.h"
#include "../headers/read_line.h"

int main(int argc, char **argv)
{
    if(argc != 3) {
        printf("client <ip-address> <port-number>\n");
        exit(EXIT_FAILURE);
    }
    char *ip_address = argv[1];
    int port_number = atoi(argv[2]);
    struct sockaddr_in server_address;
    int sd = socket(AF_INET, SOCK_STREAM, 0);
    if (sd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }
    printf("Client socket created successfully\n");

    memset (&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port_number);
    server_address.sin_addr.s_addr = inet_addr(ip_address);
    if(connect(sd, (struct sockaddr*) &server_address, sizeof(server_address)) < 0) {
        perror("connect");
        exit(EXIT_FAILURE);
    }
    printf("Client to server connection successfully established\n");

    char send_buff[50], recv_buff[1024];
    memset(recv_buff, 0, sizeof(recv_buff));
    while(read(sd, recv_buff, sizeof(recv_buff)) > 0) {    
        write(STDOUT_FILENO, recv_buff, strlen(recv_buff));

        memset(send_buff, 0, sizeof(send_buff));
        read_line(STDIN_FILENO, send_buff, sizeof(send_buff));
        write(sd, send_buff, strlen(send_buff));
        
        memset(recv_buff, 0, sizeof(recv_buff));
    }

    // Close the socket
    close(sd);

    return 0;
}