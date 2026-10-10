#include <stdio.h>
#include <string.h>

struct rec {
    char d[20];
    char c[20];
    char s[20];
    char v;
    int ed;
};

struct act {
    int t;
    char d[20];
    char c[20];
    char s[20];
    char o;
    char n;
    char r[20];
};

struct rec att[1000];
struct act au[1000];
struct act us[1000];
struct act rs[1000];
int na = 0, nau = 0, nu = 0, nr = 0, q = 0;

int find(char *d, char *c, char *s)
{
    int i;
    for (i = 0; i < na; i++) {
        if (strcmp(att[i].d, d) == 0 && strcmp(att[i].c, c) == 0 && strcmp(att[i].s, s) == 0)
            return i;
    }
    return -1;
}

int domark(char *d, char *c, char *s, char *p)
{
    if (strcmp(p, "P") != 0 && strcmp(p, "A") != 0) {
        if (!q) printf("status must be P or A\n");
        return 0;
    }
    if (find(d, c, s) != -1) {
        if (!q) printf("already marked, use EDIT\n");
        return 0;
    }
    strcpy(att[na].d, d);
    strcpy(att[na].c, c);
    strcpy(att[na].s, s);
    att[na].v = p[0];
    att[na].ed = 0;
    na++;
    us[nu].t = 1;
    strcpy(us[nu].d, d);
    strcpy(us[nu].c, c);
    strcpy(us[nu].s, s);
    us[nu].o = ' ';
    us[nu].n = p[0];
    strcpy(us[nu].r, "-");
    nu++;
    nr = 0;
    return 1;
}

int doedit(char *d, char *c, char *s, char *p, char *r)
{
    int i, j, k;
    if (strcmp(p, "P") != 0 && strcmp(p, "A") != 0) {
        if (!q) printf("status must be P or A\n");
        return 0;
    }
    i = find(d, c, s);
    if (i == -1) {
        if (!q) printf("not found\n");
        return 0;
    }
    if (att[i].v == p[0]) {
        if (!q) printf("no change\n");
        return 0;
    }
    if (att[i].ed > 0) {
        k = -1;
        for (j = 0; j < nau; j++) {
            if (strcmp(au[j].d, d) == 0 && strcmp(au[j].c, c) == 0 && strcmp(au[j].s, s) == 0)
                k = j;
        }
        if (!q) {
            printf("conflict, keeping earlier\n");
            printf("earlier: AUDIT: %s %s %s %c->%c (%s)\n", au[k].d, au[k].c, au[k].s, au[k].o, au[k].n, au[k].r);
            printf("new: AUDIT: %s %s %s %c->%c (%s)\n", d, c, s, att[i].v, p[0], r);
        }
        return 0;
    }
    au[nau].t = 2;
    strcpy(au[nau].d, d);
    strcpy(au[nau].c, c);
    strcpy(au[nau].s, s);
    au[nau].o = att[i].v;
    au[nau].n = p[0];
    strcpy(au[nau].r, r);
    us[nu] = au[nau];
    nau++;
    nu++;
    att[i].v = p[0];
    att[i].ed++;
    nr = 0;
    return 1;
}

int doundo()
{
    struct act a;
    int i;
    if (nu == 0) {
        if (!q) printf("nothing to undo\n");
        return 0;
    }
    nu--;
    a = us[nu];
    if (a.t == 1) {
        na--;
    } else {
        i = find(a.d, a.c, a.s);
        att[i].v = a.o;
        att[i].ed--;
        nau--;
    }
    rs[nr] = a;
    nr++;
    if (!q) printf("undone\n");
    return 1;
}

int doredo()
{
    struct act a;
    int i;
    if (nr == 0) {
        if (!q) printf("nothing to redo\n");
        return 0;
    }
    nr--;
    a = rs[nr];
    if (a.t == 1) {
        strcpy(att[na].d, a.d);
        strcpy(att[na].c, a.c);
        strcpy(att[na].s, a.s);
        att[na].v = a.n;
        att[na].ed = 0;
        na++;
    } else {
        i = find(a.d, a.c, a.s);
        att[i].v = a.n;
        att[i].ed++;
        au[nau] = a;
        nau++;
    }
    us[nu] = a;
    nu++;
    if (!q) printf("redone\n");
    return 1;
}

