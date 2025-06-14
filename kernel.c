#include "keyboard.h"
#include "api.h"
#include "apps/calc.h"
#include "apps/notes.h"
#include "apps/extras.h"
#include <string.h>

#define VGA_ADDRESS 0xb8000
#define MAX_ROWS 25
#define MAX_COLS 80

static int cursor = 0;
void clear_screen() {
    unsigned short *vga = (unsigned short *)VGA_ADDRESS;
    for (int i = 0; i < MAX_ROWS * MAX_COLS; i++) vga[i] = (0x07 << 8) | ' ';
    cursor = 0;
}
void putchar(char c) {
    unsigned short *vga = (unsigned short *)VGA_ADDRESS;
    if (c == '
') {
        cursor += MAX_COLS - (cursor % MAX_COLS);
    } else if (c == '\r') {
        cursor -= (cursor % MAX_COLS);
    } else {
        vga[cursor++] = (0x07 << 8) | c;
    }
    if (cursor >= MAX_ROWS * MAX_COLS) cursor = 0;
}
void print(const char *str) { while (*str) putchar(*str++); }

// ---- Simple RAM FILESYSTEM ----
#define MAX_FILES 16
#define MAX_FILENAME 12
#define MAX_FILE_SIZE 256
struct file {
    char name[MAX_FILENAME+1];
    char data[MAX_FILE_SIZE];
    int size;
};
struct file fs[MAX_FILES];

int fs_find(const char *name) {
    for (int i = 0; i < MAX_FILES; ++i)
        if (fs[i].name[0] && !strcmp(fs[i].name, name)) return i;
    return -1;
}
int fs_alloc() {
    for (int i = 0; i < MAX_FILES; ++i)
        if (!fs[i].name[0]) return i;
    return -1;
}
// Create or overwrite
int file_create(const char *filename, const char *text) {
    if (!filename || !text) return -1;
    int idx = fs_find(filename);
    if (idx == -1) idx = fs_alloc();
    if (idx == -1) return -2;
    strncpy(fs[idx].name, filename, MAX_FILENAME); fs[idx].name[MAX_FILENAME] = 0;
    strncpy(fs[idx].data, text, MAX_FILE_SIZE); fs[idx].data[MAX_FILE_SIZE-1] = 0;
    fs[idx].size = strlen(fs[idx].data);
    return 0;
}
int file_read(const char *filename, char *buf, int max) {
    int idx = fs_find(filename);
    if (idx == -1) return -1;
    int sz = fs[idx].size < max ? fs[idx].size : max-1;
    strncpy(buf, fs[idx].data, sz);
    buf[sz] = 0;
    return sz;
}
int file_delete(const char *filename) {
    int idx = fs_find(filename);
    if (idx == -1) return -1;
    fs[idx].name[0] = 0; fs[idx].data[0] = 0; fs[idx].size = 0;
    return 0;
}
int file_list(char files[][MAX_FILENAME+1], int max) {
    int n = 0;
    for (int i = 0; i < MAX_FILES && n < max; ++i) {
        if (fs[i].name[0]) {
            strncpy(files[n], fs[i].name, MAX_FILENAME);
            files[n][MAX_FILENAME] = 0;
            n++;
        }
    }
    return n;
}
int file_copy(const char *src, const char *dst) {
    int s = fs_find(src);
    if (s == -1) return -1;
    return file_create(dst, fs[s].data);
}

// ---- Kernel API for .prg apps ----
void k_print(const char *s) { print(s); }
char k_getchar() { char buf[2]; keyboard_readline(buf, 2); return buf[0]; }
void k_clear() { clear_screen(); }
int k_file_create(const char *filename, const char *text) { return file_create(filename, text); }
int k_file_read(const char *filename, char *buf, int max) { return file_read(filename, buf, max); }
int k_file_delete(const char *filename) { return file_delete(filename); }
int k_file_list(char files[][13], int max) { return file_list(files, max); }

struct API kernel_api = {
    k_print, k_getchar, k_clear,
    k_file_create, k_file_read, k_file_delete, k_file_list
};

typedef void (*prg_entry)(struct API *);

// .prg binaries (demo)
extern unsigned char hello_prg[], echo_prg[], savehello_prg[];
extern int hello_prg_len, echo_prg_len, savehello_prg_len;

struct {
    const char *name;
    unsigned char *data;
    int len;
} programs[] = {
    {"hello", hello_prg, 0},
    {"echo", echo_prg, 0},
    {"savehello", savehello_prg, 0},
    {NULL, NULL, 0}
};

