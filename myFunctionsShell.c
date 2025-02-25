#include "myFunctionsShell.h"
#include <sys/stat.h>
#include <errno.h>
#include <libgen.h>
#include <string.h>

//  פונקציה לקבלת קלט מהמשתמש (שורה אחת) ומחזירה אותו כמערך של String
char *inputFromUser()
{
    char ch;
    int len = 0;
    // הקצאת זיכרון עבור הקלט
    char *input = (char *)malloc(sizeof(char) * (len + 1));
    *input = '\0';
    while ((ch = getchar()) != '\n')  // קלט עד לשורת חדשה
    {
        *(input + len) = ch;
        // הגדלת הזיכרון לכל תו נוסף
        input = (char *)realloc(input, (++len + 1));
    }
    *(input + len) = '\0';  // סיום המערך

    return input;
}

//  פונקציה שמפצלת מערך לשמות של פרמטרים (מילים) ומחזירה מערך של מערכים של string
char **splitArguments(char *str)
{
    int len = strlen(str);  // קבלת אורך המיתר
    int ind = 0;  // אינדקס שמייצג את המילה הנוכחית במערך words

    // הקצאת זיכרון עבור המילים שיתפוצלו
    char **words = calloc(len + 1, sizeof(char *));
    int stringMode = 0;  // משתנה שמצביע אם אנחנו בתוך מיתר (string) או לא

    words[0] = str;  // המילה הראשונה היא כל המיתר

    // אם המיתר מתחיל בציטוטים (")
    if (str[0] == '"')
    {
        str[0] = str[1];  // מתעלמים מהציטוט הראשון
        stringMode = !stringMode;  // נכנסים למצב של string (בתוך ציטוט)
    }

    for (int i = 0; i < len; i++)
    {
        // אם מוצאים ציטוט, נכנסים או יוצאים ממצב stringMode
        if (str[i] == '"')
        {
            stringMode = !stringMode;
            str[i] = '\0';  // מחליפים את הציטוט בתו null כדי לסיים את המילה
        }

        // אם מוצאים רווח (space) אבל לא בתוך string (לא בתוך ציטוטים)
        if (str[i] == ' ' && !stringMode)
        {
            str[i] = '\0';  // מסיימים את המילה הנוכחית

            // אם הרווח אחרי ציטוט, אז המשמעות היא התחלה של string חדש
            if (str[i + 1] == '"')
            {
                words[++ind] = &str[i + 2];  // המילה החדשה מתחילה אחרי הציטוט השני
            }
            else
                words[++ind] = &str[i + 1];  // המילה החדשה מתחילה אחרי הרווח
        }
    }

    // אם יש ציטוט שלא נסגר, כלומר string לא סגור
    if (stringMode)
    {
        puts("Error: Unclosed string detected");
        return NULL;
    }

    // אם המיתר מסתיים בציטוט, נסגור אותו על ידי החלפת הציטוט בתו null
    if (str[len - 1] == '"')
    {
        str[len - 1] = '\0';
    }

    // הרחבת המערך של המילים, אם יש יותר מ-2 מילים
    words = realloc(words, sizeof(char *) * (ind + 2));
    if (words == NULL)  // אם הקצאת הזיכרון נכשלת
    {
        perror("Error reallocating memory");
        return NULL;
    }

    // מסמנים את סוף המערך
    words[ind + 1] = NULL;

    return words;  // מחזירים את המערך של המילים המפוצלות
}

// פונקציה שמדפיסה את מיקום המשתמש, שם המחשב והתיקייה הנוכחית
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

    // הדפסת שורת הפקודה עם צבעים
    printf("\033[0;32m"); // צבע ירוק
    printf("%s@%s", username, hostname);
    printf("\033[0m"); // מחזיר צבע לקדמותו
    printf("%s", ":");
    printf("\033[0;34m"); // צבע כחול
    printf("%s", cwd);
    printf("\033[0m"); // מחזיר צבע לקדמותו
    printf("%s", "$ ");
}

// פונקציה שגורמת ליציאה מהתוכנית
void logout(char *input)
{
    free(input);
    puts("logout");
    exit(EXIT_SUCCESS);
}

// פונקציה שמבצעת פקודת cd (מעבר לתיקייה)
void cd(char **arguments)
{
    int size = 0;
    while (arguments[size] != NULL)
    {
        size++;
    }

    if (size > 2)
    {
        perror("Too many arguments");
    }
    else if (size == 1)
    {
        chdir(getenv("HOME"));  // אם לא נמסרה תיקייה, עובר לתיקיית הבית
    }
    else
    {
        chdir(arguments[1]);  // עובר לתיקייה שנמסרה כפרמטר
    }
}

