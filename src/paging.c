#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "header/memory/paging.h"
#include "header/stdlib/string.h"
#include "header/process/process.h"

__attribute__((aligned(0x1000))) struct PageDirectory _paging_kernel_page_directory = {
    .table = {
        [0] = {
            .flag.present_bit = 1,
            .flag.write_bit = 1,
            .flag.use_pagesize_4_mb = 1,
            .lower_address = 0,
        },
        [0x300] = {
            .flag.present_bit = 1,
            .flag.write_bit = 1,
            .flag.use_pagesize_4_mb = 1,
            .lower_address = 0,
        },
    }};

static struct PageManagerState page_manager_state = {
    .page_frame_map = {
        [0] = true,
        [1 ... PAGE_FRAME_MAX_COUNT - 1] = false},
    .free_page_frame_count = PAGE_FRAME_MAX_COUNT - 1};

void update_page_directory_entry(
    struct PageDirectory *page_dir,
    void *physical_addr,
    void *virtual_addr,
    struct PageDirectoryEntryFlag flag)
{
    uint32_t page_index = ((uint32_t)virtual_addr >> 22) & 0x3FF;
    page_dir->table[page_index].flag = flag;
    page_dir->table[page_index].lower_address = ((uint32_t)physical_addr >> 22) & 0x3FF;
    flush_single_tlb(virtual_addr);
}

void flush_single_tlb(void *virtual_addr)
{
    asm volatile("invlpg (%0)" : /* <Empty> */ : "b"(virtual_addr) : "memory");
}

/* --- Memory Management --- */
bool paging_allocate_check(uint32_t amount)
{
    // Check whether requested amount is available
    // Calculate the number of page frames required to accommodate the requested amount of memory
    uint32_t required_page_frames = (amount + PAGE_FRAME_SIZE - 1) / PAGE_FRAME_SIZE;

    // Check if the required number of page frames is available
    return page_manager_state.free_page_frame_count >= required_page_frames;
}

bool paging_allocate_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr)
{
    /*
     *Find free physical frame and map virtual frame into it
     * - Find free physical frame in page_manager_state.page_frame_map[] using any strategies
     * - Mark page_manager_state.page_frame_map[]
     * - Update page directory with user flags:
     *     > present bit    true
     *     > write bit      true
     *     > user bit       true
     *     > pagesize 4 mb  true
     */
    // Iterate over the page frame map to find a free frame
    for (size_t i = 0; i < PAGE_FRAME_MAX_COUNT; ++i)
    {
        if (!page_manager_state.page_frame_map[i])
        {
            // Found a free frame, mark it as used
            page_manager_state.page_frame_map[i] = true;
            page_manager_state.free_page_frame_count--;

            // Calculate the physical address
            void *physical_addr = (void *)(i * PAGE_FRAME_SIZE);

            struct PageDirectoryEntryFlag flag =
                {
                    .present_bit = 1,
                    .write_bit = 1,
                    .user_supervisor = 1,
                    .use_pagesize_4_mb = 1};

            // Update page directory entry
            update_page_directory_entry(page_dir, physical_addr, virtual_addr, flag);

            // Successfully allocated the page frame
            return true;
        }
    }

    // No free frame available
    return false;
}

bool paging_free_user_page_frame(struct PageDirectory *page_dir, void *virtual_addr)
{
    /*
     * Deallocate a physical frame from respective virtual address
     * - Use the page_dir.table values to check mapped physical frame
     * - Remove the entry by setting it into 0
     */
    uint32_t page_table_index = ((uint32_t)virtual_addr >> 22) & 0x3FF;

    // Check if the page table entry is valid and the page frame is allocated
    if (page_dir->table[page_table_index].flag.present_bit)
    {
        // Mark the page frame as free in the page frame map
        page_manager_state.page_frame_map[(page_dir->table[page_table_index].lower_address)] = false;
        // Increase the count of free page frames
        page_manager_state.free_page_frame_count++;
        // Clear the present bit to mark the page as not present
        // page_dir->table[page_table_index].flag.present_bit = 0;
        // page_dir->table[page_table_index].higher_address = 0;
        // page_dir->table[page_table_index].lower_address = 0;
        // page_dir->table[page_table_index].flag.write_bit = 0;
        // page_dir->table[page_table_index].flag.user_supervisor = 0;
        flush_single_tlb(virtual_addr);
        memset(&(page_dir->table[page_table_index]), 0, sizeof(struct PageDirectoryEntry));
        // Invalidate TLB entry for the virtual address
        return true; // Deallocation succeeded
    }
    else
    {
        return false; // Page frame was not allocated
    }
}

