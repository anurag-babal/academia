#ifndef COURSE_H
#define COURSE_H

struct course {
    int id;
    int faculty_id;
    char name[50];
    char department[50];
    int total_seats;
    int status;
    int available_seats;
    int credits;
};

void openCourseFile(int *fd, int flag);
void updateSeat(int course_id, int action);
int findCourseById(int fd, struct course *st, int id);
void displayCourseDetail(int client_socket, struct course *st);
int getCourseIdFromClient(int client_socket);

#endif