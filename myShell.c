#include "myShell.h"
#include "myFunctionsShell.h"
#include <sys/stat.h>
#include <string.h>

int main(int argc, char const *argv[])
{

    welcome();
    while (1)
    {
        // int isPipe = 0;
        getLocation();

        char *input = inputFromUser();

        // puts(input);

        char **arguments = splitArguments(input);
        int isPipe = -1;
        for (int i = 0; arguments[i] != NULL; i++)
        {
            if (strcmp(arguments[i], "|") == 0)
            {
                isPipe = i;
                break;
            }
        }
        int size = 0;
        while (arguments[size] != NULL)
        {
            size++;
        }

        if (strcmp(input, "exit") == 0)
        {
            logout(input);
        }
        else if (strcmp(input, "cd") == 0)
        {

            cd(arguments);
        }
        else if (strcmp(input, "cp") == 0)
        {
            cp(arguments);
        }
        else if (strcmp(input, "delete") == 0)
        {
            delete (arguments[1]);
        }
        else if (strcmp(input, "dir") == 0)
        {
            puts("Impl");
        }
        else if (strcmp(input, "mv") == 0)
        {
            move(arguments);
        }
        else if (strcmp(input, "echo") == 0)
        {

            if (strcmp(arguments[size- 2], ">>")==0)
            {
                echoppend(arguments);
            }
            else if (strcmp(arguments[size- 2], ">")==0)
            {
                echowrite(arguments);
            }
            else
            {
                fprintf(stderr, "Error: you need to chose >> or >\n");
            }
        }
        else if (strcmp(input, "read") == 0)
        {
            myRead(arguments);
        }
        else if (strcmp(input, "wc") == 0)
        {
            wordCount(arguments);
        }
        else if (isPipe != -1)
        {
            arguments[isPipe] = NULL;
            myPipe(arguments, arguments + isPipe + 1);
        }
        else
        {
            systemCall(arguments);
            wait(NULL);
        }

        free(arguments);
        free(input);
    }

    return 0;
}
void welcome()
{
    char *logo[] = {"\033[0;32mW       W  EEEEE  L        CCCC   OOO   M   M  EEEEE\n"
                    "W       W  E      L       C      O   O  MM MM  E    \n"
                    "W   W   W  EEEE   L       C      O   O  M M M  EEEE \n"
                    "W   W   W  E      L       C      O   O  M   M  E    \n"
                    "W   W   W  EEEEE  LLLLL   CCCC   OOO   M   M  EEEEE\033[0m\n",
                    '\0'};
    int i = 0;

    do
    {
        printf("%s", logo[i++]);
        // if (i % 2 != 0 || i > 26)
        //     green();
        // else
        //     blue();
    } while (logo[i]);
    // reset();
    puts("\n");
}