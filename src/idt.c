#include "header/cpu/idt.h"
#include "header/cpu/gdt.h"

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
  struct IDTGate *idt_int_gate = &interrupt_descriptor_table.table[int_vector];
  // TODO : Set handler offset, privilege & segment
  // Use &-bitmask, bitshift, and casting for offset
  // Target system 32-bit and flag this as valid interrupt gate
  uint32_t handler_addr = (uint32_t)handler_address;
  idt_int_gate->offset_low = (uint16_t)handler_addr;
  idt_int_gate->offset_high = (uint16_t)(handler_addr >> 16);
  idt_int_gate->privilege_level = privilege;
  idt_int_gate->segment = gdt_seg_selector;
  idt_int_gate->_reserved = 0;

  idt_int_gate->_r_bit_1 = INTERRUPT_GATE_R_BIT_1;
  idt_int_gate->_r_bit_2 = INTERRUPT_GATE_R_BIT_2;
  idt_int_gate->_r_bit_3 = INTERRUPT_GATE_R_BIT_3;
  idt_int_gate->gate_32 = 1;
  idt_int_gate->valid_bit = 1;
}

/**
 * Set IDT with proper values and load with lidt
 */
void initialize_idt(void)
{
  /*
   * TODO:
   * Iterate all isr_stub_table,
   * Set all IDT entry with set_interrupt_gate()
   * with following values:
   * Vector: i
   * Handler Address: isr_stub_table[i]
   * Segment: GDT_KERNEL_CODE_SEGMENT_SELECTOR
   * Privilege: 0
   */
  for (int i = 0; i < ISR_STUB_TABLE_LIMIT; i++) {
    set_interrupt_gate(i, isr_stub_table[i], GDT_KERNEL_CODE_SEGMENT_SELECTOR, 0);
  } 

  __asm__ volatile("lidt %0" : : "m"(_idt_idtr));
  __asm__ volatile("sti");
}