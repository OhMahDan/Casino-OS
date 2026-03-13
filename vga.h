#ifndef VGA_H
#define VGA_H

#include <stddef.h>

void terminal_clear(void);
void terminal_setcolor(unsigned char color);
void terminal_putchar(char c);
void terminal_writestring(const char* data);

#endif