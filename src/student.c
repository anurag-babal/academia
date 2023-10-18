#include "../headers/headers.h"
#include "../headers/config.h"
#include "../headers/student.h"
#include "../headers/course.h"
#include "../headers/constant.h"
#include "../headers/login.h"
#include "../headers/read_line.h"
#include "../headers/registration.h"

int readStudentRecordFromFile(int fd, struct student *st) {
    int n = read(fd, st, sizeof(struct student));
    if(n < 0) {
        perror("read");
        return -1;
    }
    if(n == 0) return 0;
    return 1;
}

int lockRecord(int fd, int type, int whence) {
    struct flock fl;
    switch(type) {
        case READ:
            fl.l_type = F_RDLCK;
            break;
        case WRITE:
            fl.l_type = F_WRLCK;
            break;
        case UNLOCK:
            fl.l_type = F_UNLCK;
    }
    fl.l_whence = SEEK_CUR;
    fl.l_len = sizeof(struct student);
    fl.l_start = 0;
    return fcntl(fd, F_SETLKW, &fl);
}

int findStudentById(int fd, struct student *st, int id) {
    int status = 0;
    lockFile(fd, READ);
    while(read(fd, st, sizeof(struct student)) > 0) {
        if(st->id == id) {
            status = 1;
            break;
        }
    }
    lockFile(fd, UNLOCK);
    return status;
}

void previousNStudent(int fd, int n, int whence) {
    lseek(fd, -(n * (sizeof(struct student))), whence);
}

int writeStudent(int fd, struct student *st, int operation) {
    int status;
    switch(operation) {
        case ADD:
            lseek(fd, 0, SEEK_END);
            break;
        case UPDATE:
            previousNStudent(fd, 1, SEEK_CUR);
            break;
    }
    lockFile(fd, WRITE);
    status = write(fd, st, sizeof(struct student));
    lockFile(fd, UNLOCK);
    return status;
}

void openStudentFile(int *fd, int flag) {
    *fd = open("student_details", flag | O_CREAT, 0600);
    if(*fd == -1) {
        perror("open");
    }
}

void displayStudentDetails(int client_socket, struct student *st) {
    char *str;
    char buff[10];

    str = "\n====================\n";
    send(client_socket, str, strlen(str), MSG_MORE);
    str = "Name: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->name, strlen(st->name), MSG_MORE);

    str = "\nAge: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    my_itoa(st->age, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_MORE);

    str = "\nEmail: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->email, strlen(st->email), MSG_MORE);

    str = "\nAddress: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->address, strlen(st->address), MSG_MORE);

    str = "\nLogin-id: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    send(client_socket, st->roll_no, strlen(st->roll_no), MSG_MORE);

    str = "\nEnrolled courses: ";
    send(client_socket, str, strlen(str), MSG_MORE);
    memset(buff, 0, sizeof(buff));
    int enrolled_course;
    char tmp[10];
    for(int i = 0; i < MAX_COURSE; i++) {
        enrolled_course = st->enrolled_courses[i];
        if(enrolled_course != 0) {
            my_itoa(enrolled_course, tmp, 10);
            strcat(buff, " ");
            strcat(buff, tmp);
        }
    }
    send(client_socket, buff, strlen(buff), MSG_MORE);
    str = "\n====================\n";
    send(client_socket, str, strlen(str), MSG_MORE);
}

void getStudentDetails(int client_socket, struct student *st) {
    char *str;
    char recv_buff[50], buff[50];

    memset(buff, 0, sizeof(recv_buff));
    str = "Name: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->name, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Age: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    st->age = atoi(recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Email: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->email, recv_buff);

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Address: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    my_strcpy(st->address, recv_buff);
}

int addStudent(int client_socket) {
    struct student st, tmp;
    int fd;
    char roll_no[10] = "MT", buff[50];
    int n, status = 0;
    char *msg;

    openStudentFile(&fd, O_RDWR);
    getStudentDetails(client_socket, &st);
    lockFile(fd, READ);
    if(read(fd, buff, 1) <= 0) {
        st.id = 1;
    } else {
        previousNStudent(fd, 1, SEEK_END);
        read(fd, &tmp, sizeof(struct student));
        st.id = tmp.id + 1;
    }
    lockFile(fd, UNLOCK);

    st.status = 0;
    char abc[20];
    my_itoa(st.id, abc, 10);
    strcat(roll_no, abc);
    my_strcpy(st.roll_no, roll_no);
    printf("%s\n", st.roll_no);
    saveLoginDetails(st.id, st.roll_no, DEFAULT_PASS, STUDENT, ADD);
    for(int i = 0; i < MAX_COURSE; i++)
        st.enrolled_courses[i] = 0;
    if(writeStudent(fd, &st, ADD) > 0) {
        printf("Student saved\n");
    } else printf("Failed\n");
    close(fd);

    msg = "Student Created Successfully\nLogin-id -> ";
    send(client_socket, msg, strlen(msg), MSG_MORE);
    send(client_socket, st.roll_no, strlen(st.roll_no), MSG_MORE);
    msg = "\nStudent Id -> ";
    send(client_socket, msg, strlen(msg), MSG_MORE);
    memset(buff, 0, sizeof(buff));
    my_itoa(st.id, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_MORE);
    msg = "\n====================\n";
    send(client_socket, msg, strlen(msg), MSG_MORE);
}

