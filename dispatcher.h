#ifndef DISPATCHER_H_
#define DISPATCHER_H_
#include <mbed.h>

/**
 * Vehicle subsystems are numerous and too many for each to have its own
 * thread. Instead they are to be executed inside single thread and wrapped
 * inside a "routine". Dispatcher can handle arbitrary number of registered
 * routines and call them as needed. Each routine can have init() function
 * callback (called once on registration) and tick() function callback called
 * repeatedly with defined period. Tick() function is never preemptied so it
 * should not block for long periods. This means the dispatching mechanism is
 * a cooperative multitasking system.
 * 
 * The dispatcher also has a mailbox for receiving commands to execute from
 * other threads. Each received command is dispatched to appropriate routine
 * which can obtain results and return them to caller.
 * 
 */

#define MAX_ROUTINES             100
#define MAX_CONCURRENT_COMMANDS   16

enum commandType {
    CMD_TYPE_GET,
    CMD_TYPE_SET,
    CMD_TYPE_SAVE_CONFIG,
    CMD_TYPE_REVERT_CONFIG,
    CMD_TYPE_FOTA_START,
    CMD_TYPE_HELP,
};

enum commandSource {
    CMD_FROM_CLOUD,
    CMD_FROM_CONSOLE,
};

typedef struct {
    enum commandType type;
    enum commandSource source;
    //enum commandStatus status;
    const char * treePath;
} Command;

enum nodeType {
    TYPE_UINT32,
    TYPE_FLOAT,
    TYPE_BOOL
};

typedef struct {
    const char* name;
    enum nodeType type1;
    char* (*getter)();
    void (*setter)(const char*);
} Node;

typedef struct routine {
    const char* name;
    void (*init)();
    void (*tick)();
    uint32_t period_ms;
} Routine;


class Dispatcher
{
public:
    Dispatcher(Serial *pc);
    ~Dispatcher();

    void stop_thread();
    void addRoutine(Routine * routine);
    void executeCommand(enum commandType type, enum commandSource source, const char * treePath);
    void loop();

private:
    bool abort;
    Serial *pc;
    Mail<Command, MAX_CONCURRENT_COMMANDS> cmdMailbox;
    EventQueue routineScheduler;
};

#endif // DISPATCHER_H_
