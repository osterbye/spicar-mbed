#include "mbed.h"
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "console.h"

Serial * console_dev;

#define PROMPT_LENGTH 80
static char prompt[PROMPT_LENGTH] = "\0";
static const char path_delimiter = '/';
static const char delimiters[] = " ,\r\t"; // list of all recognized delimiters
const char newline[] = "\r\n";
static struct command * commands_head = NULL;
static struct token current_token;

static struct callback_data {
    void (*callback)(const struct token * tok);
    const char * hint;
} callback_data = {NULL, NULL};

#if __STDC_VERSION__ < 199901L

    #define restrict 
#endif

/* local functions declarations */
static void parse_line(char line[]);
static void echo_prompt(void);
static bool interpret_integer(const char * restrict str, uint16_t * num);
static void parse_token(const char str[], struct token * const tok);

void console_init(Serial * interface) {
    console_dev = interface;
}

void console_task(void) {
    
    if (console_dev->readable()) {
        char newchar = console_dev->getc();
        uint8_t length = strlen(prompt);
        // delete previous character for backspace and delete keys
        if (length > 0 && (newchar == '\b' || newchar == 0x7F)) {
            prompt[length - 1] = '\0';
        } else if (length < PROMPT_LENGTH - 1 && newchar != '\n') {
            prompt[length] = newchar;
            prompt[length+1] = '\0';
        }
        if (newchar == '\r') {
            console_dev->puts(newline);
            parse_line(prompt);
            if (callback_data.hint) {
                console_dev->puts(callback_data.hint);
                console_dev->puts(newline);
            }
            prompt[0] = '\0';
        }
        echo_prompt();
    }
}
static void parse_line(char line[]) {
    char * str;
    str = strtok (line, delimiters);
    while(str) {
        parse_token(str, &current_token);

        if (callback_data.callback) {
            void (*callback)(const struct token * tok) = callback_data.callback;
            callback_data.callback = NULL;
            callback_data.hint = NULL;
            callback(&current_token);
        }
        else if (current_token.type == TOKEN_INVALID) {
            console_dev->puts("Invalid command ");
            console_dev->puts(str);
            console_dev->puts(newline);
        } else if (current_token.type == TOKEN_COMMAND) {
            current_token.u.cmd->exec();        
        } else if (current_token.type == TOKEN_INTEGER) {
        }
        str = strtok (NULL, delimiters);
    }
}

void console_register_command(struct command * cmd) {
    static struct command ** tail = &commands_head;
    *tail = cmd;
    tail = &(cmd->next);
}

void console_set_token_callback(void (*callback)(const struct token * tok), const char * hint) {
    callback_data.hint = hint;
    callback_data.callback = callback;
}

const struct command * console_get_command_head() {
    return commands_head;
}

static void echo_prompt(void) {
    console_dev->puts("\r> ");
    console_dev->puts(prompt);
}

static bool interpret_integer(const char * restrict str, uint16_t * num) {
    char * end = (char *) str;
    char ** restrict endp = &end;
    if (str[0] == '0') {
        if (str[1] == 'x' || str[1] == 'X')
            *num = strtol(str + 2, endp, 16);
        else if (str[1] == 'b' || str[1] == 'B')
            *num = strtol(str + 2, endp, 2);
        else
            *num = strtol(str, endp, 10);
    } else
        *num = strtol(str, endp, 10);
    return end != str;
}

static void parse_token(const char str[], struct token * const tok) {
    const struct command * cmd = commands_head;
    tok->type = TOKEN_INVALID;
    tok->str = str;
    while (cmd) {
        if (strcmp(str, cmd->name) == 0)
        {
            tok->type = TOKEN_COMMAND;
            tok->u.cmd = cmd;
            break;
        }
        cmd = cmd->next;
    }
    if (tok->type == TOKEN_INVALID && interpret_integer((const char *) str, &(tok->u.num))) {
        tok->type = TOKEN_INTEGER;
    }
    if (tok->type == TOKEN_INVALID && strlen(str) > 0 && str[0] == path_delimiter) {
        tok->type = TOKEN_PATH;
        tok->u.path = str;
    }
}
