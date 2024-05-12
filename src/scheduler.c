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
    curr->context.cpu = ctx.cpu;
    curr->context.eflags = ctx.eflags;
    curr->context.eip = ctx.eip;
    curr->context.page_directory_virtual_addr = ctx.page_directory_virtual_addr;
}

void scheduler_switch_to_next_process(void)
{
    struct ProcessControlBlock * curr = process_get_current_running_pcb_pointer();
    int idx = 0, next = -1, cnt = 0;
    if (curr != NULL)
        idx = (curr->metadata.pid + 1) % PROCESS_COUNT_MAX;
    while (cnt < PROCESS_COUNT_MAX)
    {
        if (_process_list[idx].metadata.state == READY)
        {
            next = idx;
            break;
        }
        idx = (idx + 1) % PROCESS_COUNT_MAX;
        cnt++;
    }
    if (next != -1)
    {
        // set current process to ready
        if (curr != NULL)
            curr->metadata.state = READY;

        // set next process to running
        struct ProcessControlBlock * next_process = &_process_list[idx];
        next_process->metadata.state = RUNNING;
        
        // do context switching
        paging_use_page_directory(next_process->context.page_directory_virtual_addr);
        process_context_switch(next_process->context);
    }
}