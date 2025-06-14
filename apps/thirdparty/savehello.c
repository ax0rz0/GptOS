#include "../../api.h"
void main(struct API *api) {
    api->file_create("hi.letter", "Hello, Letter!");
    char buf[256];
    if (api->file_read("hi.letter", buf, 256) > 0)
        api->print(buf);
}