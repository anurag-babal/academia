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
    return status;
}

void enrolledCourses(int student_id, int course_id) {
    int fd, fd_course_file, n;
    struct registration st;
    openRegistrationFile(&fd, O_RDONLY);
    if(findRegistrationByCourseId(fd, &st, course_id)) {
        // add student in enrolled array
        printf("%d\n", st.count);
        printf("%d\n", st.course_id);
        struct enrolled_student st1;
        fd_course_file = open(st.file_name, O_RDONLY);
        while(1) {
            n = read(fd_course_file, &st1, sizeof(struct enrolled_student));
            if(n < 0) {
                perror("read");
                break;
            }
            if(n == 0) break;
            printf("st_id = %d\n", st1.student_id);
        }
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

    write(fd_reg, &st_reg, sizeof(struct registration));

    lseek(fd_course_file, 0, SEEK_END);
    write(fd_course_file, &st_enroll, sizeof(struct enrolled_student));

    close(fd_reg);
    close(fd_course_file);
    return 1;
}

int readEnrolledRecordFromFile(int fd, struct enrolled_student *st) {
    int status = 0;
    int n = read(fd, st, sizeof(struct enrolled_student));
    if(n < 0) {
        perror("read");
        return -1;
    }
    if(n == 0) return 0;
    return 1;
}

int findEnrolledStudentById(int fd, struct enrolled_student *st, int id) {
    while(read(fd, st, sizeof(struct enrolled_student)) > 0) {
        if(st->student_id == id && st->status == ACTIVE) return 1;
    }
    return 0;
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
            write(fd_course_file, &st_enroll, sizeof(struct enrolled_student));
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
        while(count < n) {
            lseek(fd_course_file, -(i * sizeof(struct enrolled_student)), SEEK_END);
            if(read(fd_course_file, &st_enroll, sizeof(struct enrolled_student)) <= 0) break;
            if(st_enroll.status == ACTIVE) {
                st_enroll.status = INACTIVE;
                removeStudentFromCourse(st_enroll.student_id, course_id);
                removeEnrolledCourse(st_enroll.student_id, course_id);
                lseek(fd_course_file, -(sizeof(struct enrolled_student)), SEEK_CUR);
                write(fd_course_file, &st_enroll, sizeof(struct enrolled_student));
                count++;
            }
            i++;
        }
        close(fd_course_file);
    }
}