#pragma once
struct API {
    void (*print)(const char *);
    char (*getchar)();
    void (*clear_screen)();
    int  (*file_create)(const char *filename, const char *text);
    int  (*file_read)(const char *filename, char *buf, int max);
    int  (*file_delete)(const char *filename);
    int  (*file_list)(char files[][13], int max);
};