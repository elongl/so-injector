# Idea
The injector merely mimics the common strategy for injecting DLL in Windows applications.  


# Steps
1. Suspend the process.
2. Save current state (registers and such).
3. Write a `.so` path to a R/W memory area.
5. Place `dlopen()` arguments on the corresponding registers.
6. Locate and call `dlopen()` from within `libc`.
7. Revert the `.so` path's memory.
7. Revert to the previous process state.

All of this would be done using `ptrace()`.