#include <stdio.h>
#include <string.h>
#include "command.h"
#include "console.h"

static void cmd_help_exec() {
    console_dev->puts("List of commands\r\n");
    const struct command * cmd = console_get_command_head();
    while (cmd) {
        if (strlen(cmd->description)) {
            uint8_t padding;
            console_dev->putc(' ');
            console_dev->puts(cmd->name);
            padding = 16 - strlen(cmd->name);
            while (padding--)
                console_dev->putc(' ');
            console_dev->puts(cmd->description);
            console_dev->puts("\r\n");
        }
        cmd = cmd->next;
    }
}

struct command cmd_help = {.next=  NULL,
                           .description= "List available commands and descriptions",
                           .name= "help",
                           .exec= cmd_help_exec };

__attribute__((constructor)) void cmd_help_init() {
    console_register_command(&cmd_help);
}
