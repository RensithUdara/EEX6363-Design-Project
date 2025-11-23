#define _POSIX_C_SOURCE 200809L
#include "../../include/codegen/register_allocator.h"
#include <limits.h>

// Global allocation statistics
static AllocationStats global_stats = {0, 0, 0, 0, 0.0};

// Create register allocator
RegisterAllocator *create_register_allocator(CodeGenContext *ctx, AllocationStrategy strategy)
{
    RegisterAllocator *allocator = malloc(sizeof(RegisterAllocator));
    if (!allocator)
        return NULL;

    allocator->strategy = strategy;
    allocator->stats = (AllocationStats){0, 0, 0, 0, 0.0};
    allocator->graph = NULL;
    allocator->codegen_ctx = ctx;

    return allocator;
}

// Destroy register allocator
void destroy_register_allocator(RegisterAllocator *allocator)
{
    if (!allocator)
        return;

    // Clean up interference graph if it exists
    if (allocator->graph)
    {
        InterferenceNode *node = allocator->graph->nodes;
        while (node)
        {
            InterferenceNode *next = node->next;
            if (node->neighbors)
                free(node->neighbors);
            free(node);
            node = next;
        }
        free(allocator->graph);
    }

    free(allocator);
}

// Main register allocation function
Register allocate_register(CodeGenContext *ctx, Symbol *var)
{
    if (!ctx)
        return REG_NONE;

    global_stats.total_allocations++;

    // First, try to find if variable is already in a register
    if (var)
    {
        for (int i = 0; i < MAX_REGISTERS; i++)
        {
            if (ctx->registers[i].status == REG_ALLOCATED &&
                ctx->registers[i].variable == var)
            {
                global_stats.successful_allocations++;
                return (Register)i;
            }
        }
    }

    // Try to find a free register
    Register free_reg = find_free_register(ctx);
    if (free_reg != REG_NONE)
    {
        ctx->registers[free_reg].status = REG_ALLOCATED;
        ctx->registers[free_reg].variable = var;
        ctx->registers[free_reg].last_use = global_stats.total_allocations;
        global_stats.successful_allocations++;
        return free_reg;
    }

    // No free register, need to spill
    Register spill_reg = find_spill_candidate(ctx);
    if (spill_reg != REG_NONE)
    {
        spill_register(ctx, spill_reg);

        // Now allocate the freed register
        ctx->registers[spill_reg].status = REG_ALLOCATED;
        ctx->registers[spill_reg].variable = var;
        ctx->registers[spill_reg].last_use = global_stats.total_allocations;
        global_stats.successful_allocations++;
        return spill_reg;
    }

    // If all else fails, return a temporary register
    return REG_RAX; // Fallback
}

// Free a register
void free_register(CodeGenContext *ctx, Register reg)
{
    if (!ctx || reg < 0 || reg >= MAX_REGISTERS)
        return;

    if (ctx->registers[reg].status == REG_RESERVED)
    {
        // Don't free reserved registers
        return;
    }

    ctx->registers[reg].status = REG_FREE;
    ctx->registers[reg].variable = NULL;
    ctx->registers[reg].last_use = -1;
    ctx->registers[reg].spill_cost = 0;
}

// Spill a register to memory
void spill_register(CodeGenContext *ctx, Register reg)
{
    if (!ctx || reg < 0 || reg >= MAX_REGISTERS)
        return;

    RegisterDescriptor *desc = &ctx->registers[reg];

    if (desc->variable)
    {
        // Generate spill code
        spill_variable(ctx, desc->variable, reg);
        global_stats.spills_performed++;
    }

    // Free the register
    free_register(ctx, reg);
}

