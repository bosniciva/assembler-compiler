# MAVN (High-Level MIPS Assembler) Compiler Implementation

A custom-built, sequential optimization compiler written in C++ designed to translate a High-Level MIPS Assembly language (`.mavn`) into target-compliant native MIPS 32-bit assembly code (`.s`). This tool introduces the concept of register variables to simplify low-level assembly programming, abstracting away manual register management by automating physical resource tracking and memory level boundaries.

## Compiler Pipeline & Architectural Features

The compiler is structured as a standard sequential pipeline divided into separate analysis and optimization phases:
[ .mavn file ] ──> [ Lexical Analysis ] ──> [ Syntax Analysis ] ──> [ Liveness Analysis ] ──> [ Resource Allocation ] ──> [ .s file ]

*   **Lexical Analyzer (Lexer):** Tokenizes the incoming characters into discrete architectural blocks (keywords, labels, variable identifiers, symbols, and operands) while detecting lexical errors and unexpected tokens.
*   **Syntax Analyzer (Parser):** Consumes tokens produced by the lexer and builds an internal grammatical layout verification loop based on a custom defined language context grammar matrix. Tracks duplicate label/variable definitions and syntax mismatches.
*   **Liveness Analysis Engine:** Executes backward data-flow iterations to calculate variables' lifespans (`In` and `Out` variable live ranges) across basic instruction blocks using dataflow optimization formulas:
    *   $out[n] = \bigcup_{s \in succ[n]} in[s]$
    *   $in[n] = use[n] \cup (out[n] \setminus def[n])$
*   **Resource Allocation (Interference Graph Coloring):** 
    *   Generates an **Interference Graph** where nodes correspond to program variables and edges signify overlapping live ranges.
    *   Applies a recursive **Chaitin's Graph Simplification Stack** to isolate nodes with degrees below available hardware registers.
    *   Executes an **Iterative Graph Coloring Algorithm** to dynamically map virtual variables into safe, non-conflicting physical registers ($t0-\$t3) while handling resource spill limits.

---

## Compilation Target & Verification Output

The verification pipeline passes automated diagnostics showing successful variable-to-register assignment graphs and structural text output framing.

### Variable Interference Register Mapping Data
When compiling a standard test file (`simple.mavn`), the resource allocation engine flags variables, generates the interference matrix, and binds them to physical hardware registers seamlessly:
Simplification successful
Graph coloring successful
Variable r1 is assigned to register 4 -> $t3
Variable r2 is assigned to register 3 -> $t2
Variable r3 is assigned to register 1 -> $t0
Variable r4 is assigned to register 2 -> $t1
Variable r5 is assigned to register 1 -> $t0

### Translated Compilation Preview (Output `.s` Segment)
The final `.text` and `.data` code serialization blocks match native MIPS environments:

```assembly
.globl main

.data
m1:     .word 6
m2:     .word 5

.text
main:
        la \$t1, m1
        lw t3, 0(t1)
        la \$t0, m2
        lw t2, 0(t0)
        add \$t0, t3, t2
```

---

## Repository Blueprint

*   `LexicalAnalysis.cpp` / `LexicalAnalysis.h` — Input buffering, state machine initialization, and word-to-token parsing routines.
*   `SyntaxAnalysis.cpp` / `SyntaxAnalysis.h` — LL(1)-based grammar parsers and variable validation mapping.
*   `LivenessAnalysis.cpp` / `LivenessAnalysis.h` — Dynamic data-flow live range calculating matrix.
*   `ResourceAllocation.cpp` / `ResourceAllocation.h` — Interference graph builder, simplification stack filters, and graph coloring registers binder.
*   `main.cpp` — Main orchestration file managing file access hooks and triggering compilation segments.

---

## Local Execution & Verification

### Building the Compiler
The project can be built using any standard C++ compiler setup (or compiled inside Visual Studio):
```bash
# Compile via standard GCC terminal command
g++ -std=c++17 main.cpp LexicalAnalysis.cpp SyntaxAnalysis.cpp LivenessAnalysis.cpp ResourceAllocation.cpp -o mavn_compiler

# Execute the compiler targeting a test asset script
./mavn_compiler examples/simple.mavn output.s
```