# SRM IST - Coding Challenge 
## Student attendance tracker


- a command-line attendance tracker written in C
- supports marking attendance,
- editing a mark with a reason, viewing course reports and edit history, and
undoing or redoing changes.

## build and run

```sh
gcc -Wall -Wextra -o attendance attendance.c

./attendance
```

Enter one command per line. Type `EXIT` or send end-of-file (Ctrl-D on Linux or
macOS) to quit.

## demo commands

Run these commands in a fresh directory to try marking attendance, editing a
mark, viewing a report and history, then undoing and redoing the edit:

```sh
./attendance <<'EOF'
MARK 2026-10-10 CS101 Alice P
MARK 2026-10-10 CS101 Bob A
EDIT 2026-10-10 CS101 Alice A corrected
REPORT CS101
HISTORY
UNDO
REPORT CS101
REDO
REPORT CS101
EXIT
EOF
```

command formats:

```text
MARK <date> <course> <student> <P|A>
EDIT <date> <course> <student> <P|A> <reason>
REPORT <course>
UNDO
REDO
HISTORY
EXIT
```

The report shows each student's present count and the course edit audit:

```text
Alice 0/1 0.00%
Bob 0/1 0.00%
AUDIT: 2026-10-10 CS101 Alice P->A (corrected)
AUDIT: 2026-10-10 CS101 Alice P->A (corrected)
undone
Alice 1/1 100.00%
Bob 0/1 0.00%
redone
Alice 0/1 0.00%
Bob 0/1 0.00%
AUDIT: 2026-10-10 CS101 Alice P->A (corrected)
```

successful attendance changes and undo/redo operations are recorded in
`log.txt` and replayed when the program starts.
