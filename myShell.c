#include "myShell.h"
#include "myFunctionsShell.h"

int main(int argc, char const *argv[])
{

    welcome();
    while (1)
    {
        int isPipe = 0;
        getLocation();

        char *input = inputFromUser();

        // puts(input);
       
        char **arguments = splitArguments(input);

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
            puts("Impl");
        }
        else if (strcmp(input, "dir") == 0)
        {
            puts("Impl");
        }
        else if (isPipe)
        {
            myPipe(arguments,arguments+isPipe+1);
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
                                        "W   W   W  EEEEE  LLLLL   CCCC   OOO   M   M  EEEEE\033[0m\n", '\0'
    };
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