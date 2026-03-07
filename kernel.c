void kernel_main(void) {

    // Declaring a pointer that points to the start of the VGA buffer.
    unsigned short* terminal_buffer = (unsigned short*)0xB8000;

    // Constructing a 16-bit value and putting it into the first index of our pointer.
    unsigned short val = (0x0A << 8) | 'X';
    terminal_buffer[0] = val;

    // A well-designed kernel should never attempt to return.
    while(1);
}