#include "keyboard.h"

#define PORT 0x60

unsigned char keyboard_map[] = {
    0,27,'1','2','3','4','5','6','7','8','9','0','-','=', '\b',
    '	','q','w','e','r','t','y','u','i','o','p','[',']','
',0,
    'a','s','d','f','g','h','j','k','l',';',\'','`',0,'\',
    'z','x','c','v','b','n','m',',','.','/',0,'*',0,' ',0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

static inline unsigned char inb(unsigned short port) {
    unsigned char result;
    __asm__ volatile ("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

extern void putchar(char c); // Use kernel's putchar

void keyboard_readline(char *buf, int max) {
    int i = 0;
    while (i < max - 1) {
        unsigned char scancode = inb(PORT);
        if (scancode & 0x80) continue; // ignore key releases
        char c = keyboard_map[scancode];
        if (c == '
') break;
        if (c == '\b' && i > 0) {
            i--; putchar('\b'); putchar(' '); putchar('\b');
            continue;
        }
        if (c && c >= 32 && c <= 126) {
            buf[i++] = c;
            putchar(c);
        }
        // simple debounce
        for (volatile int d = 0; d < 20000; d++);
    }
    buf[i] = '\0';
    putchar('
');
}