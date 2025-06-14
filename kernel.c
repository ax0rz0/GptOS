#include "keyboard.h"
#include "apps/calc.h"
#include "apps/notes.h"
#include "apps/extras.h"

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

void shell() {
    char input[128];
    while (1) {
        print("
GPTOS> ");
        keyboard_readline(input, 128);
        if (!strcmp(input, "help")) {
            print("help, calc, notes, extra, clear, about, reboot
");
        } else if (!strcmp(input, "about")) {
            print("GPTOS: Minimal OS example. By OpenAI + you!
");
        } else if (!strcmp(input, "clear")) {
            clear_screen();
        } else if (!strcmp(input, "calc")) {
            app_calc();
        } else if (!strcmp(input, "notes")) {
            app_notes();
        } else if (!strcmp(input, "extra")) {
            app_extras();
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