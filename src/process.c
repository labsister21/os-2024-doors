#include "header/process/process.h"
#include "header/memory/paging.h"
#include "header/stdlib/string.h"
#include "header/cpu/gdt.h"

struct ProcessControlBlock _process_list[PROCESS_COUNT_MAX];

static struct ProcessManagerState process_manager_state = {
    .active_process_count = 0,
    .pid_used = {false}};

int32_t process_create_user_process(struct FAT32DriverRequest request)
{
    int32_t retcode = PROCESS_CREATE_SUCCESS;
    if (process_manager_state.active_process_count >= PROCESS_COUNT_MAX)
    {
        retcode = PROCESS_CREATE_FAIL_MAX_PROCESS_EXCEEDED;
        goto exit_cleanup;
    }

    // Ensure entrypoint is not located at kernel's section at higher half
    if ((uint32_t)request.buf >= KERNEL_VIRTUAL_ADDRESS_BASE)
    {
        retcode = PROCESS_CREATE_FAIL_INVALID_ENTRYPOINT;
        goto exit_cleanup;
    }

    // Check whether memory is enough for the executable and additional frame for user stack
    // uint32_t page_frame_count_needed = ceil_div(request.buffer_size + PAGE_FRAME_SIZE, PAGE_FRAME_SIZE);
    uint32_t required_page_frames = (request.buffer_size + PAGE_FRAME_SIZE - 1) / PAGE_FRAME_SIZE;
    if (!paging_allocate_check(request.buffer_size) || required_page_frames > PROCESS_PAGE_FRAME_COUNT_MAX)
    {
        retcode = PROCESS_CREATE_FAIL_NOT_ENOUGH_MEMORY;
        goto exit_cleanup;
    }

    // create new virtual memory
    struct PageDirectory *new_page_dir = paging_create_new_page_directory();
    if (new_page_dir == NULL)
    {
        retcode = PROCESS_CREATE_FAIL_NOT_ENOUGH_MEMORY;
        goto exit_cleanup;
    }

    // change page directory
    struct PageDirectory *curr_dir = paging_get_current_page_directory_addr();
    paging_use_page_directory(new_page_dir);

    // paging_allocate_user_page_frame(new_page_dir, request.buf);
    for (uint32_t i = 0; i < required_page_frames; i++)
    {
        paging_allocate_user_page_frame(new_page_dir, request.buf + i * PAGE_FRAME_SIZE);
    }

    // read the request on FAT32
    int8_t code = read(request);
    if (code != 0)
    {
        paging_use_page_directory(curr_dir);
        retcode = PROCESS_CREATE_FAIL_FS_READ_FAILURE;
        goto exit_cleanup;
    }

    // process PCB
    int32_t p_index = process_list_get_inactive_index();
    struct ProcessControlBlock *new_pcb = &(_process_list[p_index]);
    process_manager_state.active_process_count++;
    process_manager_state.pid_used[p_index] = true;

    // setup memory
    for (uint32_t i = 0; i < required_page_frames; i++)
    {
        new_pcb->memory.virtual_addr_used[i] = request.buf + i * PAGE_FRAME_SIZE;
    }
    new_pcb->memory.page_frame_used_count = required_page_frames;

    // setup context
    new_pcb->context.eflags |= CPU_EFLAGS_BASE_FLAG | CPU_EFLAGS_FLAG_INTERRUPT_ENABLE;
    new_pcb->context.page_directory_virtual_addr = new_page_dir;
    new_pcb->context.cpu.segment.ds = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.segment.es = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.segment.fs = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.segment.gs = GDT_USER_DATA_SEGMENT_SELECTOR;
    new_pcb->context.cpu.stack.esp = new_page_dir->table[0].lower_address * PAGE_FRAME_SIZE;

    // setup metadata
    new_pcb->metadata.pid = p_index;
    new_pcb->metadata.state = READY;

    paging_use_page_directory(new_page_dir);
exit_cleanup:
    return retcode;
}

int32_t process_list_get_inactive_index()
{
    for (int i = 0; i < PROCESS_COUNT_MAX; i++)
    {
        if (!process_manager_state.pid_used[i])
        {
            return i;
        }
    }
    return -1;
}

struct ProcessControlBlock *process_get_current_running_pcb_pointer(void)
{
    for (int i = 0; i < PROCESS_COUNT_MAX; i++)
    {
        if (process_manager_state.pid_used[i])
        {
            if (_process_list[i].metadata.state == RUNNING)
            {
                return &_process_list[i];
            }
        }
    }
    return NULL;
}

bool process_destroy(uint32_t pid)
{
    // TODO: implement process destroy
    if (pid < PROCESS_COUNT_MAX)
        return false;
    if (!process_manager_state.pid_used[pid])
        return false;

    // stop the process
    _process_list[pid].metadata.state = BLOCKED;

    // release page directory
    bool check = paging_free_page_directory(_process_list[pid].context.page_directory_virtual_addr);
    if (!check)
        return check;

    // release PCB
    memset(&(_process_list[pid]), 0, sizeof(struct ProcessControlBlock));

    // set manager state for pid to false
    process_manager_state.pid_used[pid] = false;
    process_manager_state.active_process_count--;
    return true;
}