// Find a free register
Register find_free_register(CodeGenContext *ctx)
{
    if (!ctx)
        return REG_NONE;

    // Prioritize caller-saved registers for temporary values
    int preferred_order[] = {
        REG_RAX, REG_RCX, REG_RDX, REG_RSI, REG_RDI, // Caller-saved
        REG_R8, REG_R9, REG_R10, REG_R11,            // Caller-saved
        REG_RBX, REG_R12, REG_R13, REG_R14, REG_R15  // Callee-saved
    };

    for (size_t i = 0; i < sizeof(preferred_order) / sizeof(int); i++)
    {
        Register reg = preferred_order[i];
        if (ctx->registers[reg].status == REG_FREE)
        {
            return reg;
        }
    }

    return REG_NONE;
}

// Find the best register to spill
Register find_spill_candidate(CodeGenContext *ctx)
{
    if (!ctx)
        return REG_NONE;

    Register best_candidate = REG_NONE;
    int lowest_priority = INT_MAX;

    for (int i = 0; i < MAX_REGISTERS; i++)
    {
        if (ctx->registers[i].status == REG_RESERVED)
        {
            continue; // Never spill reserved registers
        }

        if (ctx->registers[i].status == REG_ALLOCATED)
        {
            int priority = calculate_spill_priority(ctx, (Register)i);

            if (priority < lowest_priority)
            {
                lowest_priority = priority;
                best_candidate = (Register)i;
            }
        }
    }

    return best_candidate;
}

// Calculate spill priority (lower is better for spilling)
int calculate_spill_priority(CodeGenContext *ctx, Register reg)
{
    if (!ctx || reg < 0 || reg >= MAX_REGISTERS)
        return INT_MAX;

    RegisterDescriptor *desc = &ctx->registers[reg];
    int priority = 0;

    // Prefer to spill registers that haven't been used recently
    priority -= (global_stats.total_allocations - desc->last_use);

    // Prefer caller-saved over callee-saved (cheaper to spill)
    if (is_caller_saved(reg))
    {
        priority -= 10;
    }
    else if (is_callee_saved(reg))
    {
        priority += 10;
    }

    // Add spill cost
    priority += desc->spill_cost;

    return priority;
}

// Spill a variable to memory
void spill_variable(CodeGenContext *ctx, Symbol *var, Register reg)
{
    if (!ctx || !var)
        return;

    // Get or assign stack location for the variable
    int stack_offset = get_variable_offset(ctx, var);

    // Generate store instruction
    Operand reg_op = create_register_operand(reg, var->data_type);
    Operand mem_op = create_memory_operand(REG_RBP, stack_offset, var->data_type);

    Instruction *store_inst = create_instruction(INST_STORE, mem_op, reg_op, (Operand){0});
    store_inst->comment = strdup("Spill variable to stack");
    emit_instruction(ctx, store_inst);

    // Update variable descriptor
    for (int i = 0; i < ctx->variable_count; i++)
    {
        if (ctx->variables[i].symbol == var)
        {
            ctx->variables[i].allocated_register = REG_NONE;
            ctx->variables[i].stack_offset = stack_offset;
            ctx->variables[i].is_spilled = 1;
            break;
        }
    }
}

// Reload a variable from memory
Register reload_variable(CodeGenContext *ctx, Symbol *var)
{
    if (!ctx || !var)
        return REG_NONE;

    global_stats.reloads_performed++;

    // Find variable descriptor
    VariableDescriptor *var_desc = NULL;
    for (int i = 0; i < ctx->variable_count; i++)
    {
        if (ctx->variables[i].symbol == var)
        {
            var_desc = &ctx->variables[i];
            break;
        }
    }

    if (!var_desc || !var_desc->is_spilled)
    {
        return REG_NONE;
    }

    // Allocate a new register
    Register new_reg = allocate_register(ctx, var);
    if (new_reg == REG_NONE)
        return REG_NONE;

    // Generate load instruction
    Operand mem_op = create_memory_operand(REG_RBP, var_desc->stack_offset, var->data_type);
    Operand reg_op = create_register_operand(new_reg, var->data_type);

    Instruction *load_inst = create_instruction(INST_LOAD, reg_op, mem_op, (Operand){0});
    load_inst->comment = strdup("Reload variable from stack");
    emit_instruction(ctx, load_inst);

    // Update variable descriptor
    var_desc->allocated_register = new_reg;
    var_desc->is_spilled = 0;

    return new_reg;
}

