#include <stdio.h>
#include <string.h>
#include "command.h"
#include "console.h"
#include "dispatcher.h"

static void cmd_set_exec();
static void cmd_set_gotPath(const struct token * tok);
static void cmd_set_gotValue(const struct token * tok);

static void cmd_set_exec() {
    console_set_token_callback(&cmd_set_gotPath, "Specify path in Vehicle Interaction Tree");
}

static void cmd_set_gotPath(const struct token * tok) {
    if (tok->type == TOKEN_PATH) {
        console_set_token_callback(&cmd_set_gotValue, "Specify new value");
    } else {
        console_dev->puts("Command argument #1 is not path\r\n");
    }
}

static void cmd_set_gotValue(const struct token * tok) { 
    if (tok->type == TOKEN_INTEGER) {
        dispatcher_execute(CMD_TYPE_SET, CMD_FROM_CONSOLE, tok->u.path);
    } else {
        console_dev->puts("Command argument #2 is not a value\r\n");
    }
}

struct command cmd_set = {.next=  NULL,
                          .description= "Modify value in Vehicle Interaction Tree: set <path> <value>",
                          .name= "set",
                          .exec= cmd_set_exec };

__attribute__((constructor)) void cmd_set_init() {
    console_register_command(&cmd_set);
}
