#ifndef _CONSOLE_H
#define _CONSOLE_H
#include <stdint.h>

#include "command.h"
#include "mbed.h"

extern const char newline[];

enum token_type {
    TOKEN_COMMAND,
    TOKEN_INTEGER,
    TOKEN_PATH,
    TOKEN_INVALID,
};

struct token {
    enum token_type type;
    const char * str;
    union {
        const struct command * cmd;
        uint16_t num;
        const char * path;
    } u;
};

extern Serial * console_dev;

void console_init(Serial * interface);
void console_task(void);
void console_register_command(struct command * cmd);
void console_set_token_callback(void (*callback)(const struct token * tok), const char * hint);
const struct command * console_get_command_head();

#endif /* header guard */