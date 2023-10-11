#ifndef COURSE_H
#define COURSE_H

// #include "headers.h"
// #include "config.h"

struct course {
    int id;
    int faculty_id;
    char name[50];
    char department[50];
    int seats;
    int status;
    int available_seats;
    int credits;
};

int getCourse(int client_socket, struct course *st, int fd) {
    char recv_buff[20];
    int n, status = 0, id;
    char *str;

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Enter course id: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    id = atoi(recv_buff);
    while(1) {
        n = read(fd, st, sizeof(struct course));
        if(n < 0) {
            perror("read");
            break;
        }
        if(n == 0) break;
        if(st->id == id && st->status == ACTIVE) {
            status = 1;
            break;
        }
    }
    return status;
}

void previousNCourse(int fd, int n, int whence) {
    lseek(fd, -(n * (sizeof(struct course))), whence);
}

int writeCourse(int fd, struct course *st, int operation) {
    switch(operation) {
        case ADD:
            lseek(fd, 0, SEEK_END);
            break;
        case UPDATE:
            previousNCourse(fd, 1, SEEK_CUR);
            break;
    }
    write(fd, st, sizeof(struct course));
}

void openCourseFile(int *fd, int flag) {
    *fd = open("course_details", flag);
    if(*fd == -1) {
        perror("open");
    }
}

void getCourseDetails(int client_socket, struct course *st) {
    char *str;
    char recv_buff[50];

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
    st->seats = atoi(recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Credits: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    st->credits = atoi(recv_buff);
    st->available_seats = st->seats;
}

int addCourse(int client_socket) {
    struct course st, tmp;
    int fd;
    openCourseFile(&fd, O_RDWR);
    int n, status = 0;
    char *str;

    getCourseDetails(client_socket, &st);
    if(read(fd, buff, 1) == 0) {
        st.id = 1;
    } else {
        previousNCourse(fd, 1, SEEK_END);
        read(fd, &tmp, sizeof(tmp));
        st.id = tmp.id + 1;
    }

    st.available_seats = st.seats;
    st.faculty_id = faculty_id;
    st.status = ACTIVE;

    writeCourse(fd, &st, ADD);
    close(fd);

    str = "Course Created Successfully\nCourse-id -> ";
    send(client_socket, str, strlen(str), MSG_DONTWAIT);
    memset(buff, 0, sizeof(buff));
    my_itoa(st.id, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_DONTWAIT);
    write(client_socket, "\n", 1);
}

void viewCourses(int client_socket) {
    struct course st;
    char *str;
    char recv_buff[20];
    int n, status = 0;

    int fd;
    openCourseFile(&fd, O_RDONLY);

    while(1) {
        n = read(fd, &st, sizeof(struct course));
        if(n < 0) {
            perror("read");
            break;
        }
        if(n == 0) break;
        if(st.faculty_id == faculty_id && st.status == ACTIVE) {
            str = "Id: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            my_itoa(st.id, buff, 10);
            send(client_socket, buff, strlen(buff), MSG_DONTWAIT);

            str = "\nName: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            send(client_socket, st.name, strlen(st.name), MSG_DONTWAIT);

            str = "\nDepartment: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            send(client_socket, st.department, strlen(st.department), MSG_DONTWAIT);

            str = "\nNo. of seats: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            my_itoa(st.seats, buff, 10);
            send(client_socket, buff, strlen(buff), MSG_DONTWAIT);

            str = "\nCredits: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            my_itoa(st.credits, buff, 10);
            send(client_socket, buff, strlen(buff), MSG_DONTWAIT);

            str = "\nNo. of available seats: ";
            send(client_socket, str, strlen(str), MSG_DONTWAIT);
            my_itoa(st.available_seats, buff, 10);
            send(client_socket, buff, strlen(buff), MSG_DONTWAIT);

            str = "\n\n";
            write(client_socket, str, strlen(str));
            status = 1;
        }
    }
    if(!status) {
        str = "No course offered.\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}

int modifyCourse(int client_socket) {
    struct course st;
    char *str;
    int fd;
    openCourseFile(&fd, O_RDWR);
    if(getCourse(client_socket, &st, fd)) {
        getCourseDetails(client_socket, &st);
        writeCourse(fd, &st, UPDATE);
    } else {
        str = "Course not found\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}

void removeCourse(int client_socket) {
    struct course st;
    char *str;
    int fd;
    openCourseFile(&fd, O_RDWR);
    if(getCourse(client_socket, &st, fd)) {
        st.status = INACTIVE;
        writeCourse(fd, &st, UPDATE);
    } else {
        str = "Course not found\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}

#endif