// Save caller-saved registers before function call
void save_caller_saved_registers(CodeGenContext *ctx)
{
    if (!ctx)
        return;

    emit_comment(ctx, "Save caller-saved registers");

    Register caller_saved[] = {REG_RAX, REG_RCX, REG_RDX, REG_RSI, REG_RDI,
                               REG_R8, REG_R9, REG_R10, REG_R11};

    for (size_t i = 0; i < sizeof(caller_saved) / sizeof(Register); i++)
    {
        Register reg = caller_saved[i];

        if (ctx->registers[reg].status == REG_ALLOCATED && ctx->registers[reg].variable)
        {
            // Save this register
            Operand reg_op = create_register_operand(reg, TYPE_INTEGER);
            Instruction *push_inst = create_instruction(INST_PUSH, reg_op, (Operand){0}, (Operand){0});
            emit_instruction(ctx, push_inst);

            ctx->registers[reg].status = REG_CALLER_SAVED;
        }
    }
}

// Restore caller-saved registers after function call
void restore_caller_saved_registers(CodeGenContext *ctx)
{
    if (!ctx)
        return;

    emit_comment(ctx, "Restore caller-saved registers");

    Register caller_saved[] = {REG_R11, REG_R10, REG_R9, REG_R8,
                               REG_RDI, REG_RSI, REG_RDX, REG_RCX, REG_RAX};

    // Restore in reverse order (stack is LIFO)
    for (size_t i = 0; i < sizeof(caller_saved) / sizeof(Register); i++)
    {
        Register reg = caller_saved[i];

        if (ctx->registers[reg].status == REG_CALLER_SAVED)
        {
            // Restore this register
            Operand reg_op = create_register_operand(reg, TYPE_INTEGER);
            Instruction *pop_inst = create_instruction(INST_POP, reg_op, (Operand){0}, (Operand){0});
            emit_instruction(ctx, pop_inst);

            ctx->registers[reg].status = REG_ALLOCATED;
        }
    }
}

// LRU allocation strategy
Register allocate_with_lru(CodeGenContext *ctx, Symbol *var)
{
    if (!ctx)
        return REG_NONE;

    // Find free register first
    Register free_reg = find_free_register(ctx);
    if (free_reg != REG_NONE)
    {
        ctx->registers[free_reg].status = REG_ALLOCATED;
        ctx->registers[free_reg].variable = var;
        ctx->registers[free_reg].last_use = global_stats.total_allocations;
        return free_reg;
    }

    // Find LRU register
    Register lru_reg = find_lru_register(ctx);
    if (lru_reg != REG_NONE)
    {
        spill_register(ctx, lru_reg);

        ctx->registers[lru_reg].status = REG_ALLOCATED;
        ctx->registers[lru_reg].variable = var;
        ctx->registers[lru_reg].last_use = global_stats.total_allocations;
        return lru_reg;
    }

    return REG_NONE;
}

// Find least recently used register
Register find_lru_register(CodeGenContext *ctx)
{
    if (!ctx)
        return REG_NONE;

    Register lru_reg = REG_NONE;
    int oldest_use = INT_MAX;

    for (int i = 0; i < MAX_REGISTERS; i++)
    {
        if (ctx->registers[i].status == REG_RESERVED)
            continue;

        if (ctx->registers[i].status == REG_ALLOCATED &&
            ctx->registers[i].last_use < oldest_use)
        {
            oldest_use = ctx->registers[i].last_use;
            lru_reg = (Register)i;
        }
    }

    return lru_reg;
}

// Update last use timestamp
void update_last_use(CodeGenContext *ctx, Register reg)
{
    if (!ctx || reg < 0 || reg >= MAX_REGISTERS)
        return;

    ctx->registers[reg].last_use = global_stats.total_allocations;
}

