```
"Every malloc call incurs a 5% memory tax. Processes can attempt a double_or_nothing() syscall to reclaim it." 
```
To build this, we have to correct a fundamental misunderstanding of how memory works. The kernel does not know what `malloc` is. `malloc` is a user-space C library function (part of `libc`). The kernel only understands **Page Allocations**.
Here is the technical reality of how we implement this "tax" at the hardware level.
## The 5% Memory Tax
When a user process needs memory, its `libc` makes a system call-- usually `mmap`or `sbrk`--asking the kernel for physical RAM. The CPU's Memory Management Unit (MMU) divides RAM into discrete 4-Kilobyte blocks called **Pages**.
- **Technical Implementation:** When a process requests $N$ pages via `mmap`, the kernel allocates the physical RAM, but manipulates the **Page Table Entries (PTEs).** For exactly 5% of the requested pages, the kernel clears the `User/Supervisor` bit in the PTE.
- **The Result:** The process is given the virtual memory addresses and believes the allocation was fully successful. However, if the process attempts to read or write to those specific "taxed" pages, the CPU hardware detects the missing User bit throws a **Page Fault (Exception 14)**. The kernel intercepts this faults and immediately terminates the process for "Tax Evasion."
## The `double_or_nothing()` Syscall
This requires creating a custom system call (e.g., syscall number 42). A process calls this function and passes the memory pointer of its allocation as the argument.
- **Technical Implementation:** The kernel receives the pointer, enters Ring 0 (Kernel Mode), and queries its PRNG (the dice roll).
	- **Heads (The Win)**: The kernel walks the Page Table, finds the taxed pages associated with that pointer, and flips the `User/Supervisor` bit back to `User`. It returns execution to the process. The process now has 100% of its memory.
	- **Tails (The Bust)**: The kernel executes a full memory unmap (`munmap`) on the *entire* allocation--destroying both the taxed 5% and the 95% the user rightfully owned. The physical RAM is reclaimed by the House. The kernel then sends a SIGSEGV (Segmentation Fault) signal to the process.
## Pitfalls
- **Hardware Granularity Limit:** You cannot tax "5% of an integer" or "5% of a 100-byte array." The x86 MMU only enforces permissions on 4,096-byte boundaries (4KB Pages).
	- *The Check/Fix*: The tax can only be applied to bulk allocations. If a process requests 20 pages (80KB), the kernel successfully locks exactly 1 page (5%). if a process requests a single page, the kernel cannot apply the tax at the hardware level without locking the entire page.  `libc` must be written to artificially pad small allocations so the kernel has enough pages to tax.
- **Kernel Panics from Kernel Space**: If the kernel tries to write to a taxed page while doing a legitimate system task (like copying a file to a user buffer), the CPU will still throw a Page Fault and crash the entire OS. The kernel must temporarily bypass its own tax when doing I/O.