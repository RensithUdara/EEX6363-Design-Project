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

# Function: test
test:
    # Function prologue
    pushq %rbp
    movq %rsp, %rbp
    subq $48, %rsp              # Allocate space for 1 local variables
    # Processing function body statement
    # Local variable declaration
    # Variable: x allocated on stack
    # Assignment: variable := integer_literal
    movq $5, -8(%rbp)
    # Additional statement found
    # Assignment: variable := integer_literal
    movq $5, -8(%rbp)
    # Function epilogue
    movq %rbp, %rsp
    popq %rbp
    ret


# ======================================
# Code Generation Statistics
# Instructions generated: 0
# Registers spilled: 0
# Optimization passes: 0
# Memory layout size: 48 bytes
# ======================================

.section .note.GNU-stack,"",@progbits
# End of generated code
