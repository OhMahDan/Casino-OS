## 2.1 The Display Board (Terminal Driver)
Right now, if you want to print "Casino OS", you would have to manually calculate the index for every single letter. This is unsustainable. We need an abstraction layer: a **Terminal Driver.**
### The Mental Model
The terminal driver acts as the "Dealer" for the screen. User programs will ask the dealer to "print this string." The Dealer handles the underlying math: tracking where the cursor currently is, wrapping text to the next line when it hits the right edge, and clearing the table when it's full.
### The Method: Managing State
To do this, your C kernel needs to "remember" where the cursor is. The screen is a grid of *x* (columns) and *y* (rows).
- *x* goes from 0 to 79.
- *y* goes from 0 to 24.
- The formula to convert an (x, y) coordinate into a flat array index is: `index = y * x + x`
### Architectural Comparison: State Management
Before we write any code, we must decide how to store the cursor's current position globally so our functions can access it.

| **Option**                                           | Pros                                                                    | Cons                                                                                  | Recommendation                                                  |
| ---------------------------------------------------- | ----------------------------------------------------------------------- | ------------------------------------------------------------------------------------- | --------------------------------------------------------------- |
| *Global Primitives*<br>`size_t row;`<br>`size_t col` | Easiest to write immediately.                                           | Clutters the global namespace. Doesn't scale well if you want multiple windows later. | Not recommended for a serious OS.                               |
| *Global Struct*<br>`struct Terminal {...}`           | Encapsulates all terminal-related data cleanly. Easy to pass functions. | Requires slightly more C syntax to setup.                                             | **Recommended.** Fits a high standard for scalable, clean code. |
### Clearing the Table
Before we can print strings, we must be able to wipe away any text currently stored in the terminal.
"Clearing" the screen simply means looping through every single one of the 2,000 indexes in the VGA buffer and replacing whatever is there with a "Blank Space" character (`' '`) and a default color (e.g., Light Gray on Black, which is 0x07)
### Pitfalls:
- **Off-by-one errors:** The grid is 80x25. That is exactly 2,000 slots. If your loop goes to 2,001, you will write outside the VGA buffer, potentially corrupting kernel memory.
- **Hardcoding Magic Numbers:** Do not use 80 and 25 directly in your loops. Define them as constants so your code is readable.
### Actions: Expanding `kernel.c`
It's time to remove the hardcoded 'X' logic from our `kernel_main` function and prepare to write standard driver functions.
1. **Define Constants:** At the top of `kernel.c`, define the VGA dimensions:
2. **Write `terminal_clear`**: Create a function `void terminal_clear(void)`.
3. **Update `kernel_main`:** Call `terminal_clear()` inside your main function before the infinite loop.
4. **Test:** Run `make run`.
## Lesson 2.2 The Terminal Driver (`terminal_putchar`)
Currently, our kernel can wipe the screen clean, but is has no concept of spatial awareness. To print a sequential string of text, the OS must track the current location of the cursor, write a character to that exact spot, and advance the cursor foward.
### The Typewriter
Think of the VGA hardware as a mechanical typewriter.
- You strike a key (pass a character to the function).
- The carriage prints the letter and physically advances one space to the right (incrementing the X coordinate).
- If the carriage hits the right margin, it must return to the left side and advance the paper up one line (resetting X, incrementing Y).
### Managing Cursor State
To implement this, the kernel needs global state tracking.
1. **Global Variables:** At the top of `kernel.c` (below our `VGA_COLS` and `VGA_ROWS` constants), create two static int variables named `terminal_row` and `terminal_column`. Initialize both to `0`.
2. **The Color State**: Create a `static unsigned char` named `terminal_color`. Initialize it to `0x07` (light gray on black).
3. **The Buffer Pointer**: Move the `unsigned short* terminal buffer = (unsigned short*)0xB8000;` declaration out of the `terminal_clear` function and make it a global `static` variable so all terminal functions can access it.
4. **The Write Function:** Create a new function: `void terminal_putchar(char c)`
5. **The Logic:** Inside `terminal_putchar`:
	- Calculate the flat memory index using the standard 2D-to-1D array formula: `index = (terminal_row * VGA_COLS) + terminal_column;`
	- Combine the global `terminal_color` and the character `c` using the exact same bitwise shift and OR logic from before.
	- Assign the combined 16-bit value to `terminal_buffer[index]`.
	- Increment `terminal_column` by 1.
