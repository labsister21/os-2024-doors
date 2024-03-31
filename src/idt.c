#include "header/cpu/idt.h"

struct InterruptDescriptorTable interrupt_descriptor_table = {
    .table = {}
};

struct IDTR _idt_idtr = {
    .size = sizeof(interrupt_descriptor_table) - 1,
    .address = &(interrupt_descriptor_table)
}