#include "myShell.h"
#include "myFunctionsShell.h"
#include <sys/stat.h>
#include <string.h>

int main(int argc, char const *argv[])
{
    // מציג מסך פתיחה עם לוגו
    welcome();

    // לולאה אין-סופית שמחכה לפקודות מהמשתמש
    while (1)
    {
        // מדפיס את מיקום המשתמש (נתיב נוכחי)
        getLocation();

        // מקבל קלט מהמשתמש
        char *input = inputFromUser();

        // מפרק את הקלט למערך של מחרוזות (ארגומנטים)
        char **arguments = splitArguments(input);

        // בודק אם הפקודה מכילה Pipe (|)
        int isPipe = -1;
        for (int i = 0; arguments[i] != NULL; i++)
        {
            if (strcmp(arguments[i], "|") == 0)
            {
                isPipe = i;
                break;
            }
        }

        // מחשב את מספר הארגומנטים שהוזנו
        int size = 0;
        while (arguments[size] != NULL)
        {
            size++;
        }

        // תנאים לביצוע הפקודות בהתאם לקלט המשתמש
        if (strcmp(input, "exit") == 0) // יציאה מהתוכנית
        {
            logout(input);
        }
        else if (strcmp(input, "cd") == 0) // שינוי תיקייה
        {
            cd(arguments);
        }
        else if (strcmp(input, "cp") == 0) // העתקת קובץ
        {
            cp(arguments);
        }
        else if (strcmp(input, "delete") == 0) // מחיקת קובץ
        {
            delete (arguments[1]);
        }
        else if (strcmp(input, "dir") == 0) 
        {
            puts("Impl");
        }
        else if (strcmp(input, "mv") == 0) // העברת קובץ (mv)
        {
            move(arguments);
        }
        else if (strcmp(input, "echo") == 0) // כתיבה לקובץ עם הפניות (>> או >)
        {
            if (strcmp(arguments[size - 2], ">>") == 0)
            {
                echoppend(arguments); // הוספת תוכן לקובץ קיים
            }
            else if (strcmp(arguments[size - 2], ">") == 0)
            {
                echowrite(arguments); // כתיבה לקובץ (מחיקה וכתיבה חדשה)
            }
            else
            {
                fprintf(stderr, "Error: you need to chose >> or >\n");
            }
        }
        else if (strcmp(input, "read") == 0) // קריאת קובץ
        {
            myRead(arguments);
        }
        else if (strcmp(input, "wc") == 0) // ספירת מילים או שורות בקובץ
        {
            wordCount(arguments);
        }
        else if (isPipe != -1) // ביצוע פקודה עם Pipe
        {
            arguments[isPipe] = NULL;
            myPipe(arguments, arguments + isPipe + 1);
        }
        else // במקרה שהפקודה לא מוכרת, מנסה להריץ אותה כמערכתית
        {
            systemCall(arguments);
            wait(NULL); // ממתין להשלמת התהליך
        }

        // שחרור זיכרון שהוקצה
        free(arguments);
        free(input);
    }

    return 0;
}

// פונקציה להצגת מסך הפתיחה (לוגו צבעוני)
void welcome()
{
    char *logo[] = {
        "\033[0;32mW       W  EEEEE  L        CCCC   OOO   M   M  EEEEE\n"
        "W       W  E      L       C      O   O  MM MM  E    \n"
        "W   W   W  EEEE   L       C      O   O  M M M  EEEE \n"
        "W   W   W  E      L       C      O   O  M   M  E    \n"
        "W   W   W  EEEEE  LLLLL   CCCC   OOO   M   M  EEEEE\033[0m\n",
        '\0'}; // '\0' מייצג את הסיום של המערך

    int i = 0;
    do
    {
        printf("%s", logo[i++]); // מדפיס את הלוגו
    } while (logo[i]);

    puts("\n"); // מעבר שורה לאחר הצגת הלוגו
}
