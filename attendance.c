#include <stdio.h>
#include <string.h>

struct rec {
    char date[20];
    char course[40];
    char student[40];
    char val;
};

struct change {
    char date[20];
    char course[40];
    char student[40];
    char oldv;
    char newv;
    char reason[40];
};

struct rec r[1000];
struct change a[2000];
struct change u[2000];
struct change d[2000];
int n, h, un, dn;

int find(char *date, char *course, char *student)
{
    int i;
    for (i = 0; i < n; i++)
        if (!strcmp(r[i].date, date) &&
            !strcmp(r[i].course, course) &&
            !strcmp(r[i].student, student)) {
            return i;
        }
    return -1;
}

void save(void)
{
    FILE *f;
    int i;
    f = fopen("attendance.txt", "w");
    if (f) {
        for (i = 0; i < n; i++)
                fprintf(f, "%s %s %s %c\n", r[i].date, r[i].course,
                    r[i].student, r[i].val);
        fclose(f);
    }
    f = fopen("audit.txt", "w");
    if (f) {
        for (i = 0; i < h; i++)
                fprintf(f, "%s %s %s %c %c %s\n",
                    a[i].date, a[i].course, a[i].student,
                    a[i].oldv, a[i].newv, a[i].reason);
        fclose(f);
    }
}

void load(void)
{
    FILE *f;
    f = fopen("attendance.txt", "r");
    if (f) {
        while (n < 1000 &&
               fscanf(f, "%19s %39s %39s %c", r[n].date,
                      r[n].course, r[n].student, &r[n].val) == 4) {
            if (r[n].val == 'P' || r[n].val == 'A') {
                n++;
            }
        }
        fclose(f);
    }
    f = fopen("audit.txt", "r");
    if (f) {
         while (h < 2000 &&
             fscanf(f, "%19s %39s %39s %c %c %39s",
                 a[h].date, a[h].course, a[h].student,
                 &a[h].oldv, &a[h].newv, a[h].reason) == 6) {
             h++;
         }
        fclose(f);
    }
}

int valid_value(char *s)
{
    return strlen(s) == 1 && (s[0] == 'P' || s[0] == 'A');
}

void mark(char w[10][100], int argc)
{
    if (argc != 5 || strlen(w[1]) > 19 || strlen(w[2]) > 39 ||
        strlen(w[3]) > 39 || !valid_value(w[4])) {
        puts("invalid args");
        return;
    }

    if (find(w[1], w[2], w[3]) >= 0) {
        puts("already exists");
        return;
    }
    if (n == 1000) {
        puts("limit reached");
        return;
    }

    strcpy(r[n].date, w[1]);
    strcpy(r[n].course, w[2]);
    strcpy(r[n].student, w[3]);
    r[n].val = w[4][0];
    n++;

    save();
    puts("marked");
}

void edit(char w[10][100], int argc)
{
    struct change z;
    int x, i;

    if (argc == 5) {
        puts("reason required");
        return;
    }
    if (argc != 6 || strlen(w[1]) > 19 || strlen(w[2]) > 39 ||
        strlen(w[3]) > 39 || !valid_value(w[4]) || strlen(w[5]) > 39) {
        puts("invalid args");
        return;
    }

    x = find(w[1], w[2], w[3]);
    if (x < 0) {
        puts("not found");
        return;
    }
    if (h == 2000) {
        puts("history full");
        return;
    }

    for (i = h - 1; i >= 0; i--) {
        if (!strcmp(a[i].date, w[1]) &&
            !strcmp(a[i].course, w[2]) &&
            !strcmp(a[i].student, w[3])) {
            if (a[i].newv != r[x].val) {
                puts("conflict warning");
            }
            break;
        }
    }

    strcpy(z.date, r[x].date);
    strcpy(z.course, r[x].course);
    strcpy(z.student, r[x].student);
    strcpy(z.reason, w[5]);
    z.oldv = r[x].val;
    z.newv = w[4][0];

    r[x].val = z.newv;
    a[h++] = z;
    u[un++] = z;
    dn = 0;

    save();
    puts("edited");
}

void report(char w[10][100], int argc)
{
    char names[1000][40];
    int present[1000], total[1000];
    int i, j, count;

    if (argc != 2 || strlen(w[1]) > 39) {
        puts("invalid args");
        return;
    }

    count = 0;
    for (i = 0; i < n; i++) {
        if (strcmp(r[i].course, w[1]) == 0) {
            for (j = 0; j < count; j++) {
                if (strcmp(names[j], r[i].student) == 0) {
                    break;
                }
            }
            if (j == count) {
                strcpy(names[count], r[i].student);
                present[count] = 0;
                total[count] = 0;
                count++;
            }
            total[j]++;
            if (r[i].val == 'P') {
                present[j]++;
            }
        }
    }

    if (count == 0) {
        puts("not found");
        return;
    }
    for (i = 0; i < count; i++) {
        printf("%s %d %d %.2f%%\n", names[i], present[i], total[i],
               100.0 * present[i] / total[i]);
    }
}

void move_change(int redo, int argc)
{
    struct change z;
    int x;

    if (argc != 1) {
        puts("invalid args");
        return;
    }
    if ((!redo && un == 0) || (redo && dn == 0)) {
        puts(redo ? "nothing to redo" : "nothing to undo");
        return;
    }

    if (redo) {
        z = d[--dn];
    } else {
        z = u[--un];
    }

    x = find(z.date, z.course, z.student);
    if (x < 0) {
        puts("not found");
        return;
    }
    r[x].val = redo ? z.newv : z.oldv;

    if (redo) {
        u[un++] = z;
    } else {
        d[dn++] = z;
    }

    save();
    puts(redo ? "redone" : "undone");
}

void history(int argc)
{
    int i;

    if (argc != 1) {
        puts("invalid args");
        return;
    }
    if (h == 0) {
        puts("no history");
        return;
    }

    for (i = 0; i < h; i++) {
        printf("%s %s %s %c -> %c %s\n", a[i].date, a[i].course,
               a[i].student, a[i].oldv, a[i].newv, a[i].reason);
    }
}

int main(void)
{
    char line[512];
    char w[10][100];
    int argc;

    load();
    while (scanf(" %511[^\n]", line) == 1) {
        argc = sscanf(line,
                      "%99s %99s %99s %99s %99s %99s %99s %99s %99s %99s",
                      w[0], w[1], w[2], w[3], w[4],
                      w[5], w[6], w[7], w[8], w[9]);

        if (strcmp(w[0], "MARK") == 0) {
            mark(w, argc);
        } else if (strcmp(w[0], "EDIT") == 0) {
            edit(w, argc);
        } else if (strcmp(w[0], "REPORT") == 0) {
            report(w, argc);
        } else if (strcmp(w[0], "UNDO") == 0) {
            move_change(0, argc);
        } else if (strcmp(w[0], "REDO") == 0) {
            move_change(1, argc);
        } else if (strcmp(w[0], "HISTORY") == 0) {
            history(argc);
        } else {
            puts("invalid command");
        }
    }

    save();
    return 0;
}
