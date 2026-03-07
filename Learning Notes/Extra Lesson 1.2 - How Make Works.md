## The Directed Acyclic Graph (DAG)
`make` is not a script runner; it is a **dependency resolution engine**. When you run `make`, it builds a mathematical graph (a DAG) of what files rely on what other files. It looks at the timestamps of these files. If a source file (like `kernel.c`) is newer than the target object file (`kernel.o`), make knows the source was edited and re-compiles only that specific file. This saves hours of compilation time on large projects.

## The Assembly Line
Think of a `Makefile` as a factory floor.
- **Target:** The product you want to build
- **Prerequisites:** The parts needed to build it
- **Recipe:** The physical actions the machines take to assemble those parts
## The Anatomy of a Rule
Every instruction in a `Makefile` must follow this exact syntax block:
```
target_name: prerequisite1 prerequisite2
<TAB>command_to_run
```
- **Target:** What file are we creating? (Or what abstract action are we naming like `clean`)
- **Colon (`:`)**: Separates the target from its dependencies.
- **Prerequisites:** What files must exist before the command can run?
- **The command line must begin with a tab character.** If you use spaces, `make` throws a "missing separator" error.
## Pitfalls
- **The Space vs. Tab Trap:** This is the #1 reason Makefiles fail. Your IDE might be converting Tabs to Spaces automatically. Ensure your editor inserts a real Tab character.
- **Missing Dependencies:** If you don't list a prerequisite (e.g., a header file), `make` won't know it needs to recompile the C file when you change the header.
- **Variables without Parentheses:** If you define `CC = gcc`, you must call it as `$(CC)`. If you write `$CC`, `make` interprets it as the variable `$C` followed by a literal `C`