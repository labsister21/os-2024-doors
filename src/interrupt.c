#include "header/cpu/interrupt.h"
#include "header/process/scheduler.h"

void activate_keyboard_interrupt(void)
{
    out(PIC1_DATA, in(PIC1_DATA) & ~(1 << IRQ_KEYBOARD));
}

void io_wait(void)
{
    out(0x80, 0);
}
void pic_ack(uint8_t irq)
{
    if (irq >= 8)
        out(PIC2_COMMAND, PIC_ACK);
    out(PIC1_COMMAND, PIC_ACK);
}
void pic_remap(void)
{
    // Starts the initialization sequence in cascade mode
    out(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    io_wait();
    out(PIC1_DATA, PIC1_OFFSET); // ICW2: Master PIC vector offset
    io_wait();
    out(PIC2_DATA, PIC2_OFFSET); // ICW2: Slave PIC vector offset
    io_wait();
    out(PIC1_DATA, 0b0100); // ICW3: tell Master PIC, slave PIC at IRQ2 (0000 0100)
    io_wait();
    out(PIC2_DATA, 0b0010); // ICW3: tell Slave PIC its cascade identity (0000 0010)
    io_wait();
    out(PIC1_DATA, ICW4_8086);
    io_wait();
    out(PIC2_DATA, ICW4_8086);
    io_wait();
    // Disable all interrupts
    out(PIC1_DATA, PIC_DISABLE_ALL_MASK);
    out(PIC2_DATA, PIC_DISABLE_ALL_MASK);
}

void main_interrupt_handler(struct InterruptFrame frame)
{
    switch (frame.int_number)
    {
    case (PIC1_OFFSET + IRQ_KEYBOARD):
        keyboard_isr();
        break;
    case (PIC1_OFFSET + IRQ_TIMER):
        struct Context ctx;
        ctx.eflags = frame.int_stack.eflags;
        ctx.eip = frame.int_stack.eip;
        ctx.cpu = frame.cpu;
        scheduler_save_context_to_current_running_pcb(ctx);
        scheduler_switch_to_next_process();
        break;
    case 0x30:
        syscall(frame);
        break;
    }
}

struct TSSEntry _interrupt_tss_entry = {
    .ss0 = GDT_KERNEL_DATA_SEGMENT_SELECTOR,
};

void set_tss_kernel_current_stack(void)
{
    uint32_t stack_ptr;
    // Reading base stack frame instead esp
    __asm__ volatile("mov %%ebp, %0" : "=r"(stack_ptr) : /* <Empty> */);
    // Add 8 because 4 for ret address and other 4 is for stack_ptr variable
    _interrupt_tss_entry.esp0 = stack_ptr + 8;
}

void syscall(struct InterruptFrame frame)
{
    switch (frame.cpu.general.eax)
    {
    case 0:
        *((int8_t *)frame.cpu.general.ecx) = read(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;

    case 1:
        *((int8_t *)frame.cpu.general.ecx) = read_directory(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;

    case 2:
        *((int8_t *)frame.cpu.general.ecx) = write(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;

    case 3:
        *((int8_t *)frame.cpu.general.ecx) = delete (*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;

    case 4:
        get_keyboard_buffer((char *)frame.cpu.general.ebx);
        break;
    case 5:
        put_char_color(frame.cpu.general.ebx, frame.cpu.general.ecx);
        break;
    case 6:
        puts(
            (char *)frame.cpu.general.ebx,
            frame.cpu.general.ecx,
            frame.cpu.general.edx); // Assuming puts() exist in kernel
        break;
    case 7:
        keyboard_state_activate();
        break;
    case 8:
        clear_screen();
        break;
    case 9:
        read_clusters((void *)frame.cpu.general.ebx, frame.cpu.general.ecx, (uint32_t)frame.cpu.general.edx);
        break;
    case 10:
        set_cursor_col(frame.cpu.general.ebx);
        break;
    case 11:
        *((int8_t *)frame.cpu.general.ecx) = search_file(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;
    case 12:
        *((uint32_t *)frame.cpu.general.ecx) = get_cluster_number(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;
    case 13:
        *((int8_t *)frame.cpu.general.edx) = copy_folder((uint32_t)frame.cpu.general.ebx, (uint32_t)frame.cpu.general.ecx);
        break;
    case 14:
        *((int8_t *)frame.cpu.general.edx) = copy_file(*(struct FAT32DriverRequest *)frame.cpu.general.ebx, *(struct FAT32DriverRequest *)frame.cpu.general.ecx);
        break;
    case 15:
        *((int8_t *)frame.cpu.general.ecx) = delete_folder_rec(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;
    case 16:
        move_screen((char)frame.cpu.general.ebx);
        break;
    case 17:
        *(bool *)frame.cpu.general.ebx = is_shift();
        break;
    case 18:
        *(bool *)frame.cpu.general.ebx = get_is_cursor_viewable();
        break;
    case 19:
        process_exit();
        break;
    case 20:
        *(bool *)frame.cpu.general.ecx = process_destroy(*(uint32_t *)frame.cpu.general.ebx);
        break;
    case 21:
        *(int32_t *)frame.cpu.general.ecx = process_create_user_process(*(struct FAT32DriverRequest *)frame.cpu.general.ebx);
        break;
    case 22:
        *(struct ProcessList *)frame.cpu.general.ebx = get_process_list();
        break;
    case 23:
        uint16_t **values = (uint16_t **)frame.cpu.general.ebx;
        read_rtc(values[0], values[1], values[2], values[3], values[4], values[5]);
        break;
    case 24:
        framebuffer_write((uint8_t)frame.cpu.general.ebx, (uint8_t)frame.cpu.general.ecx, (char)frame.cpu.general.edx, 0xA, 0);
        break;
    case 25:
        *(uint32_t *)frame.cpu.general.ebx = get_timestamp();
        break;
    default:
        break;
    }
}

void activate_timer_interrupt(void)
{
    __asm__ volatile("cli");
    // Setup how often PIT fire
    uint32_t pit_timer_counter_to_fire = PIT_TIMER_COUNTER;
    out(PIT_COMMAND_REGISTER_PIO, PIT_COMMAND_VALUE);
    out(PIT_CHANNEL_0_DATA_PIO, (uint8_t)(pit_timer_counter_to_fire & 0xFF));
    out(PIT_CHANNEL_0_DATA_PIO, (uint8_t)((pit_timer_counter_to_fire >> 8) & 0xFF));
    // Activate the interrupt
    out(PIC1_DATA, in(PIC1_DATA) & ~(1 << IRQ_TIMER));
}