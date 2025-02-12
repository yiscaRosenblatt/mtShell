#include "myFunctionsShell.h"

char *inputFromUser()
{
    char ch;
    int len = 0;
    char *input = (char *)malloc(sizeof(char) * (len + 1));
    *input = '\0';
    while ((ch = getchar()) != '\n')
    {
        *(input + len) = ch;
        input = (char *)realloc(input, (++len + 1));
    }
    *(input + len) = '\0';

    return input;
}
// [l,s, ,-,l, ,-,s, ,-,u,\0]
// [l,s,\0,-,l,\0,-,s,\0,-,u,\0]
//[input,input+3,input+6,input+9, NULL]
//"ls\0-l\0-s\0-u\0"
//["ls", "-l", "-s" ,"-u",NULL]

// [ , , , , ,l,s, ,-,l, ,-,s, ,-,u,\0]
// [\0,\0,\0,\0,\0,l,s, ,-,l, ,-,s, ,-,u,\0]
//[input+5,input+8,input+1,input+14, NULL]
//"ls\0-l\0-s\0-u\0"
//["ls", "-l", "-s" ,"-u",NULL]
char **splitArguments(char *str)
{

   int len = strlen(str);
    int ind = 0;

    char** words = calloc(len+1, sizeof(char*));

    int stringMode = 0;

    words[0] = str; 

    if(str[0] == '"'){
        str[0] = str[1];
        stringMode = !stringMode;
    }
        

    for (int i = 0; i < len; i++) {
        if (str[i] == '"') {
            stringMode = !stringMode;
            str[i] = '\0';
        }

    
        if (str[i] == ' ' && !stringMode) {

            str[i] = '\0';
            

            if(str[i+1] == '"'){
                words[++ind] = &str[i + 2];
            }
            else
                words[++ind] = &str[i + 1];
        }
    }

    if (stringMode) {
        puts("Error: Unclosed string detected");
        return NULL;
    }

    if(str[len-1] == '"'){
        str[len-1] = '\0';
    }


        // for (int i = 0; i <= ind; i++) {
        //     puts(words[i]);
        // }

        words = realloc(words, sizeof(char*) * (ind + 2));
        if (words == NULL) {
            perror("Error reallocating memory");
            return NULL;
        }
    
        words[ind + 1] = NULL;

    return words;
}

void getLocation()
{
    char hostname[1024];
    char username[1024];
    char cwd[1024];

    // קבלת שם המשתמש
    char *user = getenv("USER");
    if (user)
    {
        strncpy(username, user, sizeof(username));
    }
    else
    {
        snprintf(username, sizeof(username), "unknown");
    }

    // קבלת שם המחשב
    if (gethostname(hostname, sizeof(hostname)) != 0)
    {
        snprintf(hostname, sizeof(hostname), "unknown");
    }

    // קבלת התיקייה הנוכחית
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        snprintf(cwd, sizeof(cwd), "unknown");
    }


    // הדפסת שורת הפקודה

    printf("\033[0;32m"); // מפעיל צבע ירוק
    printf("%s@%s", username, hostname);
    printf("\033[0m"); // מאפס את הצבע
    printf("%s", ":");
    printf("\033[0;34m"); // מפעיל צבע כחול
    printf("%s", cwd);
    printf("\033[0m"); // מאפס צבע
    printf("%s", "$ ");
}

void logout(char *input)
{
    free(input);
    puts("logout");
    exit(EXIT_SUCCESS);
}

void cd(char **arguments){
    int size = 0;
    while (arguments[size] != NULL) {
        size++;
    }
    
    if(size>2){
        perror("to manny arguments");
    }
    else if(size==1){
        chdir(getenv("HOME"));
    }else{
        chdir(arguments[1]);  
    }

}

void cp(char **arguments){

    int size = 0;
    while (arguments[size] != NULL) {
        size++;
    }
    

    if(size!=3){
        perror("too less or too manny arguments");
        return;
    }

    FILE *fptr1, *fptr2;
    int c;



    fptr1 = fopen(arguments[1], "r");
    if (fptr1 == NULL)
    {
        printf("Cannot open file %s\n", arguments[1]);
        exit(1);
    }


    fptr2 = fopen(arguments[2], "w");
    if (fptr2 == NULL)
    {
        printf("Cannot open file %s\n", arguments[2]);
        exit(1);
    }

    while ((c = fgetc(fptr1)) != EOF)
    {
        fputc(c, fptr2);
    }


    fclose(fptr1);
    fclose(fptr2);
    

}

void delete (char *str){
    int status;
   status = remove(str);
   if( status == 0 )
      printf("%s file deleted successfully.\n",str);
   else
   {
      printf("Unable to delete the file\n");
      perror("Error");
   }
}


void systemCall(char **arguments)

{
    puts("systemCall");
    pid_t pid = fork();
    if (pid == -1)
    {
        perror("fork err");
        return;
    }

    if (pid == 0)
    {

        if (execvp(arguments[0], arguments) == -1)
        {
            exit(EXIT_FAILURE);
        }
    }
}


void myPipe(char **argv1, char **argv2)
{

    int fd[2];
    if (fork() == 0)
    {
        pipe(fd);
        if (fork() == 0)
        {
            close(STDOUT_FILENO);
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);
            close(fd[0]);
            execvp(argv1[0], argv1);
        }

        close(STDIN_FILENO);
        dup(fd[0]);
        close(fd[1]);
        close(fd[0]);
        execvp(argv2[0], argv2);
    }
}