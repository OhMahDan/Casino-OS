// Constants
static const int VGA_COLS = 80;
static const int VGA_ROWS = 25;

// Function to clear the terminal of all characters.
void terminal_clear(void) {

    // Declaring a pointer that points to the start of the VGA buffer.
    unsigned short* terminal_buffer = (unsigned short*)0xB8000;

    // Constructing a 16-bit value to represent a clear space.
    unsigned short val = (0x07 << 8) | ' ';

    // Loop through every position in the VGA buffer and assign the blank space to it.
    for(int loc = 0; loc < (VGA_COLS * VGA_ROWS); loc++) terminal_buffer[loc] = val;
}

void kernel_main(void) {

    // Call terminal_clear
    terminal_clear();

    // DO NOT REMOVE
    // A well-designed kernel should never attempt to return.
    while(1);
}