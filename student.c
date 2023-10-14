#include "config.h"

int readStudentRecordFromFile(int fd, struct student *st) {
    int status = 0;
    int n = read(fd, st, sizeof(struct student));
    if(n < 0) {
        perror("read");
        return -1;
    }
    if(n == 0) return 0;
    return 1;
}

int findStudentById(int fd, struct student *st, int id) {
    int status = 0;
    while(1) {
        if(readStudentRecordFromFile(fd, st) == 1) {
            if(st->id == id) {
                status = 1;
                break;
            }
        } else break;
    }
    return status;
}

int getStudentIdFromClient(int client_socket) {
    char recv_buff[20];

    memset(recv_buff, 0, sizeof(recv_buff));
    str = "Enter Studetn Id: ";
    write(client_socket, str, strlen(str));
    read_line(client_socket, recv_buff, sizeof(recv_buff));
    return atoi(recv_buff);
}

void previousNStudent(int fd, int n, int whence) {
    lseek(fd, -(n * (sizeof(struct student))), whence);
}

void writeStudent(int fd, struct student *st, int operation) {
    switch(operation) {
        case ADD:
            lseek(fd, 0, SEEK_END);
            break;
        case UPDATE:
            previousNStudent(fd, 1, SEEK_CUR);
            break;
    }
    write(fd, st, sizeof(struct student));
}

void openStudentFile(int *fd, int flag) {
    *fd = open("student_details", flag | O_CREAT, 0600);
    if(*fd == -1) {
        perror("open");
    }
}

void getStudentDetails(int client_socket, struct student *st) {
    char *str;
    char recv_buff[50];

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
    int fd = open("student_details", O_RDWR);
    char roll_no[10] = "MT";
    int n, status = 0;
    char *str;

    getStudentDetails(client_socket, &st);
    if(read(fd, buff, 1) == 0) {
        st.id = 1;
    } else {
        previousNStudent(fd, 1, SEEK_END);
        read(fd, &tmp, sizeof(tmp));
        st.id = tmp.id + 1;
    }
    st.status = 1;

    char abc[20];
    my_itoa(st.id, abc, 10);
    strcat(roll_no, abc);
    strcpy(st.roll_no, roll_no);
    saveLoginDetails(st.id, st.roll_no, default_password, STUDENT, ADD);

    writeStudent(fd, &st, ADD);
    close(fd);

    str = "Student Created Successfully\nLogin-id -> ";
    send(client_socket, str, strlen(str), MSG_DONTWAIT);
    send(client_socket, st.roll_no, strlen(st.roll_no), MSG_DONTWAIT);
    str = "\nStudent Id -> ";
    send(client_socket, str, strlen(str), MSG_DONTWAIT);
    memset(buff, 0, sizeof(buff));
    my_itoa(st.id, buff, 10);
    send(client_socket, buff, strlen(buff), MSG_DONTWAIT);
    write(client_socket, "\n", 1);
}

void viewStudent(int client_socket) {
    struct student st;
    char *str;
    int fd;
    openStudentFile(&fd, O_RDONLY);
    int id = getStudentIdFromClient(client_socket);
    if(findStudentById(client_socket, &st, id)) {
        str = "Name: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st.name, strlen(st.name), MSG_DONTWAIT);

        str = "\nAge: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        printf("%d\n", st.age);
        my_itoa(st.age, buff, 10);
        printf("%s\n", buff);
        send(client_socket, buff, strlen(buff), MSG_DONTWAIT);

        str = "\nEmail: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st.email, strlen(st.email), MSG_DONTWAIT);

        str = "\nAddress: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st.address, strlen(st.address), MSG_DONTWAIT);

        str = "\nLogin-id: ";
        send(client_socket, str, strlen(str), MSG_DONTWAIT);
        send(client_socket, st.roll_no, strlen(st.roll_no), MSG_DONTWAIT);

        write(client_socket, "\n", 1);
    } else {
        str = "Student not found\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}

int modifyStudent(int client_socket, int action) {
    struct student st;
    char *str;
    int fd;
    openStudentFile(&fd, O_RDWR);
    int id = getStudentIdFromClient(client_socket);
    if(findStudentById(client_socket, &st, id)) {
        switch(action) {
            case 1:
                st.status = 0;
                break;
            case 2:
                st.status = 1;
                break;
            case 3:
                getStudentDetails(client_socket, &st);
                break;
        }
        writeStudent(fd, &st, UPDATE);
    } else {
        str = "Student not found\n";
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

void registerStudent(int client_socket) {
    int fd_course, fd_student;
    struct student st_student;
    struct course st_course;
    int course_id = getCourseIdFromClient(client_socket);
    
    openCourseFile(&fd_course, O_RDONLY);
    openStudentFile(&fd_student, O_RDWR);

    if(findStudentById(fd_student, &st_student, student_id)) {
        if(getEnrolledCoursesCount(st_student) > MAX_COURSE) {
            str = "You have reached your maximum limit\n";
        } else if(checkCourseAlreadyEnrolled(st_student, course_id)) {
            str = "You have already registered for this course\n";
        } else {
            if(findCourseById(fd_course, &st_course, course_id)) {
                if(st_course.available_seats != 0) {
                    if(insertNewStudent(student_id, course_id)) {
                        updateSeat(course_id, DEC);
                    } else {
                        str = "Can't register\n";
                    }
                } else {
                    str = "Course seats full\n";
                }
            } else {
                str = "Course not found\n";
            }
        }
    } else {
        str = "Student not found\n";
    }
    write(client_socket, str, strlen(str));
    close(fd_student);
    close(fd_course);
}

void dropCourse(int client_socket) {
    int fd;
    struct course st;
    int course_id = getCourseIdFromClient(client_socket);
    openCourseFile(&fd, O_RDONLY);
    if(findCourseById(fd, &st, course_id)) {
        removeStudentFromCourse(student_id, course_id);
        updateSeat(course_id, INC);
    } else {
        char *str = "Course not found\n";
        write(client_socket, str, strlen(str));
    }
    close(fd);
}

void viewEnrolledCourses(int client_socket) {
    int fd_student, fd_course;
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
            }
        }
        close(fd_course);
    } else {
        str = "Student not found\n";
        write(client_socket, str, strlen(str));
    }
    close(fd_student);
}