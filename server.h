#include<unistd.h>
#include<stdio.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<fcntl.h>

#include "config.h"
#include "read_line.h"

struct course {
    int id;
    char course_name[20];
    char department[20];
    int seats;
    int credits;
};

struct student {
    int id;
    char name[50];
    int age;
    char email[50];
    char address[50];
    char roll_no[10];
    char password[20];
    int status;
};