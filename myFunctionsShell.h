#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>


char* inputFromUser();
void getLocation();
char **splitArguments(char* );
void systemCall(char** arguments);
void logout();
void myPipe(char** argv1, char** argv2);
void cd(char **arguments);
void cp(char **arguments);
void delete (char *str);
void move(char **args);
void echoppend(char **args);
void echowrite(char **args);
void myRead(char **args);
void wordCount(char **args);

