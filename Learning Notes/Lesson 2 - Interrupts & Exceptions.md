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