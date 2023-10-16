#ifndef FACULTY_H
#define FACULTY_H

struct faculty {
    int id;
    char name[50];
    char department[50];
    char designation[50];
    char email[50];
    char address[50];
    char login_id[50];
};

void viewAllCoursesForFaculty(int, int);
int addCourse(int, int);
void removeCourse(int);
int modifyCourse(int);

int addFaculty(int client_socket);
void viewFaculty(int client_socket);
int modifyFaculty(int client_socket);

#endif