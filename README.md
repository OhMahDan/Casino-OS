# Casino OS (🎰)

### "The House Always Wins"



*Fact: 90% of gamblers quite right before they're about to hit big*

This is a personal project for a parody operating system to expand my knowledge and skills. Notes for the process and everything I've learned are kept in /PersonalNotes.



Casino OS is an experimental, non-deterministic kernel designed to explore the limits of **Stochastic Resource Allocation**. While traditional kernels aim for fairness and determinism, Casino OS treats system resources (CPU, RAM, I/O) as a zero-sum game managed by the **Kernel Croupier**.



## Core Architecture: The House Rules

* **Weighted Lottery Scheduling:** Processes "ante up" clock cycles. The scheduler uses a high-entropy RNG to determine the next thread, with a 5.1% "House Edge" reserved for kernel tasks.

* **Vigorish Memory Management:** Every `malloc` incurs a 5% memory tax. Processes can attempt a `double_or_nothing()` syscall to reclaim it.

* **JackpotFS:** A file system where data integrity is probabilistic. High-value data can be "hit" with a `CRC-32` check, or "bust" with bit-rot.



## Status: ALPHA

**Warning:** This OS is a parody. It is designed to be unstable. Running this may result in process starvation, memory embezzlement, or spontaneous system-wide bankruptcies (Kernel Panics).