### Pitfalls
- **State Desync:** If you define `terminal_row` and `terminal_column` inside the function, they will reset to `0` every time the function is called. They must be global.
- **Boundary Overflows:** We are intentionally ignoring what happens when `terminal_column` hits 80 right now. Do not write the newline logic yet. We isolate variables and test one mechanical step at a time.
### The Newline Problem
One more mechanical hurdle we will encounter is what happens if we attempt to write a string with a newline character. In `terminal_putchar('\n')`, the VGA will not drop to the next line. It will print a weird symbol instead.
The VGA hardware does not know what a "newline" is. It only renders ASCII characters. To create a newline, the software driver (our OS) must intercept the `\n` character and manually manipulate the X/Y coordinates instead of writing to the buffer.
## Lesson 2.3: Characters & The String Wrapper
Right now, we are printing strings by calling `terminal_putchar` six times in a row. This is unscalable. We need a `terminal_writestring()` function.
Furthermore, as we discussed, the VGA hardware is *dumb*. If you pass `\n` to the VGA buffer, it doesn't drop to a new line; it prints an obscure IBM character. The OS must intercept control characters and manually translate them into cursor coordinate math.
### The Typewriter Bell
When a typewriter carriage hits the right margin, a bell rings, and the user manually pushes the carriage back to the left (Carriage Return) and rolls the drum up one line (Line Feed).
Our `terminal_puchar` function must act as the bell. It needs to check if the incoming character is a `\n`, or if the carriage has naturally hit the 80th column. If either is true, it must manually execute the Carriage Return/Line Feed math.
### Interception and Iteration
Because we are in a freestanding environment, we do not have `<string.h>`. We must build ow own string utilities.
1. **The Safe Include:** At the absolute top of `kernel.c`, add `#include <stddef.h>`.
	- *Professor's Note*: Normally I told you to avoid standard headers, but `<stddef.h>`,`<stdint.h>`, and `<stdbool.h>` are **freestanding headers.** They do not require a host OS; they are provided directly by the compiler to give you standard types like `size_t` and `uint32_t`. It is best practice to use them.
2. **Write `strlen`**: Create a function `size_t strlen(const char* str)`. Write a simple `while` loop that increments a counter until it hits the null terminator (`\0`).
3. **Update `terminal_putchar`**: Add logic before you calculate the `index`:
	- If `c == '\n'`, set `terminal_column = 0`, increment `terminal_row`, and return immediately so it doesn't print a garbage character.
	- At the very end of the function, after incrementing `terminal_column`, check if it has it `VGA_COLS`. If it has, set `terminal_column = 0` and increment `terminal_row`.
