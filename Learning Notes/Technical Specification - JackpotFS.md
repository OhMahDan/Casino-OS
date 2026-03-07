```
"JackpotFS: A file system where data integrity is probabilistic. High-value data can be 'hit' with a CRC-32 check, or 'bust' with bit-rot."
```
Standard filesystems (like ext4 or NTFS) are built on deterministic guarantees. If you write `0xAA` to block 50, you must read `0xAA` from block 50. JackpotFS turns the block device driver into a casino table.
## Non-Deterministic Block Storage
Filesystems do not write data byte-by-byte; they write in bulk blocks (usually 4KB or 8KB). JackpotFS sits between the Virtual File System (VFS) layer and the actual disk driver, intercepting these blocks before they hit the platter.
## The I/O Gamble
When a user program calls the `write()` syscall, the kernel buffers the data and eventually flushes it to the disk.
1. **The Intercept:** The block driver receives a 4KB buffer to write to Sector X.
2. **The Roll:** The driver queries the `casino_rand()` PRNG.
3. **They Payouts:**
	- **The "Hit" (e.g., 90% chance)**: The block is written perfectly. A CRC-32 checksum is calculated and appended.
	- **The "Bust" / Bit-Rot (e.g., 9% chance):** The driver runs a loop over the 4KB buffer and randomly flips $N$ bits using a bitwise XOR (`^`) before writing it to the disk.
	- **The "Embezzlement" (e.g., 1% chance):** The driver completely drops the write request, returning a "Success" code to the user program, but writing absolute zeros to the disk sector.
## Pitfalls
- **Total Filesystem Collapse:** A filesystem consists of data blocks (the actual file contents) and metadata blocks (Superblocks, Inodes, directory structures).
	- *The Danger:* If the RNG bit-rots an Inode (the data structure that tells the OS where a file lives), you don't just lose a word in a text document--you completely orphan the entire file, or worse, cross-link it with another file, corrupting the whole drive.
	- *The Check/Test:* **The "Pit Boss" Rule**. We must strictly segregate metadata I/O from data I/O. The block driver must inspect the write request type. Only raw data user data blocks are subject to the gamble; metadata must remain 100% deterministic, or Casino OS will brick itself on first boot.
- **Write Caching:** Modern OSs cache writes in RAM and sync them to disk later. If a process writes to a file, reads it back immediately, and gets corrupted data, it might crash. The OS must decide if the "gamble" happens when the data enters the RAM cache, or when it flushes to the physical disk.