// Build interference graph for graph coloring
InterferenceGraph *build_interference_graph(CodeGenContext *ctx, ASTNode *function)
{
    if (!ctx || !function)
        return NULL;

    InterferenceGraph *graph = malloc(sizeof(InterferenceGraph));
    if (!graph)
        return NULL;

    graph->nodes = NULL;
    graph->node_count = 0;

    // This is a simplified implementation
    // In a real compiler, this would analyze live ranges and build interferences

    return graph;
}

// Add interference edge between two variables
void add_interference_edge(InterferenceGraph *graph, Symbol *var1, Symbol *var2)
{
    if (!graph || !var1 || !var2 || var1 == var2)
        return;

    // This would involve finding/creating nodes and adding bidirectional edges
    // Simplified for demonstration - actual implementation would create and link nodes
    (void)var1; // Suppress unused parameter warning
    (void)var2; // Suppress unused parameter warning
}

// Calculate register pressure at a given point
int calculate_register_pressure(CodeGenContext *ctx, ASTNode *node)
{
    if (!ctx || !node)
        return 0;

    int pressure = 0;

    // Count allocated registers
    for (int i = 0; i < MAX_REGISTERS; i++)
    {
        if (ctx->registers[i].status == REG_ALLOCATED)
        {
            pressure++;
        }
    }

    return pressure;
}

// Analyze live ranges for optimal allocation
void analyze_live_ranges(CodeGenContext *ctx, ASTNode *function)
{
    if (!ctx || !function)
        return;

    // This would perform a data flow analysis to determine
    // where each variable is live (defined and potentially used)
    // Simplified for demonstration
}

// Print allocation statistics
void print_allocation_stats(RegisterAllocator *allocator)
{
    if (!allocator)
        return;

    printf("\nRegister Allocation Statistics:\n");
    printf("  Total allocation requests: %d\n", global_stats.total_allocations);
    printf("  Successful allocations: %d\n", global_stats.successful_allocations);
    printf("  Spills performed: %d\n", global_stats.spills_performed);
    printf("  Reloads performed: %d\n", global_stats.reloads_performed);

    if (global_stats.total_allocations > 0)
    {
        global_stats.allocation_efficiency =
            (double)global_stats.successful_allocations / global_stats.total_allocations * 100.0;
        printf("  Allocation efficiency: %.2f%%\n", global_stats.allocation_efficiency);
    }

    printf("  Strategy used: ");
    switch (allocator->strategy)
    {
    case ALLOC_NAIVE:
        printf("Naive\n");
        break;
    case ALLOC_LRU:
        printf("Least Recently Used\n");
        break;
    case ALLOC_OPTIMAL:
        printf("Optimal\n");
        break;
    case ALLOC_GRAPH_COLOR:
        printf("Graph Coloring\n");
        break;
    default:
        printf("Unknown\n");
        break;
    }
}

// Register state management
void mark_register_dirty(CodeGenContext *ctx, Register reg)
{
    if (!ctx || reg < 0 || reg >= MAX_REGISTERS)
        return;
    ctx->registers[reg].spill_cost += 5; // Increase spill cost for dirty registers
}

void mark_register_clean(CodeGenContext *ctx, Register reg)
{
    if (!ctx || reg < 0 || reg >= MAX_REGISTERS)
        return;
    ctx->registers[reg].spill_cost = 0; // Reset spill cost
}

int is_register_dirty(CodeGenContext *ctx, Register reg)
{
    if (!ctx || reg < 0 || reg >= MAX_REGISTERS)
        return 0;
    return ctx->registers[reg].spill_cost > 0;
}

// Global statistics access functions for output generation
int get_registers_used(void)
{
    return global_stats.successful_allocations > MAX_REGISTERS ? MAX_REGISTERS : global_stats.successful_allocations;
}

int get_spill_operations(void)
{
    return global_stats.spills_performed;
}

const char *get_allocation_strategy(void)
{
    return "Linear Scan"; // Default strategy name
}