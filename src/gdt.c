#include "header/cpu/gdt.h"

/**
 * global_descriptor_table, predefined GDT.
 * Initial SegmentDescriptor already set properly according to Intel Manual & OSDev.
 * Table entry : [{Null Descriptor}, {Kernel Code}, {Kernel Data (variable, etc)}, ...].
 */
struct GlobalDescriptorTable global_descriptor_table = {
    .table = {
        {
            .segment_low = 0,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0,
            .non_system = 0,
            .privilege_level = 0,
            .segment_present = 0,
            .segment_high = 0,
            .code_segment = 0,
            .default_ops_size = 0,
            .granularity = 0,
            .base_high = 0,
        },
        {
            // TODO : Implement
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0xA,
            .non_system = 1,
            .privilege_level = 0,
            .segment_present = 1,
            .segment_high = 0xF,
            .code_segment = 0,
            .default_ops_size = 1,
            .granularity = 1,
            .base_high = 0,
        },
        {
            // TODO : Implement
            .segment_low = 0xFFFF,
            .base_low = 0,
            .base_mid = 0,
            .type_bit = 0x2,
            .non_system = 1,
            .privilege_level = 0,
            .segment_present = 1,
            .segment_high = 0xF,
            .code_segment = 0,
            .default_ops_size = 1,
            .granularity = 1,
            .base_high = 0,
        },
        {/* TODO: User   Code Descriptor */
            .segment_low       = 0xFFFF,
            .base_low          = 0,
            .base_mid          = 0,
            .type_bit          = 0xA,
            .non_system        = 1,
            .privilege_level   = 0x3,
            .segment_present   = 1,
            .segment_high      = 0xF,
            .code_segment      = 0,
            .default_ops_size  = 1,
            .granularity       = 1,
            .base_high         = 0,
        },
        {/* TODO: User   Data Descriptor */
            .segment_low       = 0xFFFF,
            .base_low          = 0,
            .base_mid          = 0,
            .type_bit          = 0x2,
            .non_system        = 1,
            .privilege_level   = 0x3,
            .segment_present   = 1,
            .segment_high      = 0xF,
            .code_segment      = 0,
            .default_ops_size  = 1,
            .granularity       = 1,
            .base_high         = 0,
            
        },
        {
            .segment_low       = sizeof(struct TSSEntry),
            .base_low          = 0,
            .base_mid          = 0,
            .type_bit          = 0x9,
            .non_system        = 0,    // S bit
            .privilege_level   = 0,    // DPL
            .segment_present   = 1,    // P bit
            .segment_high      = (sizeof(struct TSSEntry) & (0xF << 16)) >> 16,
            .code_segment      = 0,    // L bit
            .default_ops_size  = 1,    // D/B bit
            .granularity       = 0,    // G bit
            .base_high         = 0,
        },
        {0}}};

/**
 * _gdt_gdtr, predefined system GDTR.
 * GDT pointed by this variable is already set to point global_descriptor_table above.
 * From: https://wiki.osdev.org/Global_Descriptor_Table, GDTR.size is GDT size minus 1.
 */
struct GDTR _gdt_gdtr = {
    .size = sizeof(global_descriptor_table) - 1,
    .address = &(global_descriptor_table)
    // TODO : Implement, this GDTR will point to global_descriptor_table.
    //        Use sizeof operator
};

void gdt_install_tss(void) {
    uint32_t base = (uint32_t) &_interrupt_tss_entry;
    global_descriptor_table.table[5].base_high = (base & (0xFF << 24)) >> 24;
    global_descriptor_table.table[5].base_mid  = (base & (0xFF << 16)) >> 16;
    global_descriptor_table.table[5].base_low  = base & 0xFFFF;
}