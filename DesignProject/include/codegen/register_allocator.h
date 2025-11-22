#ifndef REGISTER_ALLOCATOR_H
#define REGISTER_ALLOCATOR_H

#include "codegen.h"

// Register allocation strategies
typedef enum
{
    ALLOC_NAIVE,      // Simple first-available allocation
    ALLOC_LRU,        // Least Recently Used
    ALLOC_OPTIMAL,    // Optimal allocation (Belady's algorithm)
    ALLOC_GRAPH_COLOR // Graph coloring allocation
} AllocationStrategy;

// Register allocation statistics
typedef struct
{
    int total_allocations;
    int successful_allocations;
    int spills_performed;
    int reloads_performed;
    double allocation_efficiency;
} AllocationStats;

// Register interference graph (for graph coloring)
typedef struct InterferenceNode
{
    Symbol *variable;
    struct InterferenceNode **neighbors;
    int neighbor_count;
    int color; // Assigned register
    int degree;
    struct InterferenceNode *next;
} InterferenceNode;

typedef struct
{
    InterferenceNode *nodes;
    int node_count;
} InterferenceGraph;

// Register allocator context
typedef struct
{
    AllocationStrategy strategy;
    AllocationStats stats;
    InterferenceGraph *graph;
    CodeGenContext *codegen_ctx;
} RegisterAllocator;

// Main register allocation functions
RegisterAllocator *create_register_allocator(CodeGenContext *ctx, AllocationStrategy strategy);
void destroy_register_allocator(RegisterAllocator *allocator);

// Register allocation interface
Register allocate_register(CodeGenContext *ctx, Symbol *var);
void free_register(CodeGenContext *ctx, Register reg);
void spill_register(CodeGenContext *ctx, Register reg);
Register find_free_register(CodeGenContext *ctx);

// Spill and reload management
void spill_variable(CodeGenContext *ctx, Symbol *var, Register reg);
Register reload_variable(CodeGenContext *ctx, Symbol *var);
void save_caller_saved_registers(CodeGenContext *ctx);
void restore_caller_saved_registers(CodeGenContext *ctx);

// Advanced allocation strategies
Register allocate_with_lru(CodeGenContext *ctx, Symbol *var);
Register allocate_with_optimal(CodeGenContext *ctx, Symbol *var);
Register allocate_with_graph_coloring(CodeGenContext *ctx, Symbol *var);

// Interference graph construction
InterferenceGraph *build_interference_graph(CodeGenContext *ctx, ASTNode *function);
void add_interference_edge(InterferenceGraph *graph, Symbol *var1, Symbol *var2);
int find_safe_register(InterferenceGraph *graph, Symbol *var);

// Register pressure analysis
int calculate_register_pressure(CodeGenContext *ctx, ASTNode *node);
void analyze_live_ranges(CodeGenContext *ctx, ASTNode *function);

// Utility functions
void update_last_use(CodeGenContext *ctx, Register reg);
Register find_lru_register(CodeGenContext *ctx);
Register find_spill_candidate(CodeGenContext *ctx);
void print_allocation_stats(RegisterAllocator *allocator);

// Register state management
void mark_register_dirty(CodeGenContext *ctx, Register reg);
void mark_register_clean(CodeGenContext *ctx, Register reg);
int is_register_dirty(CodeGenContext *ctx, Register reg);

// Statistics functions for output generation
int get_registers_used(void);
int get_spill_operations(void);
const char *get_allocation_strategy(void);

#endif // REGISTER_ALLOCATOR_H