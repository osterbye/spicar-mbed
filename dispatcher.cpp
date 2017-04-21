#include "dispatcher.h"
#include "mbed.h"
#include "mbed_events.h"

Dispatcher::Dispatcher(Serial *pc) :
    abort(false), pc(pc), routineScheduler(MAX_ROUTINES * EVENTS_EVENT_SIZE) {
}

Dispatcher::~Dispatcher() {
}

void Dispatcher::stop_thread() {
    abort = true;
}

void Dispatcher::addRoutine(Routine * routine) {
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

void Dispatcher::executeCommand(enum commandType type, enum commandSource source, const char * treePath) {
    Command *cmd = cmdMailbox.alloc();
    cmd->type = type;
    cmd->source = source;
    cmd->treePath = treePath;
    if (osStatus status = cmdMailbox.put(cmd)) {
        error("Unsuccessful command enqueueing (status %d)", status);
    }
}

void Dispatcher::loop() {
    while (!abort) {
        /* execute any commands pending in mailbox */
        osEvent evt = cmdMailbox.get(0);
        if (evt.status == osEventMail) {
            Command *cmd = (Command*)evt.value.p;
            /* TODO: execute by dispatching to appropriate task action (get/set/config) */

            /* TODO: remove following debug output: */
            const char * commandTypes[] = {
                "GET",
                "SET",
                "SAVE_CONFIG",
                "REVERT_CONFIG",
                "FOTA_START",
                "HELP",
            };
            const char *  commandSources[] = {
                "CLOUD",
                "CONSOLE",
            };    
            pc->printf("\r\nExec %s on %s from %s\r\n", commandTypes[cmd->type], cmd->treePath, commandSources[cmd->source]);

            cmdMailbox.free(cmd);
        }
        /* execute scheduled routines */
        routineScheduler.dispatch(0);
        wait_ms(5);
    }
}
