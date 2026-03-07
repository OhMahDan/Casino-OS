# Casino OS (🎰)

### "The House Always Wins"



*Fact: 90% of gamblers quite right before they're about to hit big*

This is a personal project for a parody operating system to expand my knowledge and skills. Notes for the process I'm taking, technical specification details, and everything I've learned in this project are kept in `/Learning Notes`.



Casino OS is an experimental, non-deterministic kernel designed to explore the limits of **Stochastic Resource Allocation**. While traditional kernels aim for fairness and determinism, Casino OS treats system resources (CPU, RAM, I/O) as a zero-sum game managed by the **Kernel Croupier**.



## Core Architecture: The House Rules

* **Weighted Lottery Scheduling:** Processes "ante up" clock cycles. The scheduler uses a high-entropy RNG to determine the next thread, with a 5.1% "House Edge" reserved for kernel tasks.

* **Vigorish Memory Management:** Every `malloc` incurs a 5% memory tax. Processes can attempt a `double_or_nothing()` syscall to reclaim it.

* **JackpotFS:** A file system where data integrity is probabilistic. High-value data can be "hit" with a `CRC-32` check, or "bust" with bit-rot.
	* *One possible benefit:* **Chaos Engineering.** By building a filesystem that randomly corrupts data, I am inadvertently forcing myself to learn how to write incredibly robust data-recovery algorithms, checksum validators, and error-handling routines. The joke masks a brutally difficult engineering problem.

## Optional Accessory "Features"

* **The High-Roller Shell** Instead of a standard Bash-like shell, we can build a command line that requires users to pass "wagers" as arguments to standard commands.
	* *Example:* To read a file, the user types `read("notes.txt", 50)`. The `50` is the percentage of their CPU tickets they are willing to bet. If they win, the file opens instantly. If they lose, the shell hangs for 5 seconds as a penalty.
* **Suicide-Linux Inspired "Russian Roulette Shell"**: Suicide Linux introduces the concept of High-Stakes Input. 
	* *Application:* We can implement a similar, albeit less destructive, mechanic in your Casino OS shell. If you mistype a command or provide invalid arguments, the shell doesn't just reject it. It rolls the `casino_rand()` dice.
	* *The Mechanic:* 
		* 80% chance: "Command not found. The House forgives you."
		* 20% chance: The shell executes a minor penalty, like forcibly sleeping the terminal for 10 seconds or killing your most recent backgrounded process. It turns typos into a gamble.

## Status: ALPHA

**Warning:** This OS is a parody. It is designed to be unstable. Running this may result in process starvation, memory embezzlement, or spontaneous system-wide bankruptcies (Kernel Panics).

