#include <stdio.h>
#include <string.h>

/* one student's attendance for a course */
struct rec {
    char date[20];
    char course[20];
    char student[20];
    char status;
    int edits;
};

/* stores a change so it can be undone or redone */
struct act {
    int type;
    char date[20];
    char course[20];
    char student[20];
    char old;
    char new;
    char reason[20];
};

struct rec att[1000];
struct act au[1000];
struct act us[1000];
struct act rs[1000];
/* counts for attendance, edit history, undo, and redo */
int total = 0, audit = 0, nu = 0, redo = 0, quiet = 0;

/* find an attendance record using its date, course, and student */
int find(char *date, char *course, char *student)
{
    int index;
    for (index = 0; index < total; index++) {
        if (strcmp(att[index].date, date) == 0 && strcmp(att[index].course, course) == 0 && strcmp(att[index].student, student) == 0)
            return index;
    }
    return -1;
}

int domark(char *date, char *course, char *student, char *status)
{
    if (strcmp(status, "P") != 0 && strcmp(status, "A") != 0) {
        if (!quiet) printf("status must be P or A\n");
        return 0;
    }
    if (find(date, course, student) != -1) {
        if (!quiet) printf("already marked, use EDIT\n");
        return 0;
    }
    strcpy(att[total].date, date);
    strcpy(att[total].course, course);
    strcpy(att[total].student, student);
    att[total].status = status[0];
    att[total].edits = 0;
    total++;
    us[nu].type = 1;
    strcpy(us[nu].date, date);
    strcpy(us[nu].course, course);
    strcpy(us[nu].student, student);
    us[nu].old = ' ';
    us[nu].new = status[0];
    strcpy(us[nu].reason, "-");
    nu++;
    redo = 0;
    return 1;
}

int doedit(char *date, char *course, char *student, char *status, char *reason)
{
    int index, pos, found;
    if (strcmp(status, "P") != 0 && strcmp(status, "A") != 0) {
        if (!quiet) printf("status must be P or A\n");
        return 0;
    }
    index = find(date, course, student);
    if (index == -1) {
        if (!quiet) printf("not found\n");
        return 0;
    }
    if (att[index].status == status[0]) {
        if (!quiet) printf("no change\n");
        return 0;
    }
    if (att[index].edits > 0) {
        found = -1;
        for (pos = 0; pos < audit; pos++) {
            if (strcmp(au[pos].date, date) == 0 && strcmp(au[pos].course, course) == 0 && strcmp(au[pos].student, student) == 0)
                found = pos;
        }
        if (!quiet) {
            printf("conflict, keeping earlier\n");
            printf("earlier: AUDIT: %s %s %s %c->%c (%s)\n", au[found].date, au[found].course, au[found].student, au[found].old, au[found].new, au[found].reason);
            printf("new: AUDIT: %s %s %s %c->%c (%s)\n", date, course, student, att[index].status, status[0], reason);
        }
        return 0;
    }
    au[audit].type = 2;
    strcpy(au[audit].date, date);
    strcpy(au[audit].course, course);
    strcpy(au[audit].student, student);
    au[audit].old = att[index].status;
    au[audit].new = status[0];
    strcpy(au[audit].reason, reason);
    us[nu] = au[audit];
    audit++;
    nu++;
    att[index].status = status[0];
    att[index].edits++;
    redo = 0;
    return 1;
}

/* undo the most recent change */
int doundo()
{
    struct act action;
    int index;
    if (nu == 0) {
        if (!quiet) printf("nothing to undo\n");
        return 0;
    }
    nu--;
    action = us[nu];
    if (action.type == 1) {
        total--;
    } else {
        index = find(action.date, action.course, action.student);
        att[index].status = action.old;
        att[index].edits--;
        audit--;
    }
    rs[redo] = action;
    redo++;
    if (!quiet) printf("undone\n");
    return 1;
}

