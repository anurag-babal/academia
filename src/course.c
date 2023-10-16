#include "../headers/headers.h"
#include "../headers/config.h"
#include "../headers/constant.h"
#include "../headers/course.h"
#include "../headers/read_line.h"
#include "../headers/registration.h"

int readCourseRecordFromFile(int fd, struct course *st) {
    int status = 0;
    int n = read(fd, st, sizeof(struct course));
    if(n < 0) {
        perror("read");
        return -1;
    }
    if(n == 0) return 0;
    return 1;
}

int findCourseById(int fd, struct course *st, int id) {
    int n, status = 0;
    
    while(1) {
        if(readCourseRecordFromFile(fd, st) == 1) {
            if(st->id == id && st->status == ACTIVE) {
                status = 1;
                break;
            }
        } else break;
    }
    return status;
}

int getCourseIdFromClient(int client_socket) {
    char *str;
    char recv_buff[20];

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Enter course id: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    return atoi(recv_buff);
}

void previousRecord(int fd, int whence) {
    int n = 1;
    lseek(fd, -(n * (sizeof(struct course))), whence);
}

int writeCourse(int fd, struct course *st, int operation) {
    switch(operation) {
        case ADD:
            lseek(fd, 0, SEEK_END);
            break;
        case UPDATE:
            previousRecord(fd, SEEK_CUR);
            break;
    }
    write(fd, st, sizeof(struct course));
}

void openCourseFile(int *fd, int flag) {
    *fd = open("course_details", flag | O_CREAT, 0600);
    if(*fd == -1) {
        perror("open");
    }
}

void getCourseDetails(int client_socket, struct course *st) {
    char *str;
    char recv_buff[50], buff[50];

    memset(buff, 0, sizeof(recv_buff));
    str = "Name: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->name, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Department: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->department, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "No. of seats: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    st->total_seats = atoi(recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Credits: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    st->credits = atoi(recv_buff);
}

void displayCourseDetail(int client_socket, struct course *st) {
    char *str;
    char buff[50];

    str = "Id: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    my_itoa(st->id, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_MORE);

    str = "\nName: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->name, strlen(st->name), MSG_MORE);

    str = "\nDepartment: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->department, strlen(st->department), MSG_MORE);

    str = "\nNo. of seats: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    my_itoa(st->total_seats, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_MORE);

    str = "\nCredits: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    my_itoa(st->credits, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_MORE);

    str = "\nNo. of available seats: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    my_itoa(st->available_seats, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_MORE);

    str = "\n=============\n";
    send(client_socket, str, strlen(str), MSG_MORE);
}

int addCourse(int client_socket, int faculty_id) {
    char buff[50];
    struct course st, tmp;
    int fd;
    openCourseFile(&fd, O_RDWR);
    int n, status = 0;
    char *str;

    getCourseDetails(client_socket, &st);
    if(read(fd, buff, 1) == 0) {
        st.id = 1;
    } else {
        lseek(fd, -sizeof(struct course), SEEK_END);
        read(fd, &tmp, sizeof(tmp));
        st.id = tmp.id + 1;
    }

    st.available_seats = st.total_seats;
    st.faculty_id = faculty_id;
    st.status = ACTIVE;

    writeCourse(fd, &st, ADD);
    close(fd);

    str = "Course Created Successfully\nCourse-id -> ";
    send(client_socket, str, strlen(str), MSG_MORE);
    memset(buff, 0, sizeof(buff));
    my_itoa(st.id, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_MORE);
    str = "\n=============\n";
    send(client_socket, str, strlen(str), MSG_MORE);
}

void viewAllCourses(int client_socket) {
    struct course st;
    char *str;
    char recv_buff[20];
    int n, status = 0;

    int fd;
    openCourseFile(&fd, O_RDONLY);

    while(1) {
        if(readCourseRecordFromFile(fd, &st) == 1) {
            if(st.status == ACTIVE) {
                displayCourseDetail(client_socket, &st);
                status = 1;
            }
        } else break;
    }
    if(!status) {
        str = "No course available.\n";
        send(client_socket, str, strlen(str), MSG_MORE);
    }
    close(fd);
}

void viewAllCoursesForFaculty(int client_socket, int faculty_id) {
    struct course st;
    char *str;
    char recv_buff[20];
    int n, status = 0;
    int fd;
    openCourseFile(&fd, O_RDONLY);

    while(1) {
        if(readCourseRecordFromFile(fd, &st) == 1) {
            if(st.faculty_id == faculty_id && st.status == ACTIVE) {
                displayCourseDetail(client_socket, &st);
                status = 1;
            }
        } else break;
    }
    if(!status) {
        str = "No course offered.\n";
        send(client_socket, str, strlen(str), MSG_MORE);
    }
    close(fd);
}

int modifyCourse(int client_socket) {
    char *str;
    struct course st;
    int fd;
    openCourseFile(&fd, O_RDWR);
    int id = getIdFromClient(client_socket, "Enter course id: ");
    if(findCourseById(fd, &st, id)) {
        int total_seats = st.total_seats;
        int enrolled_seats = st.total_seats - st.available_seats;
        getCourseDetails(client_socket, &st);
        if(st.total_seats < enrolled_seats) {
            removeLastNStudent(st.id, (enrolled_seats - st.total_seats));
            st.available_seats = 0;
        } else {
            st.available_seats = st.total_seats - enrolled_seats;
        }
        writeCourse(fd, &st, UPDATE);
    } else {
        str = "Course not found\n";
        send(client_socket, str, strlen(str), MSG_MORE);
    }
    close(fd);
}

void removeCourse(int client_socket) {
    char *str;
    struct course st;
    int fd;
    openCourseFile(&fd, O_RDWR);
    int id = getCourseIdFromClient(client_socket);
    if(findCourseById(fd, &st, id)) {
        st.status = INACTIVE;
        writeCourse(fd, &st, UPDATE);
    } else {
        str = "Course not found\n";
        send(client_socket, str, strlen(str), MSG_MORE);
    }
    close(fd);
}

void updateSeat(int course_id, int action) {
    int fd;
    struct course st;
    openCourseFile(&fd, O_RDWR);
    findCourseById(fd, &st, course_id);
    switch(action) {
        case INC:
            st.available_seats++;
            break;
        case DEC:
            st.available_seats--;
            break;
    }
    writeCourse(fd, &st, UPDATE);
    close(fd);
}