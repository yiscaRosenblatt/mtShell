# מגדיר את הקומפיילר שישמש לקימפול הקבצים
CC = gcc

# מגדיר דגלים  שישמשו בקימפול:
# -Wall  : מציג אזהרות במהלך הקימפול
# -g     : מוסיף מידע לניפוי שגיאות (debugging)
FLAGS = -Wall -g


# מטרה  שמריצה את התוכנית אחרי שהיא נבנית
run: clean myShell
	./myShell

# מטרה שמריצה את Valgrind כדי לבדוק אם יש זליגות זיכרון
leak:
	valgrind --leak-check=full ./myShell

# בניית הקובץ הבינארי myShell מתוך קובצי האובייקט myShell.o ו-myFunctionsShell.o
myShell: myShell.o myFunctionsShell.o
	$(CC) $(FLAGS) -o myShell myShell.o myFunctionsShell.o

# קומפילציה של הקובץ myShell.c ליצירת קובץ אובייקט myShell.o
myShell.o: myShell.c myShell.h
	$(CC) $(FLAGS) -c myShell.c

# קומפילציה של הקובץ myFunctionsShell.c ליצירת קובץ אובייקט myFunctionsShell.o
myFunctionsShell.o: myFunctionsShell.c myFunctionsShell.h
	$(CC) $(FLAGS) -c myFunctionsShell.c

# מטרה לניקוי כל הקבצים המתקבלים לאחר הקימפול
clean:
	rm -f *.o *.out myShell
