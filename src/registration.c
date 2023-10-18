#include "../headers/headers.h"
#include "../headers/config.h"
#include "../headers/course.h"
#include "../headers/constant.h"
#include "../headers/registration.h"
#include "../headers/student.h"

void openRegistrationFile(int *fd, int flag) {
    *fd = open("registration_details", flag | O_CREAT, 0600);
    if(*fd == -1) {
        perror("open");
    }
}

int findRegistrationByCourseId(int fd, struct registration *st, int course_id) {
    int n, status = 0;
    lockFile(fd, READ);
    while(1) {
        n = read(fd, st, sizeof(struct registration));
        if(n < 0) {
            perror("read");
            break;
        }
        if(n == 0) break;
        if(st->course_id == course_id) {
            status = 1;
            break;
        }
    }
    lockFile(fd, UNLOCK);
    return status;
}

void enrolledCourses(int student_id, int course_id) {
    int fd, fd_course_file, n;
    struct registration st;
    openRegistrationFile(&fd, O_RDONLY);
    if(findRegistrationByCourseId(fd, &st, course_id)) {
        // add student in enrolled array
        struct enrolled_student st1;
        fd_course_file = open(st.file_name, O_RDONLY);
        lockFile(fd_course_file, READ);
        while(1) {
            n = read(fd_course_file, &st1, sizeof(struct enrolled_student));
            if(n < 0) {
                perror("read");
                break;
            }
            if(n == 0) break;
        }
        lockFile(fd_course_file, UNLOCK);
        close(fd_course_file);
    } else {
        printf("Student not registered\n");
    }
    close(fd);
}

int insertNewStudent(int student_id, int course_id) {
    int fd_reg, fd_course_file, fd_course;
    struct registration st_reg;
    struct enrolled_student st_enroll;
    struct course st_course;
    openRegistrationFile(&fd_reg, O_RDWR);

    if(findRegistrationByCourseId(fd_reg, &st_reg, course_id)) {
        fd_course_file = open(st_reg.file_name, O_RDWR);
        st_enroll.student_id = student_id;
        st_enroll.status = ACTIVE;
        st_reg.count++;
        lseek(fd_reg, -(sizeof(struct registration)), SEEK_CUR);
    } else {
        // create a new entry for course
        openCourseFile(&fd_course, O_RDONLY);
        if(findCourseById(fd_course, &st_course, course_id)) {
            st_reg.count = 1;
            st_reg.course_id = course_id;
            strcpy(st_reg.file_name, st_course.name);

            st_enroll.student_id = student_id;
            st_enroll.status = ACTIVE;
            fd_course_file = open(st_reg.file_name, O_RDWR | O_CREAT | O_EXCL, 0640);
            lseek(fd_reg, 0, SEEK_END);
            close(fd_course);
        } else {
            close(fd_reg);
            close(fd_course);
            return 0;
        }
    }

    lockFile(fd_reg, WRITE);
    write(fd_reg, &st_reg, sizeof(struct registration));
    lockFile(fd_reg, UNLOCK);

    lockFile(fd_course_file, WRITE);
    lseek(fd_course_file, 0, SEEK_END);
    write(fd_course_file, &st_enroll, sizeof(struct enrolled_student));
    lockFile(fd_course_file, UNLOCK);

    close(fd_reg);
    close(fd_course_file);
    return 1;
}

int readEnrolledRecordFromFile(int fd, struct enrolled_student *st) {
    int status = 0;
    lockFile(fd, READ);
    int n = read(fd, st, sizeof(struct enrolled_student));
    if(n < 0) {
        perror("read");
        status = -1;
    }
    if(n == 0) status = 0;
    status = 1;
    lockFile(fd, WRITE);
    return status;
}

int findEnrolledStudentById(int fd, struct enrolled_student *st, int id) {
    int status = 0;
    lockFile(fd, READ);
    while(read(fd, st, sizeof(struct enrolled_student)) > 0) {
        if(st->student_id == id && st->status == ACTIVE) status = 1;
    }
    status = 0;
    lockFile(fd, UNLOCK);
    return status;
}

void removeStudentFromCourse(int student_id, int course_id) {
    int fd_reg, fd_course_file;
    struct registration st_reg;
    struct enrolled_student st_enroll;

    openRegistrationFile(&fd_reg, O_RDWR);
    if(findRegistrationByCourseId(fd_reg, &st_reg, course_id)) {
        fd_course_file = open(st_reg.file_name, O_RDWR);
        if(findEnrolledStudentById(fd_course_file, &st_enroll, student_id)) {
            st_enroll.status = INACTIVE;
            lseek(fd_course_file, -(sizeof(struct enrolled_student)), SEEK_CUR);
            lockFile(fd_course_file, WRITE);
            write(fd_course_file, &st_enroll, sizeof(struct enrolled_student));
            lockFile(fd_course_file, UNLOCK);
        }
        close(fd_course_file);
    }
    close(fd_reg);
}

void removeLastNStudent(int course_id, int n) {
    int fd_reg, fd_course_file;
    struct registration st_reg;
    struct enrolled_student st_enroll;

    openRegistrationFile(&fd_reg, O_RDWR);

    if(findRegistrationByCourseId(fd_reg, &st_reg, course_id)) {
        int count = 0, i = 1;
        fd_course_file = open(st_reg.file_name, O_RDWR);
        lockFile(fd_course_file, READ);
        while(count < n) {
            lseek(fd_course_file, -(i * sizeof(struct enrolled_student)), SEEK_END);
            if(read(fd_course_file, &st_enroll, sizeof(struct enrolled_student)) <= 0) break;
            if(st_enroll.status == ACTIVE) {
                st_enroll.status = INACTIVE;
                removeStudentFromCourse(st_enroll.student_id, course_id);
                removeEnrolledCourse(st_enroll.student_id, course_id);
                lseek(fd_course_file, -(sizeof(struct enrolled_student)), SEEK_CUR);
                lockFile(fd_course_file, UNLOCK);
                lockFile(fd_course_file, WRITE);
                write(fd_course_file, &st_enroll, sizeof(struct enrolled_student));
                lockFile(fd_course_file, UNLOCK);
                count++;
            }
            i++;
        }
        lockFile(fd_course_file, UNLOCK);
        close(fd_course_file);
    }
}