#include "../keyboard.h"
extern void print(const char *);

void app_extras() {
    print("Extras: 1=Echo, 2=Static clock, 3=Fun fact
Choose: ");
    char buf[2]; keyboard_readline(buf, 2);
    if (buf[0] == '1') {
        print("Say something: ");
        char msg[64]; keyboard_readline(msg, 64); print("Echo: "); print(msg); print("
");
    } else if (buf[0] == '2') {
        print("12:34:56 (static, set by code!)
");
    } else if (buf[0] == '3') {
        print("Fun fact: GPTOS can boot on real hardware!
");
    }
}