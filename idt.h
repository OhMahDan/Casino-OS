#ifndef IDT_H
#define IDT_H

#include <stdint.h> // We need to use exact interger sizes like `uint16_t` and `uint32_t`

struct idt_entry {
    uint16_t lw_16bits;
    uint16_t sgmntSlctr_16bits;
    uint8_t zero_8bits;
    uint8_t flgs_8bits; 
    uint16_t uppr_16bits;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

#endif