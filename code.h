#ifndef CODE_H
#define CODE_H

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants
#define MAX_MEMORY 30000 // Initial memory tape size
#define MAX_STACK 1000   // Stack size for values and loops
#define MAX_PROG 10000   // Max program length (commands)
#define MAX_CMD_LEN 6    // Max command length (e.g., +108, >1, .)
#define MAX_FUNCTIONS 100 // Max number of user-defined functions
#define MAX_FUNC_NAME 32  // Max function name length
#define MAX_CALL_STACK 100 // Max call depth for functions
#define DEBUG_MODE 0     // Enable debug logging (0 = off, 1 = on)

// Error codes
typedef enum {
    ERR_NONE,
    ERR_MEMORY_ALLOC,
    ERR_STACK_OVERFLOW,
    ERR_STACK_UNDERFLOW,
    ERR_LOOP_MISMATCH,
    ERR_INVALID_COMMAND,
    ERR_DIV_BY_ZERO,
    ERR_TIMEOUT,
    ERR_INSTRUCTION_LIMIT,
    ERR_FUNC_NOT_FOUND,
    ERR_FUNC_OVERFLOW,
    ERR_CALL_STACK_OVERFLOW
} ErrorCode;

// Command types (all 300 commands)
typedef enum {
    // Pointer Movement (30)
    CMD_RIGHT, CMD_LEFT,CMD_RIGHT_0, CMD_RIGHT_1, CMD_RIGHT_2, CMD_RIGHT_3, CMD_RIGHT_4,
    CMD_RIGHT_5, CMD_RIGHT_6, CMD_RIGHT_7, CMD_RIGHT_8, CMD_RIGHT_9,
    CMD_LEFT_1, CMD_LEFT_2, CMD_LEFT_3, CMD_LEFT_4, CMD_LEFT_5,
    CMD_LEFT_6, CMD_LEFT_7, CMD_LEFT_8, CMD_LEFT_9,
    CMD_RIGHT_REG, CMD_LEFT_REG, CMD_RIGHT_CELL, CMD_LEFT_CELL,
    CMD_RIGHT_ZERO, CMD_LEFT_LAST, CMD_RIGHT_RAND, CMD_LEFT_RAND,
    // Memory Manipulation (50)
    CMD_INC, CMD_DEC, CMD_INC_1, CMD_INC_2, CMD_INC_3, CMD_INC_4, CMD_INC_5,
    CMD_INC_6, CMD_INC_7, CMD_INC_8, CMD_INC_9,
    CMD_DEC_1, CMD_DEC_2, CMD_DEC_3, CMD_DEC_4, CMD_DEC_5,
    CMD_DEC_6, CMD_DEC_7, CMD_DEC_8, CMD_DEC_9,
    CMD_SET_ZERO, CMD_SET_REG, CMD_SET_1, CMD_SET_2, CMD_SET_3, CMD_SET_4,
    CMD_SET_5, CMD_SET_6, CMD_SET_7, CMD_SET_8, CMD_SET_9,
    CMD_SET_NEXT, CMD_SET_PREV, CMD_ADD_REG, CMD_SUB_REG,
    CMD_INC_MULTI, CMD_DEC_MULTI, CMD_SET_MULTI,
    // Input/Output (20)
    CMD_OUT, CMD_IN, CMD_OUT_NUM, CMD_OUT_REG, CMD_IN_NUM,
    CMD_OUT_1, CMD_OUT_2, CMD_OUT_3, CMD_OUT_4, CMD_OUT_5,
    CMD_OUT_6, CMD_OUT_7, CMD_OUT_8, CMD_OUT_9,
    CMD_IN_1, CMD_IN_2, CMD_IN_3, CMD_IN_4, CMD_IN_5,
    CMD_IN_6, CMD_IN_7, CMD_IN_8, CMD_IN_9,
    // Control Flow (40)
    CMD_LOOP_START, CMD_LOOP_END, CMD_JUMP_FWD, CMD_JUMP_BACK,
    CMD_IF_ZERO, CMD_IF_NONZERO,
    CMD_JUMP_FWD_1, CMD_JUMP_FWD_2, CMD_JUMP_FWD_3, CMD_JUMP_FWD_4,
    CMD_JUMP_FWD_5, CMD_JUMP_FWD_6, CMD_JUMP_FWD_7, CMD_JUMP_FWD_8,
    CMD_JUMP_FWD_9,
    CMD_JUMP_BACK_1, CMD_JUMP_BACK_2, CMD_JUMP_BACK_3, CMD_JUMP_BACK_4,
    CMD_JUMP_BACK_5, CMD_JUMP_BACK_6, CMD_JUMP_BACK_7, CMD_JUMP_BACK_8,
    CMD_JUMP_BACK_9,
    CMD_SKIP_ZERO_1, CMD_SKIP_ZERO_2, CMD_SKIP_ZERO_3, CMD_SKIP_ZERO_4,
    CMD_SKIP_ZERO_5, CMD_SKIP_ZERO_6, CMD_SKIP_ZERO_7, CMD_SKIP_ZERO_8,
    CMD_SKIP_ZERO_9,
    CMD_SKIP_NONZERO_1, CMD_SKIP_NONZERO_2, CMD_SKIP_NONZERO_3, CMD_SKIP_NONZERO_4,
    CMD_SKIP_NONZERO_5, CMD_SKIP_NONZERO_6, CMD_SKIP_NONZERO_7, CMD_SKIP_NONZERO_8,
    CMD_SKIP_NONZERO_9,
    // Arithmetic (40)
    CMD_ADD_REG_ARITH, CMD_SUB_REG_ARITH, CMD_MUL_REG, CMD_DIV_REG,
    CMD_ADD_1, CMD_ADD_2, CMD_ADD_3, CMD_ADD_4, CMD_ADD_5,
    CMD_ADD_6, CMD_ADD_7, CMD_ADD_8, CMD_ADD_9,
    CMD_SUB_1, CMD_SUB_2, CMD_SUB_3, CMD_SUB_4, CMD_SUB_5,
    CMD_SUB_6, CMD_SUB_7, CMD_SUB_8, CMD_SUB_9,
    CMD_MUL_1, CMD_MUL_2, CMD_MUL_3, CMD_MUL_4, CMD_MUL_5,
    CMD_MUL_6, CMD_MUL_7, CMD_MUL_8, CMD_MUL_9,
    CMD_DIV_1, CMD_DIV_2, CMD_DIV_3, CMD_DIV_4, CMD_DIV_5,
    CMD_DIV_6, CMD_DIV_7, CMD_DIV_8, CMD_DIV_9,
    CMD_ADD_NEXT, CMD_SUB_NEXT,
    // Bitwise Operations (30)
    CMD_AND_REG, CMD_OR_REG, CMD_XOR_REG,
    CMD_LEFT_SHIFT_1, CMD_LEFT_SHIFT_2, CMD_LEFT_SHIFT_3, CMD_LEFT_SHIFT_4,
    CMD_LEFT_SHIFT_5, CMD_LEFT_SHIFT_6, CMD_LEFT_SHIFT_7, CMD_LEFT_SHIFT_8,
    CMD_RIGHT_SHIFT_1, CMD_RIGHT_SHIFT_2, CMD_RIGHT_SHIFT_3, CMD_RIGHT_SHIFT_4,
    CMD_RIGHT_SHIFT_5, CMD_RIGHT_SHIFT_6, CMD_RIGHT_SHIFT_7, CMD_RIGHT_SHIFT_8,
    CMD_NOT_CELL,
    CMD_AND_1, CMD_AND_2, CMD_AND_3, CMD_AND_4, CMD_AND_5,
    CMD_AND_6, CMD_AND_7, CMD_AND_8, CMD_AND_9,
    // Stack Operations (30)
    CMD_PUSH_CELL, CMD_POP_CELL, CMD_PUSH_REG, CMD_POP_REG, CMD_SWAP_STACK,
    CMD_PUSH_1, CMD_PUSH_2, CMD_PUSH_3, CMD_PUSH_4, CMD_PUSH_5,
    CMD_PUSH_6, CMD_PUSH_7, CMD_PUSH_8, CMD_PUSH_9,
    CMD_POP_1, CMD_POP_2, CMD_POP_3, CMD_POP_4, CMD_POP_5,
    CMD_POP_6, CMD_POP_7, CMD_POP_8, CMD_POP_9,
    CMD_CLEAR_STACK,
    // Register Operations (20)
    CMD_COPY_CELL, CMD_PASTE_REG, CMD_CLEAR_REG, CMD_INC_REG, CMD_DEC_REG,
    CMD_SET_REG_1, CMD_SET_REG_2, CMD_SET_REG_3, CMD_SET_REG_4,
    CMD_SET_REG_5, CMD_SET_REG_6, CMD_SET_REG_7, CMD_SET_REG_8,
    CMD_SET_REG_9,
    CMD_ADD_CELL_TO_REG, CMD_SUB_CELL_FROM_REG,
    // Utility (40)
    CMD_NOP, CMD_SWAP_CELL, CMD_REVERSE_BITS, CMD_RANDOM, CMD_HALT,
    CMD_SWAP_1, CMD_SWAP_2, CMD_SWAP_3, CMD_SWAP_4, CMD_SWAP_5,
    CMD_SWAP_6, CMD_SWAP_7, CMD_SWAP_8, CMD_SWAP_9,
    CMD_COPY_1, CMD_COPY_2, CMD_COPY_3, CMD_COPY_4, CMD_COPY_5,
    CMD_COPY_6, CMD_COPY_7, CMD_COPY_8, CMD_COPY_9,
    CMD_MOVE_1, CMD_MOVE_2, CMD_MOVE_3, CMD_MOVE_4, CMD_MOVE_5,
    CMD_MOVE_6, CMD_MOVE_7, CMD_MOVE_8, CMD_MOVE_9,
    CMD_XOR_1, CMD_XOR_2, CMD_XOR_3, CMD_XOR_4, CMD_XOR_5,
    CMD_XOR_6, CMD_XOR_7, CMD_XOR_8, CMD_XOR_9,
    // Functions (4)
    CMD_FUNC_DEF, CMD_FUNC_END, CMD_FUNC_CALL, CMD_FUNC_RET,
    // String Operations (3)
    CMD_STR_OUT, CMD_STR_LEN, CMD_STR_IN,
    CMD_UNKNOWN // For invalid commands
} CmdType;