// פונקציה שמבצעת העתקה של קובץ מקובץ מקור לקובץ יעד
void cp(char **arguments)
{
    int size = 0;
    while (arguments[size] != NULL)
    {
        size++;
    }

    if (size != 3)
    {
        perror("Too few or too many arguments");
        return;
    }

    FILE *fptr1, *fptr2;
    int c;

    fptr1 = fopen(arguments[1], "r");  // פותח את הקובץ המקורי לקריאה
    if (fptr1 == NULL)
    {
        printf("Cannot open file %s\n", arguments[1]);
        exit(1);
    }

    fptr2 = fopen(arguments[2], "w");  // פותח את קובץ היעד לכתיבה
    if (fptr2 == NULL)
    {
        printf("Cannot open file %s\n", arguments[2]);
        exit(1);
    }

    // מעתיק תו-תו
    while ((c = fgetc(fptr1)) != EOF)
    {
        fputc(c, fptr2);
    }

    fclose(fptr1);
    fclose(fptr2);
}

// פונקציה שמבצעת מחיקת קובץ
void delete(char *str)
{
    int status;
    status = remove(str);  // מנסה למחוק את הקובץ
    if (status == 0)
        printf("%s file deleted successfully.\n", str);
    else
    {
        printf("Unable to delete the file\n");
        perror("Error");
    }
}

// פונקציה לביצוע פקודת מערכת (כמו פקודות shell חיצוניות)
void systemCall(char **arguments)
{
    puts("systemCall");
    pid_t pid = fork();  // יצירת תהליך חדש
    if (pid == -1)
    {
        perror("fork err");
        return;
    }

    if (pid == 0)
    {
        if (execvp(arguments[0], arguments) == -1)  // הרצת הפקודה החיצונית
        {
            exit(EXIT_FAILURE);
        }
    }
}

// פונקציה לביצוע pipe בין שני תהליכים
void myPipe(char **argv1, char **argv2)
{
    int fd[2];
    if (fork() == 0)
    {
        pipe(fd);  // יצירת pipe
        if (fork() == 0)
        {
            close(STDOUT_FILENO);
            dup2(fd[1], STDOUT_FILENO);  // כיווץ פלט לתוך ה-pipe
            close(fd[1]);
            close(fd[0]);
            execvp(argv1[0], argv1);  // הרצת הפקודה הראשונה
        }

        close(STDIN_FILENO);
        dup(fd[0]);  // קריאת קלט מה-pipe
        close(fd[1]);
        close(fd[0]);
        execvp(argv2[0], argv2);  // הרצת הפקודה השנייה
    }
}

// פונקציה להזזת קובץ
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

    if (stat(src, &statbufSrc) != 0)  // בודק אם הקובץ קיים
    {
        fprintf(stderr, "Error: Source file \"%s\" does not exist: %s\n", src, strerror(errno));
        return;
    }

    int isDestDir = (stat(dest, &statbufDest) == 0 && S_ISDIR(statbufDest.st_mode));

    char *finalDest;
    if (isDestDir)
    {
        finalDest = malloc(strlen(dest) + strlen(basename(src)) + 2);
        sprintf(finalDest, "%s/%s", dest, basename(src));  // אם היעד הוא תיקייה, מוסיף את שם הקובץ לנתיב
    }
    else
    {
        finalDest = strdup(dest);  // אם לא, פשוט שם הקובץ ביעד
    }

    if (rename(src, finalDest) != 0)  // שינוי שם הקובץ או הזזתו
    {
        fprintf(stderr, "Error: Unable to move \"%s\" to \"%s\": %s\n", src, finalDest, strerror(errno));
    }
    else
    {
        printf("Moved \"%s\" to \"%s\" successfully.\n", src, finalDest);
    }

    free(finalDest);
}

//פונקציה זו מבצעת כתיבה בקובץ (append) — היא מוסיפה טקסט לקובץ קיים.
void echoppend(char **args)
{
    int size = 0;
    // סופרים את מספר הארגומנטים
    while (args[size] != NULL)
    {
        size++;
    }

    // אם יש פחות מ-2 ארגומנטים, הפונקציה מחזירה הודעת שגיאה
    if (size < 2)
    {
        fprintf(stderr, "Error: Too few arguments.\n");
        return;
    }
    
    // בוחרים את שם הקובץ (הארגומנט האחרון)
    char *src = args[size - 1];
    
    // פותחים את הקובץ למצב append (הוספה)
    FILE *fptr1;
    fptr1 = fopen(src, "a");
    
    // אם לא ניתן לפתוח את הקובץ, מחזירים הודעת שגיאה
    if (fptr1 == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\".\n", src);
        return;
    }

    // מוסיפים את כל המילים לקובץ, כאשר כל מילה מופרדת ברווח
    for (int i = 1; i < size - 2; i++)
    {
        fputs(args[i], fptr1);
        fputs(" ", fptr1);
    }
    
    // מוסיפים שורה חדשה בסוף
    fputs("\n", fptr1);

    // סוגרים את הקובץ
    fclose(fptr1);
}

