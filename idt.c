#include "idt.h"

struct idt_entry idt[256];
struct idt_ptr idtr;