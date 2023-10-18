#ifndef STUDENT_H
#define STUDENT_H

#include "constant.h"

struct student {
    int id;
    char name[50];
    int age;
    char email[50];
    char address[50];
    char roll_no[10];
    int status;
    int enrolled_courses[MAX_COURSE];
};

void viewAllCourses(int);
void registerStudent(int, int);
void dropCourse(int, int);
void viewEnrolledCourses(int, int);

int addStudent(int client_socket);
void viewStudent(int client_socket);
int modifyStudent(int client_socket, int action);
void removeEnrolledCourse(int student_id, int course_id);

#endif