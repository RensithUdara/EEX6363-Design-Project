# ======================================
# Generated Assembly Code
# Compiler Design Project - Code Generation Phase
# Target Architecture: x86-64
# Assembly Syntax: AT&T
# Register Allocation: Default
# ======================================

.section .data
    # Global variables and constants
    .align 8

.section .text
.globl main
.type main, @function

    # Program: Code generation from semantic analyzer
    # Function definition

# Function: calculate
calculate:
    # Function prologue
    push %rbp
    mov %rsp, %rbp
    sub $64, %rsp              # Allocate space for 3 local variables
    # Local variable declaration
    # Variable: a allocated on stack
    # Function epilogue
    mov %rbp, %rsp
    pop %rbp
    ret


# ======================================
# Code Generation Statistics
# Instructions generated: 0
# Registers spilled: 0
# Optimization passes: 0
# Memory layout size: 64 bytes
# ======================================

.section .note.GNU-stack,"",@progbits
# End of generated code
