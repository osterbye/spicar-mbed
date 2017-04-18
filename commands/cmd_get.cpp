#include <stdio.h>
#include <string.h>
#include "command.h"
#include "console.h"
#include "dispatcher.h"

static void cmd_get_exec();
static void cmd_get_gotPath(const struct token * tok);

static void cmd_get_exec() {
    console_set_token_callback(&cmd_get_gotPath, "Specify path in Vehicle Interaction Tree");
}

static void cmd_get_gotPath(const struct token * tok) {
    if (tok->type == TOKEN_PATH) {
        dispatcher_execute(CMD_TYPE_GET, CMD_FROM_CONSOLE, tok->u.path);
    } else {
        console_dev->puts("Command argument is not path\r\n");
    }
}

struct command cmd_get = {.next=  NULL,
                          .description= "Fetch value from Vehicle Interaction Tree: get <path>",
                          .name= "get",
                          .exec= cmd_get_exec };

__attribute__((constructor)) void cmd_get_init() {
    console_register_command(&cmd_get);
}
