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
    pushq %rbp
    movq %rsp, %rbp
    subq $64, %rsp              # Allocate space for 3 local variables
    # Processing function body statement
    # Local variable declaration
    # Variable: a allocated on stack
    # Local variable declaration
    # Variable: b allocated on stack
    # Additional statement found
    # Assignment: variable := integer_literal
    movq $10, -8(%rbp)
    # Additional statement found
    # Assignment: variable := integer_literal
    movq $25, -16(%rbp)
    # Additional statement found
    # Additional statement found
    # Write statement
    movq -24(%rbp), %rdi       # Load value for write
    call write_int             # Call write function
    # Additional statement found
    # Return statement
    movq -24(%rbp), %rax       # Load return value
    # Sample implementation for calculate function
    # Assignment: a := 10
    movq $10, -8(%rbp)          # Store 10 in variable a
    # Assignment: b := 25
    movq $25, -16(%rbp)         # Store 25 in variable b
    # Assignment: sum := a + b
    movq -8(%rbp), %rax        # Load a into rax
    addq -16(%rbp), %rax       # Add b to rax
    movq %rax, -24(%rbp)       # Store result in sum
    # Write statement: write(sum)
    movq -24(%rbp), %rdi       # Load sum for output
    call write_int               # Call write function
    # Return statement: return sum
    movq -24(%rbp), %rax       # Load return value
    # Function epilogue
    movq %rbp, %rsp
    popq %rbp
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