void report(char *c)
{
    char st[100][20];
    int tot[100], pr[100];
    int ns = 0, i, j, f;
    for (i = 0; i < na; i++) {
        if (strcmp(att[i].c, c) != 0) continue;
        f = -1;
        for (j = 0; j < ns; j++) {
            if (strcmp(st[j], att[i].s) == 0) f = j;
        }
        if (f == -1) {
            strcpy(st[ns], att[i].s);
            tot[ns] = 0;
            pr[ns] = 0;
            f = ns;
            ns++;
        }
        tot[f]++;
        if (att[i].v == 'P') pr[f]++;
    }
    if (ns == 0) {
        printf("not found\n");
        return;
    }
    for (j = 0; j < ns; j++)
        printf("%s %d/%d %.2f%%\n", st[j], pr[j], tot[j], pr[j] * 100.0 / tot[j]);
    for (i = 0; i < nau; i++) {
        if (strcmp(au[i].c, c) == 0)
            printf("AUDIT: %s %s %s %c->%c (%s)\n", au[i].d, au[i].c, au[i].s, au[i].o, au[i].n, au[i].r);
    }
}

void history()
{
    int i;
    if (nau == 0) {
        printf("no history\n");
        return;
    }
    for (i = 0; i < nau; i++)
        printf("AUDIT: %s %s %s %c->%c (%s)\n", au[i].d, au[i].c, au[i].s, au[i].o, au[i].n, au[i].r);
}

void run(char *line)
{
    char cmd[20], a[20], b[20], c[20], d[20], e[20];
    int n, ok = 0;
    FILE *f;
    n = sscanf(line, "%19s %19s %19s %19s %19s %19s", cmd, a, b, c, d, e);
    if (n <= 0) return;
    if (strcmp(cmd, "MARK") == 0) {
        if (n != 5) {
            printf("wrong args\n");
            return;
        }
        ok = domark(a, b, c, d);
    } else if (strcmp(cmd, "EDIT") == 0) {
        if (n == 5) {
            printf("reason code missing\n");
            return;
        }
        if (n != 6) {
            printf("wrong args\n");
            return;
        }
        ok = doedit(a, b, c, d, e);
    } else if (strcmp(cmd, "REPORT") == 0) {
        if (n != 2) {
            printf("wrong args\n");
            return;
        }
        report(a);
    } else if (strcmp(cmd, "UNDO") == 0) {
        ok = doundo();
    } else if (strcmp(cmd, "REDO") == 0) {
        ok = doredo();
    } else if (strcmp(cmd, "HISTORY") == 0) {
        history();
    } else {
        printf("invalid command\n");
    }
    if (ok && q == 0) {
        f = fopen("log.txt", "a");
        if (f != NULL) {
            fprintf(f, "%s\n", line);
            fclose(f);
        }
    }
}

int main()
{
    char line[200];
    int i;
    FILE *f;
    f = fopen("log.txt", "r");
    if (f != NULL) {
        q = 1;
        while (fgets(line, 200, f) != NULL) {
            i = strlen(line);
            while (i > 0 && (line[i - 1] == '\n' || line[i - 1] == '\r')) {
                line[i - 1] = 0;
                i--;
            }
            run(line);
        }
        fclose(f);
        q = 0;
    }
    while (1) {
        if (fgets(line, 200, stdin) == NULL) break;
        i = strlen(line);
        while (i > 0 && (line[i - 1] == '\n' || line[i - 1] == '\r')) {
            line[i - 1] = 0;
            i--;
        }
        if (strcmp(line, "EXIT") == 0) break;
        run(line);
    }
    return 0;
}
