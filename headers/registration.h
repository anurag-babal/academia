#ifndef REGISTRATION_H
#define REGISTRATION_H

struct registration {
    int course_id;
    int count;
    char file_name[50];
};

struct enrolled_student {
    int student_id;
    int status;
};

int insertNewStudent(int student_id, int course_id);
void removeStudentFromCourse(int student_id, int course_id);
void removeLastNStudent(int course_id, int n);

#endif