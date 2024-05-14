#include "header/process/scheduler.h"

void scheduler_init()
{
    activate_timer_interrupt();
    struct ProcessControlBlock *curr = process_get_current_running_pcb_pointer();
    if (curr != NULL)
    {
        curr->metadata.state = RUNNING;
    }
}

void scheduler_save_context_to_current_running_pcb(struct Context ctx)
{
    struct ProcessControlBlock *curr = process_get_current_running_pcb_pointer();
    if (curr != NULL)
    {
        curr->context.cpu = ctx.cpu;
        curr->context.eip = ctx.eip;
        curr->context.eflags = ctx.eflags;
    }
}

// assume that there's must be a process
void scheduler_switch_to_next_process(void)
{
    struct ProcessControlBlock *curr = process_get_current_running_pcb_pointer();
    int idx = 0, cnt = 0;

    // stop current running process and save context
    if (curr != NULL)
    {
        idx = (curr->metadata.pid + 1) % PROCESS_COUNT_MAX;
        curr->metadata.state = READY;
    }

    while (cnt < PROCESS_COUNT_MAX)
    {
        if (_process_list[idx].metadata.state == READY)
            break;
        idx = (idx + 1) % PROCESS_COUNT_MAX;
        cnt++;
    }
    struct ProcessControlBlock *next_process = &_process_list[idx];
    next_process->metadata.state = RUNNING;

    // do context switching
    paging_use_page_directory(next_process->context.page_directory_virtual_addr);
    process_context_switch(next_process->context);
}