/* redo the last undone change */
int doredo()
{
    struct act action;
    int index;
    if (redo == 0) {
        if (!quiet) printf("nothing to redo\n");
        return 0;
    }
    redo--;
    action = rs[redo];
    if (action.type == 1) {
        strcpy(att[total].date, action.date);
        strcpy(att[total].course, action.course);
        strcpy(att[total].student, action.student);
        att[total].status = action.new;
        att[total].edits = 0;
        total++;
    } else {
        index = find(action.date, action.course, action.student);
        att[index].status = action.new;
        att[index].edits++;
        au[audit] = action;
        audit++;
    }
    us[nu] = action;
    nu++;
    if (!quiet) printf("redone\n");
    return 1;
}

void report(char *course)
{
    char students[100][20];
    int totals[100], present[100];
    int count = 0, index, pos, found;
    for (index = 0; index < total; index++) {
        if (strcmp(att[index].course, course) != 0) continue;
        found = -1;
        for (pos = 0; pos < count; pos++) {
            if (strcmp(students[pos], att[index].student) == 0) found = pos;
        }
        if (found == -1) {
            strcpy(students[count], att[index].student);
            totals[count] = 0;
            present[count] = 0;
            found = count;
            count++;
        }
        totals[found]++;
        if (att[index].status == 'P') present[found]++;
    }
    if (count == 0) {
        printf("not found\n");
        return;
    }
    for (pos = 0; pos < count; pos++)
        printf("%s %d/%d %.2f%%\n", students[pos], present[pos], totals[pos], present[pos] * 100.0 / totals[pos]);
    for (index = 0; index < audit; index++) {
        if (strcmp(au[index].course, course) == 0)
            printf("AUDIT: %s %s %s %c->%c (%s)\n", au[index].date, au[index].course, au[index].student, au[index].old, au[index].new, au[index].reason);
    }
}

void history()
{
    int index;
    if (audit == 0) {
        printf("no history\n");
        return;
    }
    for (index = 0; index < audit; index++)
        printf("AUDIT: %s %s %s %c->%c (%s)\n", au[index].date, au[index].course, au[index].student, au[index].old, au[index].new, au[index].reason);
}

void run(char *line)
{
    char command[20], date[20], course[20], student[20], status[20], reason[20];
    int count, done = 0;
    FILE *file;
    count = sscanf(line, "%19s %19s %19s %19s %19s %19s", command, date, course, student, status, reason);
    if (count <= 0) return;
    if (strcmp(command, "MARK") == 0) {
        if (count != 5) {
            printf("wrong args\n");
            return;
        }
        done = domark(date, course, student, status);
    } else if (strcmp(command, "EDIT") == 0) {
        if (count == 5) {
            printf("reason code missing\n");
            return;
        }
        if (count != 6) {
            printf("wrong args\n");
            return;
        }
        done = doedit(date, course, student, status, reason);
    } else if (strcmp(command, "REPORT") == 0) {
        if (count != 2) {
            printf("wrong args\n");
            return;
        }
        report(date);
    } else if (strcmp(command, "UNDO") == 0) {
        done = doundo();
    } else if (strcmp(command, "REDO") == 0) {
        done = doredo();
    } else if (strcmp(command, "HISTORY") == 0) {
        history();
    } else {
        printf("invalid command\n");
    }
    if (done && quiet == 0) {
        file = fopen("log.txt", "a");
        if (file != NULL) {
            fprintf(file, "%s\n", line);
            fclose(file);
        }
    }
}

int main()
{
    char line[200];
    int index;
    FILE *file;
    file = fopen("log.txt", "r");
    if (file != NULL) {
        quiet = 1;
        while (fgets(line, 200, file) != NULL) {
            index = strlen(line);
            while (index > 0 && (line[index - 1] == '\n' || line[index - 1] == '\r')) {
                line[index - 1] = 0;
                index--;
            }
            run(line);
        }
        fclose(file);
        quiet = 0;
    }
    while (1) {
        if (fgets(line, 200, stdin) == NULL) break;
        index = strlen(line);
        while (index > 0 && (line[index - 1] == '\n' || line[index - 1] == '\r')) {
            line[index - 1] = 0;
            index--;
        }
        if (strcmp(line, "EXIT") == 0) break;
        run(line);
    }
    return 0;
}
