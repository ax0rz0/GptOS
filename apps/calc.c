#include "../keyboard.h"
extern void print(const char *);

void app_calc() {
    char buf[32];
    int a=0, b=0, res=0; char op=0;
    print("Enter calc (a+b, a-b, a*b, a/b): ");
    keyboard_readline(buf, 32);
    if (sscanf(buf, "%d%c%d", &a, &op, &b) == 3) {
        if (op=='+') res = a+b;
        else if (op=='-') res = a-b;
        else if (op=='*') res = a*b;
        else if (op=='/' && b) res = a/b;
        else print("Divide by zero!
");
        char out[32]; sprintf(out, "Result: %d
", res); print(out);
    } else print("Error.
");
}