#include <stddef.h>

// Constants
// Terminal related constants.
static const int VGA_COLS = 80, VGA_ROWS = 25;

// Static global variables
// Terminal tracking variables.
static int terminal_row = 0, terminal_column = 0;               // X + Y position of terminal Cursor
static unsigned short* terminal_buffer = (unsigned short*)0xB8000;     //Starting position of VGA terminal
static unsigned char terminal_color = 0x07;

// Function: Next line on terminal
void terminal_nextLine(){
    terminal_column = 0;
    terminal_row++;
}

// Function to print characters to the terminal.
void terminal_putchar(char c){

    // If newline, next terminal line.
    if(c == '\n'){
        terminal_nextLine();
        return;
    }

    // Determine VGA index and insert.
    int index = (terminal_row * VGA_COLS) + terminal_column;
    unsigned short val = (terminal_color << 8) | c;
    terminal_buffer[index] = val;
    terminal_column++;

    // If end of line, next terminal line.
    if(terminal_column == VGA_COLS){
        terminal_nextLine();
    }
}

// Function to clear the terminal of all characters.
void terminal_clear(void) {
    
    // Constructing a 16-bit value to represent a clear space.
    unsigned short val = (terminal_color << 8) | ' ';

    // Loop through every position in the VGA buffer and assign the blank space to it.
    for(int loc = 0; loc < (VGA_COLS * VGA_ROWS); loc++) terminal_buffer[loc] = val;

    // Reset cursor state.
    terminal_row = 0;
    terminal_column = 0;
}

// Function: Find a string length.
size_t strnlen(const char* str){
    size_t i = 0;
    while(str[i] != '\0'){
        i++;
    }
    return i;
}

void terminal_writestring(const char* data) {
    int len = strnlen(data);
    for(int i = 0; i < len; i++){
        terminal_putchar(data[i]);
    }
}

void kernel_main(void) {

    // Call terminal_clear
    terminal_clear();

    // Test terminal_writestring.
    terminal_writestring("Casino v0.1\nMoto: I'm going to build my own OS with blackjack and roulette.\n\nBy Daniel Grabowski");

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}