4. **Write `terminal_writestring`**: Create `void terminal_writestring(const char* data)
	- Get the length of `data` using your new `strlen`
	- Use a `for` loop to iterate through the string, passing each character to `terminal_putchar`.
### Pitfalls
- **The Scrolling Trap:** What happens when `terminal_row` hits 25 (`VGA_ROWS`)? Right now, it will keep incrementing, and your math will calculate indexes outside the VGA buffer (`> 2000`). It will start overwriting kernel memory, eventually causing a fatal crash.
- **The Temporary Check:** For this lesson, we will intentionally ignore the scrolling feature. Just be aware that if you print more than 25 lines of text, your OS will corrupt its own memory.
## Lesson 2.4 Attributes and The Scrolling Trap
Every character on the VGA screen is a 16-bit value. We've been using `0x07`. To make a "Jackpot" screen or a "Busted" alert, we need to manipulate the **Attribute Byte** dynamically.
The 8-bit attribute is split into two 4-bit sections:
- **Lower 4 bits:** Foreground color (0-15)
- **Upper 4 bits:** Background color (0-7, or 0-15 if blinking is disabled).
### Dynamic Coloring
We will create a helper function to change the "House" color on the fly.
1. **The Setter:** Create `void terminal_setcolor(unsigned char color)`.
2. **The Enum:** (Optional but Professor-recommended) Define a set of color constants (e.g., `VGA_COLOR_RED = 4`, `VGA_COLOR_GOLD = 14`) so you aren't typing "magic numbers" into the code.
### The Scrolling Trap (Mechanical Integrity)
Right now, `terminal_putchar` increments `terminal_row` indefinitely. When it hits row 26, the index calculation points to memory addresses **past** the VGA buffer.
In a real OS, this is a "Silent Killer." You aren't writing to the screen anymore; you are overwriting your kernel's code or data, leading to a "Triple Fault" (reboot).
#### The Fix - Simple Scrolling:
When the cursor hits the last row, we must shift everything up.
1. **The Shift:** Move Row 1 to Row 0, Row 2 to Row 1, etc.
2. **The Clear:** Wipe the very last row to make it blank
3. **The Reset:** Set `terminal_row` back to the last line (24) instead of incrementing to 25.
### Pitfalls
- **Performance:** Nested loops ($O(n^2)$) inside a `putchar` function are technically slow. In a production OS, we would use `memmove`, but since we are "Bare Metal," we write the loops ourselves.
- **Attribute Consistency:** When you clear the bottom row, you should use the *current* `terminal_color`. If the last thing you printed was "JACKPOT" (Yellow), a bad implementation might make the entire empty bottom row yellow instead of black.
## Lesson 2.5: The Pit Boss (Interrupt Descriptor Table)
Casino OS is no longer mute, but it is still deaf, blind, and numb. If you press a key, or if your code triggers and Exception 14 (Page Fault) for the Vigorish Tax, the CPU will panic and reboot. We must build a system that catches these events.
### The IDT
The Interrupt Descriptor Table (IDT) is an array of exactly 256 entries. When the CPU encounters an event (an Exception or a Hardware Interrupt), it stops current execution, looks up the corresponding entry in this array, and jumps to the memory address of the handler function defined there.
- **Entries 0-31:** CPU Exceptions (0 is Divide by Zero, 14 is Page Fault)
- **Entries 32-255:** Hardware Interrupts (Keyboard, Mouse, System Calls)
### The 64-Bit Struct
The x86 architecture strictly dictates the format of an IDT entry. It is exactly 64 bits (8 bytes) long. It does not look like a standard C pointer. It is fragmented.
An IDT Entry requires:
1. **Lower 16 bits:** The lower 16 bits of the handler function's memory address.
2. **Next 16 bits:** The Kernel Segment Selector (a constant value of `0x08`)
3. **Next 8 bits:** Always zero.
4. **Next 8 bits:** Type and Attributes (Flags dictating who can call this interrupt.)
5. **Upper 16 bits:** The upper 16 bits of the handler function's memory address.
#### Side Topic: Hardware Struct Layout vs. C Grouping
In standard C programming, it is common practice to group variables of the same type together on a single line (e.g., `int a, b, c;`).
When defining data structures strictly for hardware consumption, this is a dangerous habit. **The CPU does not read your variable names.** It reads raw sequential byte offsets in RAM. The exact order you declare the variables in the struct dictates their physical sequence in memory.
By grouping all your `uint16_t` variables together first, and your `uint8_t` variables second, you'll build this memory layout:
- **Bytes 0-1:** `lw_16bits`
- **Bytes 2-3:** `sgmntSlctr_16bits`
- **Bytes 4-5:** `uppr_16bits` (The hardware expects the Zero byte and Flags here!)
- **Bytes 6-7:** `zero_8bits` and `flgs_8bits` (The hardware expects the Upper 16 bits here!)
When a keyboard interrupt fires, the CPU will attempt to construct the memory address of your handler function. It will accidentally stitch `lw_16bits` together with the `zero_8bits` and `flgs_8bits` values, generating a completely random memory address. It will jump to that random address and execute invalid instructions until the kernel violently crashes.
#### The x86 IDT Specification
To prevent this, you must declare the variables in the exact sequence the hardware demands. The x86 architecture dictates this specific byte-by-byte order from lowest memory address to highest:
1. **Bytes 0-1:** Lower 16 bits of the handler address (`uint16_t`).
2. **Bytes 2-3:** Segment selector (`uint16_t`).
3. **Byte 4:** Always zero (`uint8_t`).
4. **Byte 5:** Type and Attributes/Flags (`uint8_t`).E
5. **Bytes 6-7:** Upper 16 bits of the handler address (`uint16_t`).
### The IDT Pointer / IDTR
The CPU does not magically know where you allocated your array of 256 entries. You have to explicitly tell the processor where the table begins and how large it is.
The CPU stores this information in a dedicated hardware register called the **Interrupt Descriptor Table Register (IDTR)**. To load this register, x86 requires a specific 48-bit (6-byte) data structure.
### The 48-bit Map
We must define a second packed `struct` in the header file. This struct acts as the payload we will hand to the CPU via assembly later. It requires the exact two fields:
1. **Limit (16-bit integer)**: The total size of the IDT array in bytes, minus one.
2. **Base (32-bit integer):** The exact physical memory address where the first element of your array begins.
### Pitfalls
If you write a standard C `struct` with these variables, the C compiler will automatically insert invisible "padding" byes to align the memory for performance.
- *The Danger:* If the compiler adds padding, your struct will be larger than 8 bytes. The CPU will read the wrong data, execute a garbage memory address, and instantly Triple Fault (crash).
- *The Check:* We must use a compiler directive called `__attribute__((packed))` to force the compiler to leave the struct exactly as we define it, with zero padding.
Hardware limits usually define the maximum addressable offset, not the literal size.
- *The Danger:* Our table has 256 entries. Each entry is 8 bytes. The total size is 2,048 bytes. If you set the limit to `2048`, the CPU will technically believe byte offset `2048` is a valid interrupt, which is out of bounds.
- *The Fix:* You must explicitly define the limit as `(sizeof(struct idt_entry) * 256) - 1`.
## Lesson 2.6: The Gatekeeper (Setting an Entry)
When an interrupt fires (like Exception 14 for the Vigorish Tax), the CPU needs to jump to your handler function. In a 32-bit operating system, the memory address of that function is exactly 32 bits long.
As dictated by the `idt_entry` struct, the x86 hardware demands this 32-bit address be sliced in half. The lower 16 bits go to the byte offsets 0-1, and the upper 16 bits go to the byte offsets 6-7.