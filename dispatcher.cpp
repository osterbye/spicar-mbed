#include "dispatcher.h"
#include "mbed.h"
#include "mbed_events.h"

static Mail<Command, MAX_CONCURRENT_COMMANDS> cmdMailbox;
static EventQueue routineScheduler(MAX_ROUTINES*EVENTS_EVENT_SIZE);

void dispatcher_register(Routine * routine) {
    int id;
    if (routine->init != NULL) {
        routine->init();
    }
    if(routine->tick != NULL) {
        if (routine->period_ms > 0) {
            id = routineScheduler.call_every(routine->period_ms, routine->tick);
        } else {
            id = routineScheduler.call(routine->tick);
        }
        if (id == 0) {
            error("Not enough memory to add routine to scheduler");
        }
    }
}

void dispatcher_execute(enum commandType type, enum commandSource source, const char * treePath) {
    Command *cmd = cmdMailbox.alloc();
    cmd->type = type;
    cmd->source = source;
    cmd->treePath = treePath;
    if (osStatus status = cmdMailbox.put(cmd)) {
        error("Unsuccessful command enqueueing (status %d)", status);
    }
}

void dispatcher_task() {
    while (1) {
        /* execute any commands pending in mailbox */
        osEvent evt = cmdMailbox.get(0);
        if (evt.status == osEventMail) {
            Command *cmd = (Command*)evt.value.p;
            /* TODO: execute by dispatching to appropriate task action (get/set/config) */

            /* TODO: remove following debug output: */
            const char * commandTypes[] = {
                "CMD_TYPE_GET",
                "CMD_TYPE_SET",
                "CMD_TYPE_SAVE_CONFIG",
                "CMD_TYPE_REVERT_CONFIG",
                "CMD_TYPE_FOTA_START",
                "CMD_TYPE_HELP",
            };
            const char *  commandSources[] = {
                "CMD_FROM_CLOUD",
                "CMD_FROM_CONSOLE",
            };    
            printf("Executing %s, path %s, source %s\r\n", commandTypes[cmd->type], cmd->treePath, commandSources[cmd->source]);

            cmdMailbox.free(cmd);
        }
        /* execute scheduled routines */
        routineScheduler.dispatch(0);
        wait_ms(5);
    }
}
