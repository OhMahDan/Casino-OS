
1. **Module 1: The Foundation (Booting & Text Mode).** Setting up a modern bootloader to drop us into a C kernel. Writing to the VGA text buffer (printing to the screen without an OS).
2. **Module 2: The Pit Boss (Interrupts & Exceptions).** Setting up the IDT (Interrupt Descriptor Table). Catching CPU panics and reading keyboard input.
3. **Module 3: The Vault (Memory Management).** Physical page allocation and virtual memory (Paging). Implementing the "Vigorish" (taxed memory).
4. **Module 4: The Croupier (Process Scheduling).** Context switching. Moving from a single thread to multiple threads. Implementing the "Weighted Lottery."
5. **Module 5: The Floor (User Space & Syscalls).** Dropping from Kernel Mode (Ring 0) to User Mode (Ring 3). Creating the `gamble()` system call API.
6. **Module 6: The Showroom (Graphics).** Transitioning from VGA text to a linear framebuffer. Drawing pixels, shapes, and eventually text rendering and windows.