void run_prg(const char *name) {
    for (int i = 0; programs[i].name; ++i) {
        if (!strcmp(name, programs[i].name)) {
            prg_entry entry = (prg_entry)(programs[i].data);
            entry(&kernel_api);
            return;
        }
    }
    print("No such .prg
");
}

// ---- CLI FILE COMMANDS ----
void cmd_ls() {
    char files[MAX_FILES][MAX_FILENAME+1];
    int n = file_list(files, MAX_FILES);
    if (n == 0) print("No files
");
    for (int i = 0; i < n; i++) { print(files[i]); print("
"); }
}
void cmd_cat(const char *name) {
    char buf[MAX_FILE_SIZE+1];
    int n = file_read(name, buf, MAX_FILE_SIZE);
    if (n < 0) print("No such file
");
    else print(buf);
    print("
");
}
void cmd_new(const char *name) {
    print("Type your text (end with ~ on a new line):
");
    char buf[MAX_FILE_SIZE];
    int i = 0;
    while (i < MAX_FILE_SIZE-1) {
        char line[64]; keyboard_readline(line, 64);
        if (line[0] == '~' && line[1] == 0) break;
        int l = strlen(line);
        if (i+l >= MAX_FILE_SIZE-1) break;
        strcpy(buf + i, line); i += l;
        buf[i++] = '
';
    }
    buf[i] = 0;
    file_create(name, buf);
    print("Saved
");
}
void cmd_del(const char *name) {
    if (file_delete(name) == 0) print("Deleted
");
    else print("No such file
");
}
void cmd_cp(const char *src, const char *dst) {
    if (file_copy(src, dst) == 0) print("Copied
");
    else print("Copy failed
");
}

// ---- SHELL ----
void shell() {
    char input[128];
    while (1) {
        print("
GPTOS> ");
        keyboard_readline(input, 128);
        if (!strcmp(input, "help")) {
            print("help, calc, notes, extra, clear, about, reboot
"
                  "ls, cat <file>, new <file>, del <file>, cp <a> <b>, run <prg>
");
        } else if (!strcmp(input, "about")) {
            print("GPTOS: Minimal OS with file system and .prg support.
");
        } else if (!strcmp(input, "clear")) {
            clear_screen();
        } else if (!strcmp(input, "calc")) {
            app_calc();
        } else if (!strcmp(input, "notes")) {
            app_notes();
        } else if (!strcmp(input, "extra")) {
            app_extras();
        } else if (!strcmp(input, "ls")) {
            cmd_ls();
        } else if (!strncmp(input, "cat ", 4)) {
            cmd_cat(input+4);
        } else if (!strncmp(input, "new ", 4)) {
            cmd_new(input+4);
        } else if (!strncmp(input, "del ", 4)) {
            cmd_del(input+4);
        } else if (!strncmp(input, "cp ", 3)) {
            char a[20], b[20];
            if (sscanf(input+3, "%s %s", a, b) == 2) cmd_cp(a, b);
            else print("Usage: cp <a> <b>
");
        } else if (!strncmp(input, "run ", 4)) {
            run_prg(input + 4);
        } else if (!strcmp(input, "reboot")) {
            print("Simulated reboot (reset in QEMU to try again).
");
        } else {
            print("Unknown command. Type 'help'.
");
        }
    }
}

void kmain() {
    clear_screen();
    print("Welcome to GPTOS!
Type 'help' for commands.
");
    shell();
    while(1);
}

// ---- DEMO .prg (as before) ----
unsigned char hello_prg[] = { 0x55, 0x89, 0xe5, 0x83, 0xec, 0x08, 0x8b, 0x45, 0x08, 0x8b, 0x40, 0x00, 0x68, 0x01, 0x00, 0x00, 0x00, 0x8b, 0x45, 0x08, 0x8b, 0x40, 0x04, 0xff, 0xd0, 0xc9, 0xc3, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x66, 0x72, 0x6f, 0x6d, 0x20, 0x50, 0x52, 0x47, 0x21, 0x0a, 0x00 };
unsigned char echo_prg[]  = { 0x55, 0x89, 0xe5, 0x8b, 0x45, 0x08, 0x8b, 0x40, 0x00, 0xff, 0xd0, 0xc9, 0xc3 };
unsigned char savehello_prg[] = { 0x55, 0x89, 0xe5, 0x8b, 0x45, 0x08, 0x68, 0x48, 0x69, 0x2e, 0x6c, 0x65, 0x74, 0x74, 0x65, 0x72, 0x00, 0x68, 0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x2c, 0x20, 0x4c, 0x65, 0x74, 0x74, 0x65, 0x72, 0x21, 0x00, 0x8b, 0x45, 0x08, 0x8b, 0x40, 0x0c, 0x8b, 0x45, 0x08, 0x8b, 0x40, 0x10, 0xff, 0xd0, 0x8b, 0x45, 0x08, 0x8b, 0x40, 0x08, 0x8b, 0x45, 0x08, 0x8b, 0x40, 0x14, 0xff, 0xd0, 0xc9, 0xc3 };
int hello_prg_len = sizeof(hello_prg);
int echo_prg_len  = sizeof(echo_prg);
int savehello_prg_len = sizeof(savehello_prg);