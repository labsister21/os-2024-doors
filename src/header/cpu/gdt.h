#ifndef _GDT_H
#define _GDT_H

#include <stdint.h>

// Some GDT Constant
#define GDT_MAX_ENTRY_COUNT 32
/**
 * As kernel SegmentDescriptor for code located at index 1 in GDT, 
 * segment selector is sizeof(SegmentDescriptor) * 1 = 0x8
*/ 
#define GDT_KERNEL_CODE_SEGMENT_SELECTOR 0x8
#define GDT_KERNEL_DATA_SEGMENT_SELECTOR 0x10

extern struct GDTR _gdt_gdtr;

/**
 * Segment Descriptor storing system segment information.
 * Struct defined exactly as Intel Manual Segment Descriptor definition (Figure 3-8 Segment Descriptor).
 * Manual can be downloaded at www.intel.com/content/www/us/en/architecture-and-technology/64-ia-32-architectures-software-developer-vol-3a-part-1-manual.html/ 
 *
 * @param segment_low  16-bit lower-bit segment limit (LIMIT)
 * @param base_low     16-bit lower-bit base address (BASE)
 * @param base_mid     8-bit middle-bit base address (BASE)
 * @param type_bit     4-bit contain type flags (TYPE)
 * @param non_system   1-bit contain system (S)
 * @param privilege_level 2-bit contain descriptor privilege level (DPL)
 * @param segment_present 1-bit contain segment present (P)
 * @param segment_high 4-bit higher-bit segment limit (LIMIT)
 * @param available_bits 1-bit available for use (AVL)
 * @param code_segment 1-bit 64-bit code segment (L)
 * @param default_ops_size 1-bit contain default operation size (D/B)
 * @param granularity 1-bit contain granularity flag (G)
 * @param base_high 8-bit higher base address (B)
 */
struct SegmentDescriptor {
    // First 32-bit
    uint16_t segment_low;
    uint16_t base_low;

    // Next 16-bit (Bit 32 to 47)
    uint8_t base_mid;
    uint8_t type_bit   : 4;
    uint8_t non_system : 1;
    uint8_t privilege_level: 2;
    uint8_t segment_present: 1;

    // Next 16-bit (Bit 48 to 63)
    uint8_t segment_high: 4;
    uint8_t available_bits: 1;
    uint8_t code_segment: 1;
    uint8_t default_ops_size: 1;
    uint8_t granularity: 1;
    uint8_t base_high;
} __attribute__((packed));

/**
 * Global Descriptor Table containing list of segment descriptor. One GDT already defined in memory.c.
 * More details at https://wiki.osdev.org/GDT_Tutorial
 * @param table Fixed-width array of SegmentDescriptor with size GDT_MAX_ENTRY_COUNT
 */
struct GlobalDescriptorTable {
    struct SegmentDescriptor table[GDT_MAX_ENTRY_COUNT];
} __attribute__((packed));

/**
 * GDTR, carrying information where's the GDT located and GDT size.
 * Global kernel variable defined at memory.c.
 * 
 * @param size    Global Descriptor Table size, use sizeof operator
 * @param address GDT address, GDT should already defined properly
 */
struct GDTR {
    uint16_t                     size;
    struct GlobalDescriptorTable *address;
} __attribute__((packed));

#endif
