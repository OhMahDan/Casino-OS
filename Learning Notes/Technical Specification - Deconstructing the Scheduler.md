```
"Weighted Lottery Scheduling: Processes 'ante up' clock cycles. The scheduler uses a high-entropy RNG to determine the next thread, with a 5.1% 'House Edge' reserved for kernel tasks."
```
## Time Multiplexing
A CPU core can only execute one process at a time. The OS creates the illusion of multitasking by rapidly switching between processes (e.g., every 10 milliseconds). This 10ms window is called a **Time Quantum.** The **Scheduler** is the algorithm that decides which process gets the CPU for the next quantum.
## Weighted Lottery Scheduling
Standard operating systems use a Multi-level Feedback Queue (MLFQ). Casino OS uses a probabilistic algorithm.
- **The Technical Reality:** Every process in the "Runnable" state is assigned an integer number of "tickets". To pick the next process, the kernel generates a random number $R$ between 0 and the total number of tickets, $T_{total}$. 
- **The Math:** The probability P of a specific process `i` getting the CPU is exactly proportional to its ticket count:
$$
		P(Process_i) = \frac{T_i}{T_{total}}
$$
## "Ante Up" Clock Cycles
In standard OS design, if a process finishes its work early (before its 10ms quantum is up), it simply yields the CPU, and that time is lost.
- **Technical Implementation:** In Casino OS, a process can execute a custom system call (e.g., `sys_ante())` to voluntarily yield the CPU early. The kernel tracks how many milliseconds were forfeited. It then converts those forfeited milliseconds into *additional lottery tickets* for that process in the next scheduling round.
- **The Result**: A process trades guaranteed immediate execution time for a statistically higher probability of winning a longer execution streak later.
## High-Entropy RNG
A standard C `rand()` function uses a predictable mathematical formula. If the sequence is predictable, a malicious user process could calculate exactly when it needs to "ante up" to guarantee a win, effectively starving the rest of the system.
- **Technical Reality:** The kernel must seed its Pseudo-Random Number Generator (PRNG) with hardware entropy. On x86, we use `RDTSC` assembly instruction, which returns the exact number of clock cycles since the CPU was powered on. Because hardware interrupts and user inputs happen at unpredictable microsecond intervals, the lower bits of the `RDTSC` counter are statistically random and prevent processes from "counting cards".
## The "House Edge"
If user processes can hoard tickets, they could collectively hold 99% of the lottery tickets, meaning the OS itself (kernel cleanup threads, disk flushing) would never get to run. The system would freeze.
- **Technical Reality:** The kernel maintains a hardcoded ticket floor. Before calculating the winning ticket, the scheduler calculates $T_{total}$. It automatically assigns $0.051\times T_{total}$ tickets directly to the Kernel Thread.
- **The Result:** No matter how many tickets user processes buy, there is a mathematically guaranteed 5.1% probability on every context switch that the kernel takes control of the CPU to perform critical maintenance.
### Why 5.1%?
Two main reasons, *in order of importance*:
	1. **Thematic Accuracy**: In double-zero American Roulette, the true mathematical "House Edge" is exactly $\frac{2}{38}$, which is $5.26\%$. Picking ~5% aligns the kernel's behavior with real-world casino mathematics.
	2. **Technical Reality (Kernel Overhead)**: In a standard operating system, the kernel typically consumes between 1% to 5% of total CPU cycles just handling background tasks: timer interrupts, hardware I/O, and updating process queues. If the kernel's reserved time drops below 1% under heavy load, you risk **Watchdog Starvation--** hardware components time out and drop data (like network packets) because the kernel wasn't awake to process them. Reserving ~5% mathematically guarantees system stability while maximizing user-space execution.