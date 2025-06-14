#include "../keyboard.h"
extern void print(const char *);

char notes[5][128];
int note_count = 0;

void app_notes() {
    print("Notes app!
1=Add, 2=View all: ");
    char buf[2]; keyboard_readline(buf, 2);
    if (buf[0] == '1') {
        if (note_count >= 5) { print("Max notes!
"); return; }
        print("Type note: ");
        keyboard_readline(notes[note_count], 128);
        note_count++;
        print("Saved!
");
    } else if (buf[0] == '2') {
        for (int i=0;i<note_count;i++) {
            print("Note "); putchar('1'+i); print(": "); print(notes[i]); print("
");
        }
    }
}