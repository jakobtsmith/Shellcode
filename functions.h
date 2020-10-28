#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/file.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

void cd(char* change);
void path(char* pathfile, char* prompt);
void execFlags(char** flags);
void pipecmd(char* line);
void redircmd(char* line);
int ispipe(char* line);
void myhistory(char history[][512], int counter);