void viewStudent(int client_socket) {
    struct student st;
    char *str;
    char buff[50];
    int fd;
    openStudentFile(&fd, O_RDONLY);
    int id = getIdFromClient(client_socket, "Enter Id: ");
    if(findStudentById(fd, &st, id)) {
        displayStudentDetails(client_socket, &st);
    } else {
        str = "==========Student not found==========\n";
        send(client_socket, str, strlen(str), MSG_MORE);
    }
    close(fd);
}

int modifyStudent(int client_socket, int action) {
    struct student st;
    char *str;
    int fd;
    openStudentFile(&fd, O_RDWR);
    int id = getIdFromClient(client_socket, "Enter Student Id: ");
    if(findStudentById(fd, &st, id)) {
        switch(action) {
            case INACTIVE:
                st.status = 0;
                break;
            case ACTIVE:
                st.status = 1;
                break;
            case UPDATE:
                getStudentDetails(client_socket, &st);
                break;
        }
        writeStudent(fd, &st, UPDATE);
    } else {
        str = "==========Student not found==========\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}

int getEnrolledCoursesCount(struct student st) {
    int count = 0;
    for(int i = 0; i < MAX_COURSE; i++) {
        if(st.enrolled_courses[i] != 0) {
            count++;
        }
    }
    return count;
}

int checkCourseAlreadyEnrolled(struct student st, int course_id) {
    for(int i = 0; i < MAX_COURSE; i++) {
        if(st.enrolled_courses[i] == course_id) return 1;
    }
    return 0;
}

int updateEnrolledCourses(int student_id, struct student *st, int course_id) {
    for(int i = 0; i < MAX_COURSE; i++) {
        if(st->enrolled_courses[i] == 0) {
            st->enrolled_courses[i] = course_id;
            return 1;
        }
    }
    return 0;
}

void registerStudent(int client_socket, int student_id) {
    char *str;
    int fd_course, fd_student;
    struct student st_student;
    struct course st_course;
    int course_id = getIdFromClient(client_socket, "Enter course id: ");
    
    openCourseFile(&fd_course, O_RDONLY);
    openStudentFile(&fd_student, O_RDWR);

    if(findStudentById(fd_student, &st_student, student_id)) {
        printf("st_id: %d\n", st_student.id);
        if(getEnrolledCoursesCount(st_student) > MAX_COURSE) {
            str = "==========You have reached your maximum limit==========\n";
        } else if(checkCourseAlreadyEnrolled(st_student, course_id)) {
            str = "==========You have already registered for this course==========\n";
        } else {
            if(findCourseById(fd_course, &st_course, course_id)) {
                printf("course id: %d\n", st_course.id);
                if(st_course.available_seats != 0) {
                    if(insertNewStudent(student_id, course_id)) {
                        updateEnrolledCourses(student_id, &st_student, course_id);
                        updateSeat(course_id, DEC);
                        writeStudent(fd_student, &st_student, UPDATE);
                        str = "==========Student registered successfully==========\n";
                    } else {
                        str = "==========Can't register==========\n";
                    }
                } else {
                    str = "==========Course seats full==========\n";
                }
            } else {
                str = "==========Course not found==========\n";
            }
        }
    } else {
        str = "==========Student not found==========\n";
    }
    send(client_socket, str, strlen(str), MSG_MORE);
    close(fd_student);
    close(fd_course);
}


int dropEnrolledCourse(struct student *st, int course_id) {
    for(int i = 0; i < MAX_COURSE; i++) {
        if(st->enrolled_courses[i] == course_id) {
            st->enrolled_courses[i] = 0;
            return 1;
        }
    }
    return 0;
}

void removeEnrolledCourse(int student_id, int course_id) {
    int fd;
    struct student st;
    openStudentFile(&fd, O_RDWR);
    findStudentById(fd, &st, student_id);
    dropEnrolledCourse(&st, course_id);
    writeStudent(fd, &st, UPDATE);
    close(fd);
}

void dropCourse(int client_socket, int student_id) {
    int fd_course, fd_student;
    char *str;
    struct course st_course;
    struct student st_student;
    int course_id = getCourseIdFromClient(client_socket);
    openCourseFile(&fd_course, O_RDONLY);
    if(findCourseById(fd_course, &st_course, course_id)) {
        openStudentFile(&fd_student, O_RDWR);
        findStudentById(fd_student, &st_student, student_id);
        removeStudentFromCourse(student_id, course_id);
        dropEnrolledCourse(&st_student, course_id);
        writeStudent(fd_student, &st_student, UPDATE);
        updateSeat(course_id, INC);
        close(fd_student);
        str = "==========Course dropped successfully==========\n";
    } else {
        str = "==========Course not found==========\n";
    }
    send(client_socket, str, strlen(str), MSG_MORE);
    close(fd_course);
}

void viewEnrolledCourses(int client_socket, int student_id) {
    char *str;
    int fd_student, fd_course, count = 0;
    struct student st;
    struct course st_course;
    openStudentFile(&fd_student, O_RDONLY);
    if(findStudentById(fd_student, &st, student_id)) {
        openCourseFile(&fd_course, O_RDONLY);
        for(int i = 0; i < MAX_COURSE; i++) {
            int course_id = st.enrolled_courses[i];
            if(course_id != 0) {
                findCourseById(fd_course, &st_course, course_id);
                displayCourseDetail(client_socket, &st_course);
                count++;
            }
        }
        if(!count) {
            str = "==========Not enrolled any course==========\n";
            send(client_socket, str, strlen(str), MSG_MORE);
        }
        close(fd_course);
    } else {
        str = "==========Student not found==========\n";
        send(client_socket, str, strlen(str), MSG_MORE);
    }
    close(fd_student);
}