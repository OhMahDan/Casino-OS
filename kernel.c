#include "vga.h"

void kernel_main(void) {

    // Call terminal_clear
    terminal_clear();

    // Test terminal_scroll.
    for(int i = 0; i < 30; i++){
        terminal_writestring("i\n");
    }
    terminal_writestring("TEST");

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}