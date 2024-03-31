#include "header/cpu/idt.h"

struct InterruptDescriptorTable interrupt_descriptor_table = {
    .table = {}};

struct IDTR _idt_idtr = {
    .size = sizeof(interrupt_descriptor_table) - 1,
    .address = &(interrupt_descriptor_table)};

/**
 * Set IDTGate with proper interrupt handler values.
 * Will directly edit global IDT variable and set values properly
 * 
 * @param int_vector       Interrupt vector to handle
 * @param handler_address  Interrupt handler address
 * @param gdt_seg_selector GDT segment selector, for kernel use GDT_KERNEL_CODE_SEGMENT_SELECTOR
 * @param privilege        Descriptor privilege level
 */

void set_interrupt_gate(uint8_t int_vector, void *handler_address, uint16_t gdt_seg_selector, uint8_t privilege)
{
    _idt_idtr.address->table[int_vector].segment = gdt_seg_selector;
    _idt_idtr.address->table[int_vector].privilege_level = privilege;
    
}


/**
 * Set IDT with proper values and load with lidt
 */
void initialize_idt(void)
{
}