// Command struct
typedef struct {
    char str[MAX_CMD_LEN + 1]; // Command string (e.g., "+108")
    CmdType type;              // Command type
    int value;                 // For parameterized commands (e.g., 108 for +108)
} Command;

// Function definition struct
typedef struct {
    char name[MAX_FUNC_NAME];  // Function name
    uint32_t start_pc;         // PC of first instruction in function
    uint32_t end_pc;           // PC of end/ret instruction
} Function;

// Interpreter state
typedef struct {
    uint8_t *memory;      // Memory tape
    uint32_t mem_size;    // Current tape size
    uint32_t ptr;         // Memory pointer
    uint8_t reg;          // Register
    uint8_t *stack;       // Value stack
    uint32_t stack_top;   // Stack top index
    uint32_t *loop_stack; // Loop stack for [] and ][
    uint32_t loop_top;    // Loop stack top index
    Command *program;     // Program commands
    uint32_t prog_len;    // Number of commands
    uint32_t pc;          // Program counter
    // Infinite loop detection
    uint64_t instruction_count;
    uint64_t max_instructions;
    time_t execution_start;
    int timeout_seconds;
    // Error handling
    ErrorCode error;
    char error_message[256];
    uint32_t error_pc;
    // Debugging
    int trace_mode;
    // Functions
    Function functions[MAX_FUNCTIONS];
    uint32_t func_count;
    uint32_t call_stack[MAX_CALL_STACK]; // Return addresses
    uint32_t call_top;
} CodeState;

// Function prototypes
CodeState* init_state();
void free_state(CodeState *state);
int parse_program(const char *input, CodeState *state);
int execute_program(CodeState *state);
int add_command(CodeState *state, const char *cmd_str, CmdType type, int value);
void set_error(CodeState *state, ErrorCode code, const char *message);
int check_memory_bounds(CodeState *state, uint32_t index, uint32_t count);
void trace_execution(CodeState *state);

#endif