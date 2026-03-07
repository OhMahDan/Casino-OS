A CPU is a dumb rock that only knows how to read instructions from memory. But when you first turn on the power, the RAM is completely empty. If the RAM is empty, how does the CPU know what to do?

## The Relay Race
Booting a computer is a relay race where each runner is slightly smarter than the last.
1. Electricity from the power supply wakes up the **Firmware** (BIOS).
2. **Firmware** wakes up the **Bootloader** (GRUB)
3. **Bootloader** wakes up the **Kernel** (Casino OS)
## Power to Code
### Step 1: Electricity and The Reset Vector
1. You press the power button. The power supply stabilizes the voltage and sends an electrical signal called "Power Good" to the motherboard.
2. The motherboard's clock generators starts ticking.
3. The CPU receives power and executes its hardwired "Reset" sequence.
4. **The Magic Trick:** The CPU is physically hardwired by Intel/AMD to look at one specific memory address the moment it wakes up: `0xFFFFFFF0` (The Reset Vector). This address does not point to your RAM. It is physically mapped to a Flash ROM chip glued to your motherboard.
### Step 2: The Firmware (BIOS/UEFI)
1. The CPU reads the code from that ROM chip. This code is the **BIOS** (Basic Input/Output System)
2. The BIOS runs the POST (Power-On Self-Test) to ensure the hardware isn't broken.
3. **Crucial:** The BIOS initializes the RAM. Before this moment, you cannot store any variables because the RAM controller hasn't been configured.
4. The BIOS looks at your boot order (USB, Hard drive, CD). It goes to the first drive it finds and reads the very first 512 bytes of the physical disk (Sector 0).
### Step 3: The Bootloader (The Handoff)
1. Those first 512 bytes are the **Master Boot Record (MBR)**. The BIOS blindly copies those 512 bytes into RAM and tells the CPU to jump to it.
2. **You are now running the Bootloader.** Because 512 bytes is incredibly small, this "stage 1" bootloader's only job is to find the rest of itself ("Stage 2", usually a program like GRUB) on the hard drive and load it into RAM.
3. GRUB knows how to read filesystems (like FAT32 or ext4). It searches your hard drive for a file named `kernel.bin`.
4. GRUB reads the multiboot header you wrote in `boot.S`. Seeing the `MAGIC` number, it knows it's a valid OS. It loads your kernel into RAM at `0x100000` (1MB), sets up the CPU state, and executes `jmp _start`.
## Activities
If you ever want to prove this to yourself, you can write a 512-byte raw binary file containing just a few assembly instructions to print "A" to the screen and an infinite loop. If you write that to Sector 0 of a USB drive and plug it into an old laptop, the laptop will boot your code instead of Windows.