//פונקציה זו כותבת לקובץ, ובניגוד ל-echoppend, היא מוחקת את תוכן הקובץ קודם (write mode).
void echowrite(char **args)
{
    int size = 0;
    // סופרים את מספר הארגומנטים
    while (args[size] != NULL)
    {
        size++;
    }

    // אם יש פחות מ-2 ארגומנטים, הפונקציה מחזירה הודעת שגיאה
    if (size < 2)
    {
        fprintf(stderr, "Error: Too few arguments.\n");
        return;
    }
    
    // בוחרים את שם הקובץ (הארגומנט האחרון)
    char *src = args[size - 1];
    
    // סוגרים את הקובץ אם הוא פתוח (נראה שיש בעיה עם שורת הקוד הזו כי לא נסגר כראוי לפני פתיחה מחודשת)
    fclose(fopen(src, "w"));
    
    // פותחים את הקובץ למצב append
    FILE *fptr1;
    fptr1 = fopen(src, "a");
    
    // אם לא ניתן לפתוח את הקובץ, מחזירים הודעת שגיאה
    if (fptr1 == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\".\n", src);
        return;
    }

    // מוסיפים את כל המילים לקובץ, כאשר כל מילה מופרדת ברווח
    for (int i = 1; i < size - 2; i++)
    {
        fputs(args[i], fptr1);
        fputs(" ", fptr1);
    }
    
    // מוסיפים שורה חדשה בסוף
    fputs("\n", fptr1);

    // סוגרים את הקובץ
    fclose(fptr1);
}

//פונקציה זו קוראת קובץ ומדפיסה את תוכנו למסך.
void myRead(char **args)
{
    int size = 0;
    // סופרים את מספר הארגומנטים
    while (args[size] != NULL)
    {
        size++;
    }

    // אם לא התקבלו בדיוק 2 ארגומנטים (שם פקודה ושם קובץ), הפונקציה מחזירה הודעת שגיאה
    if (size != 2)
    {
        fprintf(stderr, "Error: Too few or many arguments.\n");
        return;
    }
    
    char buffer[256];
    char *src = args[1];
    
    // פותחים את הקובץ לקריאה
    FILE *fptr1;
    fptr1 = fopen(src, "r");
    
    // אם לא ניתן לפתוח את הקובץ, מחזירים הודעת שגיאה
    if (fptr1 == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\"\n", src);
        return;
    }
    
    // קוראים את הקובץ שורה אחר שורה ומדפיסים למסך
    while (fgets(buffer, sizeof(buffer), fptr1) != NULL)
    {
        printf("%s", buffer);
    }
}

//פונקציה זו סופרת את מספר המילים או השורות בקובץ, תלוי באופציה שנבחרה (-l לשורות ו--w למילים).
void wordCount(char **args)
{
    int size = 0;
    // סופרים את מספר הארגומנטים
    while (args[size] != NULL)
    {
        size++;
    }

    // אם יש פחות מ-3 ארגומנטים או יותר, הפונקציה מחזירה הודעת שגיאה
    if (size != 3)
    {
        fprintf(stderr, "Error: Too few or too many arguments.\n");
        return;
    }
    
    char buffer[256];
    char *src = args[2];
    
    // פותחים את הקובץ לקריאה
    FILE *fptr1;
    int cont = 0;
    fptr1 = fopen(src, "r");
    
    // אם לא ניתן לפתוח את הקובץ, מחזירים הודעת שגיאה
    if (fptr1 == NULL)
    {
        fprintf(stderr, "Error: Unable to open file \"%s\"\n", src);
        return;
    }
    
    // סופרים שורות אם הבחירה היא -l
    if (strcmp(args[1], "-l") == 0)
    {
        while (fgets(buffer, sizeof(buffer), fptr1) != NULL)
        {
            cont++;
        }
    }
    // סופרים מילים אם הבחירה היא -w
    else if (strcmp(args[1], "-w") == 0)
    {
        while (fgets(buffer, sizeof(buffer), fptr1) != NULL)
        {
            char *token = strtok(buffer, "\n"); 
            while (token != NULL)
            {
                cont++;  // סופרים מילים
                token = strtok(NULL, "\n"); 
            }
        }
    }
    else
    {
        // אם לא נבחרה אפשרות חוקית, מחזירים הודעת שגיאה
        fprintf(stderr, "Error: you need to chose -l or -w\n");
        return;
    }
    
    // מדפיסים את התוצאה
    printf("%d\n", cont);
}