__attribute__((aligned(0x1000))) static struct PageDirectory page_directory_list[PAGING_DIRECTORY_TABLE_MAX_COUNT] = {0};

static struct
{
    bool page_directory_used[PAGING_DIRECTORY_TABLE_MAX_COUNT];
} page_directory_manager = {
    .page_directory_used = {false},
};

struct PageDirectory *paging_create_new_page_directory(void)
{
    /*
     * TODO: Get & initialize empty page directory from page_directory_list
     * - Iterate page_directory_list[] & get unused page directory
     * - Mark selected page directory as used
     * - Create new page directory entry for kernel higher half with flag:
     *     > present bit    true
     *     > write bit      true
     *     > pagesize 4 mb  true
     *     > lower address  0
     * - Set page_directory.table[0x300] with kernel page directory entry
     * - Return the page directory address
     */
    for (int i = 0; i < PAGING_DIRECTORY_TABLE_MAX_COUNT; i++)
    {
        if (!page_directory_manager.page_directory_used[i])
        {
            page_directory_manager.page_directory_used[i] = true;
            page_directory_list[i].table[0].flag.present_bit = true;
            page_directory_list[i].table[0].flag.write_bit = true;
            page_directory_list[i].table[0].flag.use_pagesize_4_mb = true;
            page_directory_list[i].table[0].lower_address = 0;
            page_directory_list[i].table[0x300] = _paging_kernel_page_directory.table[0x300];
            return &(page_directory_list[i]);
        }
    }
    return NULL;
}

bool paging_free_page_directory(struct PageDirectory *page_dir)
{
    /**
     * TODO: Iterate & clear page directory values
     * - Iterate page_directory_list[] & check &page_directory_list[] == page_dir
     * - If matches, mark the page directory as unusued and clear all page directory entry
     * - Return true
     */
    for (int i = 0; i < PAGING_DIRECTORY_TABLE_MAX_COUNT; i++)
    {
        if (&page_directory_list[i] == page_dir)
        {
            page_directory_manager.page_directory_used[i] = false;
            for (uint32_t j = 0; j < 1024; j++)
            {
                if (j == 0x300)
                {
                    memset(&(page_dir->table[j]), 0, sizeof(struct PageDirectoryEntry));
                }
                else if (page_dir->table[j].flag.present_bit)
                {
                    paging_free_user_page_frame(page_dir, (void *)((uint32_t)j << 22));
                }
            }
            return true;
        }
    }
    return false;
}

struct PageDirectory *paging_get_current_page_directory_addr(void)
{
    uint32_t current_page_directory_phys_addr;
    __asm__ volatile("mov %%cr3, %0" : "=r"(current_page_directory_phys_addr) : /* <Empty> */);
    uint32_t virtual_addr_page_dir = current_page_directory_phys_addr + KERNEL_VIRTUAL_ADDRESS_BASE;
    return (struct PageDirectory *)virtual_addr_page_dir;
}

void paging_use_page_directory(struct PageDirectory *page_dir_virtual_addr)
{
    uint32_t physical_addr_page_dir = (uint32_t)page_dir_virtual_addr;
    // Additional layer of check & mistake safety net
    if ((uint32_t)page_dir_virtual_addr > KERNEL_VIRTUAL_ADDRESS_BASE)
        physical_addr_page_dir -= KERNEL_VIRTUAL_ADDRESS_BASE;
    __asm__ volatile("mov %0, %%cr3" : /* <Empty> */ : "r"(physical_addr_page_dir) : "memory");
}