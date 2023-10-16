#ifndef LOGIN_H
#define LOGIN_H

void openLoginFile(int *fd, int flag);
void changePassword(int, int, char *);
int checkLoginDetails(char *login_id, char *password, int type);
void saveLoginDetails(int id, char *login_id, char *password, int type, int operation);

struct login {
    int id;
    char login_id[50];
    char password[50];
    int type;
};

#endif