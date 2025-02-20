#include "myFunctionsShell.h"
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <string.h>

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

char **splitArguments(char *str)
{

    int len = strlen(str);
    int ind = 0;

    char **words = calloc(len + 1, sizeof(char *));

    int stringMode = 0;

    words[0] = str;

    if (str[0] == '"')
    {
        str[0] = str[1];
        stringMode = !stringMode;
    }

    for (int i = 0; i < len; i++)
    {
        if (str[i] == '"')
        {
            stringMode = !stringMode;
            str[i] = '\0';
        }

        if (str[i] == ' ' && !stringMode)
        {

            str[i] = '\0';

            if (str[i + 1] == '"')
            {
                words[++ind] = &str[i + 2];
            }
            else
                words[++ind] = &str[i + 1];
        }
    }

    if (stringMode)
    {
        puts("Error: Unclosed string detected");
        return NULL;
    }

    if (str[len - 1] == '"')
    {
        str[len - 1] = '\0';
    }

    // for (int i = 0; i <= ind; i++) {
    //     puts(words[i]);
    // }

    words = realloc(words, sizeof(char *) * (ind + 2));
    if (words == NULL)
    {
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

void cd(char **arguments)
{
    int size = 0;
    while (arguments[size] != NULL)
    {
        size++;
    }

    if (size > 2)
    {
        perror("to manny arguments");
    }
    else if (size == 1)
    {
        chdir(getenv("HOME"));
    }
    else
    {
        chdir(arguments[1]);
    }
}

void cp(char **arguments)
{

    int size = 0;
    while (arguments[size] != NULL)
    {
        size++;
    }

    if (size != 3)
    {
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

void delete(char *str)
{
    int status;
    status = remove(str);
    if (status == 0)
        printf("%s file deleted successfully.\n", str);
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

void move(char **args)
{
    int size = 0;
    while (args[size] != NULL)
    {
        size++;
    }

    if (size != 3)
    {
        fprintf(stderr, "Error: Too few or too many arguments.\n");
        return;
    }

    char *src = args[1];
    char *dest = args[2];
    struct stat statbufSrc, statbufDest;

    if (stat(src, &statbufSrc) != 0)
    {
        fprintf(stderr, "Error: Source file \"%s\" does not exist: %s\n", src, strerror(errno));
        return;
    }

    int isDestDir = (stat(dest, &statbufDest) == 0 && S_ISDIR(statbufDest.st_mode));

    char *finalDest;
    if (isDestDir)
    {
        finalDest = malloc(strlen(dest) + strlen(basename(src)) + 2);
        sprintf(finalDest, "%s/%s", dest, basename(src));
    }
    else
    {
        finalDest = strdup(dest);
    }

    if (rename(src, finalDest) != 0)
    {
        fprintf(stderr, "Error: Unable to move \"%s\" to \"%s\": %s\n", src, finalDest, strerror(errno));
    }
    else
    {
        printf("Moved \"%s\" to \"%s\" successfully.\n", src, finalDest);
    }

    free(finalDest);
}

void echoppend(char **args)
{

    int size = 0;
    while (args[size] != NULL)
    {
        size++;
    }

    if (size < 2)
    {
        fprintf(stderr, "Error: Too few arguments.\n");
        return;
    }
    char *src = args[size - 1];
    FILE *fptr1;
    fptr1 = fopen(src, "a");
    if (fptr1 == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\".\n", src);
        return;
    }

    for (int i = 1; i < size - 2; i++)
    {
        fputs(args[i], fptr1);
        fputs(" ", fptr1);
    }
    fputs("\n", fptr1);

    fclose(fptr1);
}

void echowrite(char **args)
{
    int size = 0;
    while (args[size] != NULL)
    {
        size++;
    }

    if (size < 2)
    {
        fprintf(stderr, "Error: Too few arguments.\n");
        return;
    }
    char *src = args[size - 1];
    FILE *fptr1;
    fclose(fopen(src, "w"));
    fptr1 = fopen(src, "a");
    if (fptr1 == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\".\n", src);
        return;
    }

    for (int i = 1; i < size - 2; i++)
    {
        fputs(args[i], fptr1);
        fputs(" ", fptr1);
    }
    fputs("\n", fptr1);

    fclose(fptr1);
}

void myRead(char **args)
{
    int size = 0;
    while (args[size] != NULL)
    {
        size++;
    }

    if (size != 2)
    {
        fprintf(stderr, "Error: Too few or many arguments.\n");
        return;
    }
    char buffer[256];
    char *src = args[1];
    FILE *fptr1;
    fptr1 = fopen(src, "r");
    if (fptr1 == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\"\n", src);
        return;
    }
    while (fgets(buffer, sizeof(buffer), fptr1) != NULL)
    {
        printf("%s", buffer);
    }
}

void wordCount(char **args)

{
    int size = 0;
    while (args[size] != NULL)
    {
        size++;
    }

    if (size != 3)
    {
        fprintf(stderr, "Error: Too few or too many arguments.\n");
        return;
    }
    char buffer[256];
    char *src = args[2];
    FILE *fptr1;
    int cont = 0;
    fptr1 = fopen(src, "r");
    if (fptr1 == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\"\n", src);
        return;
    }
    if (strcmp(args[1], "-l") == 0)
    {
        while (fgets(buffer, sizeof(buffer), fptr1) != NULL)
        {
            cont++;
        }
    }
    else if (strcmp(args[1], "-w") == 0)
    {
        while (fgets(buffer, sizeof(buffer), fptr1) != NULL)
        {
            char *token = strtok(buffer, "\n"); 
            while (token != NULL)
            {
                cont++;                        
                token = strtok(NULL, "\n"); 
            }
        }
    }
    else
    {
        fprintf(stderr, "Error: you need to chose -l or -w\n");
        return;
    }
    printf("%d\n", cont);
}

