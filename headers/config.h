#ifndef CONFIG_H
#define CONFIG_H

void my_strcpy(char *dest, char *src);
void my_itoa(int num, char *str, int base);
int getIdFromClient(int client_socket, char *str);
int lockFile(int fd, int type);

#endif