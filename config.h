#ifndef CONFIG_H
#define CONFIG_H

#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1
#define MAX_COURSE 5
#define INC 9
#define DEC 10
#define ADD 12
#define UPDATE 13
#define ACTIVE 14
#define INACTIVE 15
#define ADMIN 16
#define FACULTY 17
#define STUDENT 18

int faculty_id = 0;
int student_id = 0;
char faculty_login_id[50];
char student_login_id[50];
char buff[1024];
char *default_password = "pass";
char *str;

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

struct login {
    int id;
    char login_id[50];
    char password[50];
    int type;
};

struct faculty {
    int id;
    char name[50];
    char department[50];
    char designation[50];
    char email[50];
    char address[50];
    char login_id[50];
};

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

struct registration {
    int course_id;
    int count;
    char *file_name;
};

struct enrolled_student {
    int student_id;
    int status;
};


int addStudent(int);
void viewStudent(int);
int addFaculty(int);
void viewFaculty(int);
int modifyStudent(int, int);
int modifyFaculty(int);

void viewAllCoursesForFaculty(int);
int addCourse(int);
void removeCourse(int);
int modifyCourse(int);
void changePassword(int, int);

void viewAllCourses(int);
void registerStudent(int);
void dropCourse(int);
void viewEnrolledCourses(int);

int checkLoginDetails(char *login_id, char *password, int type);
void openLoginFile(int *fd, int flag);



void my_strcpy(char *dest, char *src) {
    while (*src != '\n') {
        *dest = *src;
        dest++;
        src++;
    }
    *dest = '\0';
}

void my_itoa(int num, char *str, int base) {
    int i = 0;
    while (num > 0) {
        int digit = num % base;
        str[i++] = digit + '0';
        num = num / base;
    }
    str[i] = '\0';

    // Reverse the string.
    for (int j = 0; j < i / 2; j++) {
        char temp = str[j];
        str[j] = str[i - j - 1];
        str[i - j - 1] = temp;
    }
}

#endif