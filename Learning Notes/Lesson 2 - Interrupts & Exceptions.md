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
