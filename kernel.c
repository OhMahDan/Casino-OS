// Constants
// Terminal related constants.
static const int VGA_COLS = 80, VGA_ROWS = 25;

// Static global variables
// Terminal tracking variables.
static int terminal_row = 0, terminal_column = 0;               // X + Y position of terminal Cursor
static unsigned short* terminal_buffer = (unsigned short*)0xB8000;     //Starting position of VGA terminal
static unsigned char terminal_color = 0x07;

// Function to print characters to the terminal.
void terminal_putchar(char c){
    int index = (terminal_row * VGA_COLS) + terminal_column;
    unsigned short val = (terminal_color << 8) | c;
    terminal_buffer[index] = val;
    terminal_column++;
    
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

void kernel_main(void) {

    // Call terminal_clear
    terminal_clear();

    // Test terminal_putchar.
    terminal_putchar('C');
    terminal_putchar('a');
    terminal_putchar('s');
    terminal_putchar('i');
    terminal_putchar('n');
    terminal_putchar('o');

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}