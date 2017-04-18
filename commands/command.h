#ifndef COMMANDS_H
#define COMMANDS_H

struct command {
    struct command * next;     // next element of linked list
    const char * description;  // description for ? help listing
    const char * name;         // name for envoking command
    void (* exec)();           // executing function
};

#endif