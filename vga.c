#include "vga.h"
#include "string.h"

// Constants
// Terminal related constants.
static const int VGA_COLS = 80, VGA_ROWS = 25;
static const int VGA_COLOR_RED = 4, VGA_COLOR_GOLD = 14, VGA_COLOR_BLACK = 0;

// Static global variables
// Terminal tracking variables.
static int terminal_row = 0, terminal_column = 0;               // X + Y position of terminal Cursor
static unsigned short* terminal_buffer = (unsigned short*)0xB8000;     //Starting position of VGA terminal
static unsigned char terminal_color = 0x07;

//Function: Shift text up for scrolling.
static void terminal_scroll(){
    if(terminal_row >= VGA_ROWS) {
        for(int row = 1; row < VGA_ROWS; row++){
            for(int col = 0; col < VGA_COLS; col++){
                terminal_buffer[(row-1) * VGA_COLS + col] = terminal_buffer[row * VGA_COLS + col];
            }
        }
        for(int i = 0; i < VGA_COLS; i++){
            terminal_buffer[(VGA_ROWS - 1) * VGA_COLS + i] = (terminal_color << 8) | ' ';
        }
        terminal_row--;
    }
}

//Function: Set the terminal color.
void terminal_setcolor(unsigned char color){
    terminal_color = color;
}

// Function: Next line on terminal
static void terminal_nextLine(){
    terminal_column = 0;
    terminal_row++;
}

// Function: Print a character to the terminal.
void terminal_putchar(char c){

    // If newline, next terminal line.
    if(c == '\n'){
        terminal_nextLine();
        terminal_scroll();
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

    // If end of terminal, shift text up.
    terminal_scroll();
}

// Function: Clear the terminal of all characters.
void terminal_clear(void) {
    
    // Constructing a 16-bit value to represent a clear space.
    unsigned short val = (terminal_color << 8) | ' ';

    // Loop through every position in the VGA buffer and assign the blank space to it.
    for(int loc = 0; loc < (VGA_COLS * VGA_ROWS); loc++) terminal_buffer[loc] = val;

    // Reset cursor state.
    terminal_row = 0;
    terminal_column = 0;
}

// Function: Write a string to the terminal.
void terminal_writestring(const char* data) {
    int len = strlen(data);
    for(int i = 0; i < len; i++){
        terminal_putchar(data[i]);
    }
}