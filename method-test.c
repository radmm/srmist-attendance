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

    if (find(d,c,s)!=1){
        if (!q)
    }
