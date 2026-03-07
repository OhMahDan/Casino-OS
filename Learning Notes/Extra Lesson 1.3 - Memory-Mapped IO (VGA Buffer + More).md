*Question: "What determines the VGA buffer address (0xB8000)? Is it the boot loader?*

Neither the bootloader nor the OS determines this address. It is dictated by **hardwired physical physics on the motherboard,** specifically an IBM PC standard established in 1987.
## The Memory Controller
When the CPU executes `terminal_buffer[loc] = val`, it pushes an electrical signal down its pins saying "Store this data at address `0xB8000`."
The CPU thinks it is talking to your RAM stick. But the motherboard has a routing chip called the **Memory Controller Hub**. This chip acts like a post office sorting mail:
- If the address is between `0x00000000` and `0x0009FFFF`, it routes the electrical signal to your physical RAM sticks.
- **If the address is exactly `0x000B8000`**, it intercepts the signal. It prevents it from going to RAM and instead routes it directly across the motherboard traces to the PCIe slot where your Video Graphics Array (VGA) chip lives.
The VGA hardware receives that byte, translates the bit pattern into a physical pixel color, and blasts it out the HDMI/DisplayPort cable to your monitor.

The Concept is called **Memory-Mapped I/O (MMIO).** We will use this exact same concept later to read keystrokes from the keyboard or read sectors from a hard drive. You don't use special "hardware" commands; you just read and write to magic memory addresses defined by Intel and IBM decades ago.

However, because the x86 architecture is heavily burdened by backwards compatibility with the 1980s, legacy devices like the PS/2 keyboard, the serial port, and the programmable interrupt controller do **not** use standard memory addresses. They use **Port-Mapped I/O (PMIO)**.
## The Hardware Illusion
- **MMIO (VGA Buffer, Modern GPUs**): The device lives on the main street. You use standard C pointers (`*ptr = value`) to drop off data at its house. The CPU doesn't even know it's talking to a device; it think's it's talking to RAM.
- **PMIO (Legacy Keyboard, Mouse)**: The device lives in a separate, hidden P.O. Box system. Standard C pointers cannot reach it. The x86 CPU has special, dedicated assembly instructions (`inb` to read a byte, `outb` to write a byte) specifically designed to access this hidden 16-bit address space.
When we eventually write the keyboard driver for Casino OS, we will have to write inline assembly to use the `inb` instructions to read from P.O. Box `0x60` (The standard x86 keyboard port).