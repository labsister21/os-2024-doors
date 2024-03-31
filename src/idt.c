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
    struct IDTGate *idt_entry = &(_idt_idtr.address->table[int_vector]);
    uint32_t handler_addr = (uint32_t)handler_address;
    idt_entry->offset_low = (uint16_t)handler_addr;
    idt_entry->segment = gdt_seg_selector;
    idt_entry->_reserved = 0;
    idt_entry->_r_bit_1 = INTERRUPT_GATE_R_BIT_1;
    idt_entry->_r_bit_2 = INTERRUPT_GATE_R_BIT_2;
    idt_entry->gate_32 = 1;
    idt_entry->_r_bit_3 = INTERRUPT_GATE_R_BIT_3;
    idt_entry->privilege_level = privilege;
    idt_entry->segment_present = 1;
    idt_entry->offset_high = (uint16_t)(handler_addr >> 16);
}


/**
 * Set IDT with proper values and load with lidt
 */
void initialize_idt(void)
{
    
}