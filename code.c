#include "code.h"
#include <time.h>
#include <ctype.h>

// Command lookup table (all 300 commands)
typedef struct {
    const char *str;
    CmdType type;
    int value;
} CmdEntry;

static CmdEntry cmd_table[] = {
    // Pointer Movement (30)
    {">>", CMD_RIGHT, 0}, {"<<", CMD_LEFT, 0},
    {">0", CMD_RIGHT_0, 0},
    {">1", CMD_RIGHT_1, 1}, {">2", CMD_RIGHT_2, 2}, {">3", CMD_RIGHT_3, 3},
    {">4", CMD_RIGHT_4, 4}, {">5", CMD_RIGHT_5, 5}, {">6", CMD_RIGHT_6, 6},
    {">7", CMD_RIGHT_7, 7}, {">8", CMD_RIGHT_8, 8}, {">9", CMD_RIGHT_9, 9},
    {"<1", CMD_LEFT_1, 1}, {"<2", CMD_LEFT_2, 2}, {"<3", CMD_LEFT_3, 3},
    {"<4", CMD_LEFT_4, 4}, {"<5", CMD_LEFT_5, 5}, {"<6", CMD_LEFT_6, 6},
    {"<7", CMD_LEFT_7, 7}, {"<8", CMD_LEFT_8, 8}, {"<9", CMD_LEFT_9, 9},
    {">a", CMD_RIGHT_REG, 0}, {"<a", CMD_LEFT_REG, 0},
    {">b", CMD_RIGHT_CELL, 0}, {"<b", CMD_LEFT_CELL, 0},
    {">x", CMD_RIGHT_ZERO, 0}, {"<x", CMD_LEFT_LAST, 0},
    {">r", CMD_RIGHT_RAND, 0}, {"<r", CMD_LEFT_RAND, 0},
    // Memory Manipulation (50)
    {"++", CMD_INC, 1}, {"--", CMD_DEC, 1},
    {"+1", CMD_INC_1, 1}, {"+2", CMD_INC_2, 2}, {"+3", CMD_INC_3, 3},
    {"+4", CMD_INC_4, 4}, {"+5", CMD_INC_5, 5}, {"+6", CMD_INC_6, 6},
    {"+7", CMD_INC_7, 7}, {"+8", CMD_INC_8, 8}, {"+9", CMD_INC_9, 9},
    {"-1", CMD_DEC_1, 1}, {"-2", CMD_DEC_2, 2}, {"-3", CMD_DEC_3, 3},
    {"-4", CMD_DEC_4, 4}, {"-5", CMD_DEC_5, 5}, {"-6", CMD_DEC_6, 6},
    {"-7", CMD_DEC_7, 7}, {"-8", CMD_DEC_8, 8}, {"-9", CMD_DEC_9, 9},
    {"=0", CMD_SET_ZERO, 0}, {"=r", CMD_SET_REG, 0},
    {"=1", CMD_SET_1, 1}, {"=2", CMD_SET_2, 2}, {"=3", CMD_SET_3, 3},
    {"=4", CMD_SET_4, 4}, {"=5", CMD_SET_5, 5}, {"=6", CMD_SET_6, 6},
    {"=7", CMD_SET_7, 7}, {"=8", CMD_SET_8, 8}, {"=9", CMD_SET_9, 9},
    {"=a", CMD_SET_NEXT, 0}, {"=b", CMD_SET_PREV, 0},
    {"+a", CMD_ADD_REG, 0}, {"-a", CMD_SUB_REG, 0},
    // Input/Output (20)
    {".", CMD_OUT, 0}, {",", CMD_IN, 0}, {":n", CMD_OUT_NUM, 0},
    {":r", CMD_OUT_REG, 0}, {",n", CMD_IN_NUM, 0},
    {".1", CMD_OUT_1, 1}, {".2", CMD_OUT_2, 2}, {".3", CMD_OUT_3, 3},
    {".4", CMD_OUT_4, 4}, {".5", CMD_OUT_5, 5}, {".6", CMD_OUT_6, 6},
    {".7", CMD_OUT_7, 7}, {".8", CMD_OUT_8, 8}, {".9", CMD_OUT_9, 9},
    {",1", CMD_IN_1, 1}, {",2", CMD_IN_2, 2}, {",3", CMD_IN_3, 3},
    {",4", CMD_IN_4, 4}, {",5", CMD_IN_5, 5}, {",6", CMD_IN_6, 6},
    {",7", CMD_IN_7, 7}, {",8", CMD_IN_8, 8}, {",9", CMD_IN_9, 9},
    // Control Flow (40)
    {"[", CMD_LOOP_START, 0}, {"]", CMD_LOOP_END, 0},
    {"j+", CMD_JUMP_FWD, 0}, {"j-", CMD_JUMP_BACK, 0},
    {"if0", CMD_IF_ZERO, 0}, {"ifn", CMD_IF_NONZERO, 0},
    {"j1", CMD_JUMP_FWD_1, 1}, {"j2", CMD_JUMP_FWD_2, 2}, {"j3", CMD_JUMP_FWD_3, 3},
    {"j4", CMD_JUMP_FWD_4, 4}, {"j5", CMD_JUMP_FWD_5, 5}, {"j6", CMD_JUMP_FWD_6, 6},
    {"j7", CMD_JUMP_FWD_7, 7}, {"j8", CMD_JUMP_FWD_8, 8}, {"j9", CMD_JUMP_FWD_9, 9},
    {"k1", CMD_JUMP_BACK_1, 1}, {"k2", CMD_JUMP_BACK_2, 2}, {"k3", CMD_JUMP_BACK_3, 3},
    {"k4", CMD_JUMP_BACK_4, 4}, {"k5", CMD_JUMP_BACK_5, 5}, {"k6", CMD_JUMP_BACK_6, 6},
    {"k7", CMD_JUMP_BACK_7, 7}, {"k8", CMD_JUMP_BACK_8, 8}, {"k9", CMD_JUMP_BACK_9, 9},
    {"i1", CMD_SKIP_ZERO_1, 1}, {"i2", CMD_SKIP_ZERO_2, 2}, {"i3", CMD_SKIP_ZERO_3, 3},
    {"i4", CMD_SKIP_ZERO_4, 4}, {"i5", CMD_SKIP_ZERO_5, 5}, {"i6", CMD_SKIP_ZERO_6, 6},
    {"i7", CMD_SKIP_ZERO_7, 7}, {"i8", CMD_SKIP_ZERO_8, 8}, {"i9", CMD_SKIP_ZERO_9, 9},
    {"n1", CMD_SKIP_NONZERO_1, 1}, {"n2", CMD_SKIP_NONZERO_2, 2}, {"n3", CMD_SKIP_NONZERO_3, 3},
    {"n4", CMD_SKIP_NONZERO_4, 4}, {"n5", CMD_SKIP_NONZERO_5, 5}, {"n6", CMD_SKIP_NONZERO_6, 6},
    {"n7", CMD_SKIP_NONZERO_7, 7}, {"n8", CMD_SKIP_NONZERO_8, 8}, {"n9", CMD_SKIP_NONZERO_9, 9},
    // Arithmetic (40)
    {"a+", CMD_ADD_REG_ARITH, 0}, {"a-", CMD_SUB_REG_ARITH, 0}, {"m*", CMD_MUL_REG, 0},
    {"d/", CMD_DIV_REG, 0},
    {"a1", CMD_ADD_1, 1}, {"a2", CMD_ADD_2, 2}, {"a3", CMD_ADD_3, 3},
    {"a4", CMD_ADD_4, 4}, {"a5", CMD_ADD_5, 5}, {"a6", CMD_ADD_6, 6},
    {"a7", CMD_ADD_7, 7}, {"a8", CMD_ADD_8, 8}, {"a9", CMD_ADD_9, 9},
    {"s1", CMD_SUB_1, 1}, {"s2", CMD_SUB_2, 2}, {"s3", CMD_SUB_3, 3},
    {"s4", CMD_SUB_4, 4}, {"s5", CMD_SUB_5, 5}, {"s6", CMD_SUB_6, 6},
    {"s7", CMD_SUB_7, 7}, {"s8", CMD_SUB_8, 8}, {"s9", CMD_SUB_9, 9},
    {"m1", CMD_MUL_1, 1}, {"m2", CMD_MUL_2, 2}, {"m3", CMD_MUL_3, 3},
    {"m4", CMD_MUL_4, 4}, {"m5", CMD_MUL_5, 5}, {"m6", CMD_MUL_6, 6},
    {"m7", CMD_MUL_7, 7}, {"m8", CMD_MUL_8, 8}, {"m9", CMD_MUL_9, 9},
    {"d1", CMD_DIV_1, 1}, {"d2", CMD_DIV_2, 2}, {"d3", CMD_DIV_3, 3},
    {"d4", CMD_DIV_4, 4}, {"d5", CMD_DIV_5, 5}, {"d6", CMD_DIV_6, 6},
    {"d7", CMD_DIV_7, 7}, {"d8", CMD_DIV_8, 8}, {"d9", CMD_DIV_9, 9},
    {"+c", CMD_ADD_NEXT, 0}, {"-c", CMD_SUB_NEXT, 0},
    // Bitwise Operations (30)
    {"&r", CMD_AND_REG, 0}, {"|r", CMD_OR_REG, 0}, {"^r", CMD_XOR_REG, 0},
    {"<<1", CMD_LEFT_SHIFT_1, 1}, {"<<2", CMD_LEFT_SHIFT_2, 2}, {"<<3", CMD_LEFT_SHIFT_3, 3},
    {"<<4", CMD_LEFT_SHIFT_4, 4}, {"<<5", CMD_LEFT_SHIFT_5, 5}, {"<<6", CMD_LEFT_SHIFT_6, 6},
    {"<<7", CMD_LEFT_SHIFT_7, 7}, {"<<8", CMD_LEFT_SHIFT_8, 8},
    {">>1", CMD_RIGHT_SHIFT_1, 1}, {">>2", CMD_RIGHT_SHIFT_2, 2}, {">>3", CMD_RIGHT_SHIFT_3, 3},
    {">>4", CMD_RIGHT_SHIFT_4, 4}, {">>5", CMD_RIGHT_SHIFT_5, 5}, {">>6", CMD_RIGHT_SHIFT_6, 6},
    {">>7", CMD_RIGHT_SHIFT_7, 7}, {">>8", CMD_RIGHT_SHIFT_8, 8},
    {"~c", CMD_NOT_CELL, 0},
    {"&1", CMD_AND_1, 1}, {"&2", CMD_AND_2, 2}, {"&3", CMD_AND_3, 3},
    {"&4", CMD_AND_4, 4}, {"&5", CMD_AND_5, 5}, {"&6", CMD_AND_6, 6},
    {"&7", CMD_AND_7, 7}, {"&8", CMD_AND_8, 8}, {"&9", CMD_AND_9, 9},
    // Stack Operations (30)
    {"ps", CMD_PUSH_CELL, 0}, {"pp", CMD_POP_CELL, 0}, {"pr", CMD_PUSH_REG, 0},
    {"sr", CMD_POP_REG, 0}, {"sp", CMD_SWAP_STACK, 0},
    {"p1", CMD_PUSH_1, 1}, {"p2", CMD_PUSH_2, 2}, {"p3", CMD_PUSH_3, 3},
    {"p4", CMD_PUSH_4, 4}, {"p5", CMD_PUSH_5, 5}, {"p6", CMD_PUSH_6, 6},
    {"p7", CMD_PUSH_7, 7}, {"p8", CMD_PUSH_8, 8}, {"p9", CMD_PUSH_9, 9},
    {"s1", CMD_POP_1, 1}, {"s2", CMD_POP_2, 2}, {"s3", CMD_POP_3, 3},
    {"s4", CMD_POP_4, 4}, {"s5", CMD_POP_5, 5}, {"s6", CMD_POP_6, 6},
    {"s7", CMD_POP_7, 7}, {"s8", CMD_POP_8, 8}, {"s9", CMD_POP_9, 9},
    {"sc", CMD_CLEAR_STACK, 0},
    // Register Operations (20)
    {"cp", CMD_COPY_CELL, 0}, {"pt", CMD_PASTE_REG, 0}, {"cr", CMD_CLEAR_REG, 0},
    {"+r", CMD_INC_REG, 1}, {"-r", CMD_DEC_REG, 1},
    {"r1", CMD_SET_REG_1, 1}, {"r2", CMD_SET_REG_2, 2}, {"r3", CMD_SET_REG_3, 3},
    {"r4", CMD_SET_REG_4, 4}, {"r5", CMD_SET_REG_5, 5}, {"r6", CMD_SET_REG_6, 6},
    {"r7", CMD_SET_REG_7, 7}, {"r8", CMD_SET_REG_8, 8}, {"r9", CMD_SET_REG_9, 9},
    {"ra", CMD_ADD_CELL_TO_REG, 0}, {"rs", CMD_SUB_CELL_FROM_REG, 0},
    // Utility (40)
    {"nop", CMD_NOP, 0}, {"sw", CMD_SWAP_CELL, 0}, {"rv", CMD_REVERSE_BITS, 0},
    {"rnd", CMD_RANDOM, 0}, {"hlt", CMD_HALT, 0},
    {"sw1", CMD_SWAP_1, 1}, {"sw2", CMD_SWAP_2, 2}, {"sw3", CMD_SWAP_3, 3},
    {"sw4", CMD_SWAP_4, 4}, {"sw5", CMD_SWAP_5, 5}, {"sw6", CMD_SWAP_6, 6},
    {"sw7", CMD_SWAP_7, 7}, {"sw8", CMD_SWAP_8, 8}, {"sw9", CMD_SWAP_9, 9},
    {"c1", CMD_COPY_1, 1}, {"c2", CMD_COPY_2, 2}, {"c3", CMD_COPY_3, 3},
    {"c4", CMD_COPY_4, 4}, {"c5", CMD_COPY_5, 5}, {"c6", CMD_COPY_6, 6},
    {"c7", CMD_COPY_7, 7}, {"c8", CMD_COPY_8, 8}, {"c9", CMD_COPY_9, 9},
    {"mv1", CMD_MOVE_1, 1}, {"mv2", CMD_MOVE_2, 2}, {"mv3", CMD_MOVE_3, 3},
    {"mv4", CMD_MOVE_4, 4}, {"mv5", CMD_MOVE_5, 5}, {"mv6", CMD_MOVE_6, 6},
    {"mv7", CMD_MOVE_7, 7}, {"mv8", CMD_MOVE_8, 8}, {"mv9", CMD_MOVE_9, 9},
    {"x1", CMD_XOR_1, 1}, {"x2", CMD_XOR_2, 2}, {"x3", CMD_XOR_3, 3},
    {"x4", CMD_XOR_4, 4}, {"x5", CMD_XOR_5, 5}, {"x6", CMD_XOR_6, 6},
    {"x7", CMD_XOR_7, 7}, {"x8", CMD_XOR_8, 8}, {"x9", CMD_XOR_9, 9},
    // Functions (4) - def/call are handled specially in parser for names
    {"def", CMD_FUNC_DEF, 0}, {"end", CMD_FUNC_END, 0},
    {"call", CMD_FUNC_CALL, 0}, {"ret", CMD_FUNC_RET, 0},
    // String Operations (3)
    {"sout", CMD_STR_OUT, 0}, {"slen", CMD_STR_LEN, 0}, {"sin", CMD_STR_IN, 0},
    {NULL, CMD_UNKNOWN, 0}
};

// Initialize interpreter state
CodeState* init_state() {
    CodeState *state = (CodeState*)malloc(sizeof(CodeState));
    if (!state) return NULL;

    state->mem_size = MAX_MEMORY;
    state->memory = (uint8_t*)calloc(state->mem_size, sizeof(uint8_t));
    state->ptr = 0;
    state->reg = 0;
    state->stack = (uint8_t*)malloc(MAX_STACK * sizeof(uint8_t));
    state->stack_top = 0;
    state->loop_stack = (uint32_t*)malloc(MAX_STACK * sizeof(uint32_t));
    state->loop_top = 0;
    state->program = (Command*)malloc(MAX_PROG * sizeof(Command));
    state->prog_len = 0;
    state->pc = 0;
    // Initialize infinite loop detection
    state->instruction_count = 0;
    state->max_instructions = 10000000; // 10M instruction limit
    state->execution_start = time(NULL);
    state->timeout_seconds = 30; // 30-second timeout
    // Initialize error handling
    state->error = ERR_NONE;
    state->error_message[0] = '\0';
    state->error_pc = 0;
    // Initialize debugging
    state->trace_mode = 0; // Off by default, enable with -d flag
    // Initialize functions
    state->func_count = 0;
    state->call_top = 0;

    if (!state->memory || !state->stack || !state->loop_stack || !state->program) {
        free_state(state);
        return NULL;
    }
    srand(time(NULL)); // For rnd command
    return state;
}

// Free state
void free_state(CodeState *state) {
    if (state) {
        free(state->memory);
        free(state->stack);
        free(state->loop_stack);
        free(state->program);
        free(state);
    }
}

// Set error
void set_error(CodeState *state, ErrorCode code, const char *message) {
    state->error = code;
    state->error_pc = state->pc;
    snprintf(state->error_message, sizeof(state->error_message), "%s", message);
    if (state->trace_mode) {
        fprintf(stderr, "Error at PC %u: %s\n", state->error_pc, state->error_message);
    }
}

// Check memory bounds
int check_memory_bounds(CodeState *state, uint32_t index, uint32_t count) {
    if (index + count >= state->mem_size) {
        uint32_t required_size = index + count + 1;
        uint32_t new_size = state->mem_size;
        while (new_size < required_size) new_size *= 2;

        uint8_t *new_mem = (uint8_t*)realloc(state->memory, new_size);
        if (!new_mem) {
            set_error(state, ERR_MEMORY_ALLOC, "Failed to allocate memory");
            return 0;
        }

        memset(new_mem + state->mem_size, 0, new_size - state->mem_size);
        state->memory = new_mem;
        state->mem_size = new_size;
        if (state->trace_mode) {
            fprintf(stderr, "Memory expanded to %u bytes\n", new_size);
        }
    }
    return 1;
}

// Trace execution
void trace_execution(CodeState *state) {
    fprintf(stderr, "PC: %u, CMD: %s, PTR: %u, VAL: %u, REG: %u\n",
            state->pc, state->program[state->pc].str,
            state->ptr, state->memory[state->ptr], state->reg);
    fprintf(stderr, "Memory: ");
    uint32_t start = (state->ptr > 5) ? state->ptr - 5 : 0;
    uint32_t end = (state->ptr + 5 < state->mem_size) ? state->ptr + 5 : state->mem_size - 1;
    for (uint32_t i = start; i <= end; i++) {
        fprintf(stderr, "%s%3u", (i == state->ptr ? "[" : " "), state->memory[i]);
        fprintf(stderr, "%s", (i == state->ptr ? "]" : " "));
    }
    fprintf(stderr, "\n");
}

// Add command to program
int add_command(CodeState *state, const char *cmd_str, CmdType type, int value) {
    if (state->prog_len >= MAX_PROG) {
        set_error(state, ERR_MEMORY_ALLOC, "Program too long");
        return 0;
    }
    strncpy(state->program[state->prog_len].str, cmd_str, MAX_CMD_LEN);
    state->program[state->prog_len].str[MAX_CMD_LEN] = '\0';
    state->program[state->prog_len].type = type;
    state->program[state->prog_len].value = value;
    if (state->trace_mode) {
        fprintf(stderr, "Parsed: %s, Type: %d, Value: %d\n", cmd_str, type, value);
    }
    state->prog_len++;
    return 1;
}

// Parse program
int parse_program(const char *input, CodeState *state) {
    size_t i = 0;
    size_t len = strlen(input);

    while (i < len) {
        // Skip whitespace
        if (isspace(input[i])) {
            i++;
            continue;
        }

        // Skip comments
        if (input[i] == '#') {
            while (i < len && input[i] != '\n') {
                i++;
            }
            continue;
        }

        // Try parsing multi-digit increment (+NN), decrement (-NN), or set (=NN)
        if ((input[i] == '+' || input[i] == '-' || input[i] == '=') && i + 1 < len && isdigit(input[i + 1])) {
            char prefix = input[i];
            size_t j = i + 1;

            // Collect digits (up to 3 for values 0-255)
            while (j < len && isdigit(input[j]) && j - i <= 4) {
                j++;
            }

            // Check if next char is NOT a digit (end of number)
            if (j < len && isdigit(input[j])) {
                // More than 3 digits, not a valid number command
            } else {
                char num_buf[8] = {0};
                strncpy(num_buf, input + i + 1, j - i - 1);
                long val = strtol(num_buf, NULL, 10);

                if (val >= 0 && val <= 255) {
                    char cmd_str[8] = {0};
                    snprintf(cmd_str, sizeof(cmd_str), "%c%ld", prefix, val);
                    CmdType type = (prefix == '+') ? CMD_INC_MULTI :
                                  (prefix == '-') ? CMD_DEC_MULTI : CMD_SET_MULTI;
                    if (!add_command(state, cmd_str, type, (int)val)) {
                        return 0;
                    }
                    i = j;
                    continue;
                }
            }
        }

        // Try to find longest matching command
        int best_match_len = 0;
        const char *best_match_str = NULL;
        CmdType best_match_type = CMD_UNKNOWN;
        int best_match_value = 0;

        for (int j = 0; cmd_table[j].str; j++) {
            int cmd_len = strlen(cmd_table[j].str);
            if (cmd_len > best_match_len && i + cmd_len <= len) {
                if (strncmp(input + i, cmd_table[j].str, cmd_len) == 0) {
                    best_match_len = cmd_len;
                    best_match_str = cmd_table[j].str;
                    best_match_type = cmd_table[j].type;
                    best_match_value = cmd_table[j].value;
                }
            }
        }

        if (best_match_len > 0) {
            // Special handling for function definition and call (need to read name)
            if (best_match_type == CMD_FUNC_DEF || best_match_type == CMD_FUNC_CALL) {
                i += best_match_len;
                // Skip whitespace after def/call
                while (i < len && (input[i] == ' ' || input[i] == '\t')) i++;
                // Read function name (alphanumeric + underscore)
                char func_name[MAX_FUNC_NAME] = {0};
                size_t name_i = 0;
                while (i < len && name_i < MAX_FUNC_NAME - 1 &&
                       (isalnum(input[i]) || input[i] == '_')) {
                    func_name[name_i++] = input[i++];
                }
                func_name[name_i] = '\0';
                if (name_i == 0) {
                    fprintf(stderr, "Error: Expected function name after %s\n", best_match_str);
                    return 0;
                }
                if (best_match_type == CMD_FUNC_DEF) {
                    // Register new function
                    if (state->func_count >= MAX_FUNCTIONS) {
                        set_error(state, ERR_FUNC_OVERFLOW, "Too many functions");
                        return 0;
                    }
                    memcpy(state->functions[state->func_count].name, func_name, MAX_FUNC_NAME);
                    state->functions[state->func_count].name[MAX_FUNC_NAME - 1] = '\0';
                    // start_pc will be set after we add the def command (prog_len + 1)
                    state->functions[state->func_count].end_pc = 0; // Set later by 'end'
                    if (!add_command(state, best_match_str, best_match_type, state->func_count)) {
                        return 0;
                    }
                    state->functions[state->func_count].start_pc = state->prog_len; // First instruction in function body
                    state->func_count++;
                } else { // CMD_FUNC_CALL
                    // Look up function by name (might be forward reference)
                    int func_idx = -1;
                    for (uint32_t fi = 0; fi < state->func_count; fi++) {
                        if (strcmp(state->functions[fi].name, func_name) == 0) {
                            func_idx = fi;
                            break;
                        }
                    }
                    if (func_idx < 0) {
                        // Forward reference: register a placeholder
                        if (state->func_count >= MAX_FUNCTIONS) {
                            set_error(state, ERR_FUNC_OVERFLOW, "Too many functions");
                            return 0;
                        }
                        func_idx = state->func_count;
                        memcpy(state->functions[func_idx].name, func_name, MAX_FUNC_NAME);
                        state->functions[func_idx].name[MAX_FUNC_NAME - 1] = '\0';
                        state->functions[func_idx].start_pc = 0; // Will be set by def
                        state->functions[func_idx].end_pc = 0;
                        state->func_count++;
                    }
                    if (!add_command(state, best_match_str, best_match_type, func_idx)) {
                        return 0;
                    }
                }
                continue;
            }
            // Special handling for 'end' - find the function to set end_pc
            if (best_match_type == CMD_FUNC_END) {
                // Find most recent function without end_pc
                for (int fi = state->func_count - 1; fi >= 0; fi--) {
                    if (state->functions[fi].end_pc == 0 && state->functions[fi].start_pc > 0) {
                        state->functions[fi].end_pc = state->prog_len; // This 'end' command
                        break;
                    }
                }
            }
            if (!add_command(state, best_match_str, best_match_type, best_match_value)) {
                return 0;
            }
            i += best_match_len;
            continue;
        }

        // Handle single-character commands that might not be in table
        if (input[i] == '[') {
            if (!add_command(state, "[", CMD_LOOP_START, 0)) {
                return 0;
            }
            i++;
            continue;
        }
        if (input[i] == ']') {
            if (!add_command(state, "]", CMD_LOOP_END, 0)) {
                return 0;
            }
            i++;
            continue;
        }
        if (input[i] == '.') {
            if (!add_command(state, ".", CMD_OUT, 0)) {
                return 0;
            }
            i++;
            continue;
        }
        if (input[i] == ',') {
            if (!add_command(state, ",", CMD_IN, 0)) {
                return 0;
            }
            i++;
            continue;
        }

        // Unknown character, skip it
        if (state->trace_mode) {
            fprintf(stderr, "Warning: Unknown character '%c' (0x%02X) at position %zu, skipping\n",
                    isprint(input[i]) ? input[i] : '.', (unsigned char)input[i], i);
        }
        i++;
    }

    return 1;
}

// Execute program
int execute_program(CodeState *state) {
    state->instruction_count = 0;
    state->execution_start = time(NULL);

    while (state->pc < state->prog_len) {
        Command *cmd = &state->program[state->pc];
        if (state->trace_mode) {
            trace_execution(state);
        }

        // Infinite loop detection
        state->instruction_count++;
        if (state->instruction_count % 1000000 == 0) {
            if (difftime(time(NULL), state->execution_start) > state->timeout_seconds) {
                set_error(state, ERR_TIMEOUT, "Execution timeout");
                return 0;
            }
            if (state->instruction_count > state->max_instructions) {
                set_error(state, ERR_INSTRUCTION_LIMIT, "Instruction limit exceeded");
                return 0;
            }
        }

        switch (cmd->type) {
            // Pointer Movement
            case CMD_RIGHT:
                if (!check_memory_bounds(state, state->ptr + 1, 1)) return 0;
                state->ptr++;
                break;
            case CMD_LEFT:
                if (state->ptr > 0) state->ptr--;
                break;
            case CMD_RIGHT_0:
                state->ptr = 0; // Move pointer to position 0
                break;
            case CMD_RIGHT_1: case CMD_RIGHT_2: case CMD_RIGHT_3: case CMD_RIGHT_4:
            case CMD_RIGHT_5: case CMD_RIGHT_6: case CMD_RIGHT_7: case CMD_RIGHT_8:
            case CMD_RIGHT_9:
                if (!check_memory_bounds(state, state->ptr + cmd->value, 1)) return 0;
                state->ptr += cmd->value;
                break;
            case CMD_LEFT_1: case CMD_LEFT_2: case CMD_LEFT_3: case CMD_LEFT_4:
            case CMD_LEFT_5: case CMD_LEFT_6: case CMD_LEFT_7: case CMD_LEFT_8:
            case CMD_LEFT_9:
                if (state->ptr >= (uint32_t)cmd->value) {
                    state->ptr -= cmd->value;
                }
                break;
            case CMD_RIGHT_REG:
                if (!check_memory_bounds(state, state->ptr + state->reg, 1)) return 0;
                state->ptr += state->reg;
                break;
            case CMD_LEFT_REG:
                if (state->ptr >= state->reg) state->ptr -= state->reg;
                break;
            case CMD_RIGHT_CELL:
                if (!check_memory_bounds(state, state->ptr + state->memory[state->ptr], 1)) return 0;
                state->ptr += state->memory[state->ptr];
                break;
            case CMD_LEFT_CELL:
                if (state->ptr >= state->memory[state->ptr]) state->ptr -= state->memory[state->ptr];
                break;
            case CMD_RIGHT_ZERO:
                state->ptr = 0;
                break;
            case CMD_LEFT_LAST:
                while (state->ptr > 0 && state->memory[state->ptr] == 0) state->ptr--;
                break;
            case CMD_RIGHT_RAND:
                if (!check_memory_bounds(state, state->ptr + (rand() % 10), 1)) return 0;
                state->ptr += rand() % 10;
                break;
            case CMD_LEFT_RAND:
                if (state->ptr >= (uint32_t)(rand() % 10)) state->ptr -= rand() % 10;
                break;

            // Memory Manipulation
            case CMD_INC:
                {
                    int count = 1;
                    while (state->pc + count < state->prog_len &&
                           state->program[state->pc + count].type == CMD_INC) {
                        count++;
                    }
                    state->memory[state->ptr] += count;
                    state->pc += count - 1;
                }
                break;
            case CMD_DEC:
                state->memory[state->ptr] -= 1;
                break;
            case CMD_INC_1: case CMD_INC_2: case CMD_INC_3: case CMD_INC_4:
            case CMD_INC_5: case CMD_INC_6: case CMD_INC_7: case CMD_INC_8:
            case CMD_INC_9:
            case CMD_INC_MULTI:
                state->memory[state->ptr] = (uint8_t)(state->memory[state->ptr] + cmd->value);
                break;
            case CMD_DEC_1: case CMD_DEC_2: case CMD_DEC_3: case CMD_DEC_4:
            case CMD_DEC_5: case CMD_DEC_6: case CMD_DEC_7: case CMD_DEC_8:
            case CMD_DEC_9:
            case CMD_DEC_MULTI:
                state->memory[state->ptr] -= cmd->value;
                break;
            case CMD_SET_ZERO:
                state->memory[state->ptr] = 0;
                break;
            case CMD_SET_REG:
                state->memory[state->ptr] = state->reg;
                break;
            case CMD_SET_1: case CMD_SET_2: case CMD_SET_3: case CMD_SET_4:
            case CMD_SET_5: case CMD_SET_6: case CMD_SET_7: case CMD_SET_8:
            case CMD_SET_9:
            case CMD_SET_MULTI:
                state->memory[state->ptr] = cmd->value;
                break;
            case CMD_SET_NEXT:
                if (!check_memory_bounds(state, state->ptr + 1, 1)) return 0;
                state->memory[state->ptr] = state->memory[state->ptr + 1];
                break;
            case CMD_SET_PREV:
                if (state->ptr > 0) state->memory[state->ptr] = state->memory[state->ptr - 1];
                break;
            case CMD_ADD_REG:
                state->memory[state->ptr] += state->reg;
                break;
            case CMD_SUB_REG:
                state->memory[state->ptr] -= state->reg;
                break;

            // Input/Output
            case CMD_OUT:
                putchar(state->memory[state->ptr]);
                fflush(stdout);
                break;
            case CMD_IN:
                {
                    int c = getchar();
                    if (c != EOF) {
                        state->memory[state->ptr] = (uint8_t)c;
                    } else {
                        state->memory[state->ptr] = 0;
                    }
                }
                break;
            case CMD_OUT_NUM:
                printf("%d", state->memory[state->ptr]);
                fflush(stdout);
                break;
            case CMD_OUT_REG:
                putchar(state->reg);
                fflush(stdout);
                break;
            case CMD_IN_NUM:
                {
                    if (DEBUG_MODE) {
                        fprintf(stderr, "Executing CMD_IN_NUM at PC %u, waiting for input\n", state->pc);
                    }
                    int n;
                    printf("Enter number: ");
                    fflush(stdout);
                    if (scanf("%d", &n) == 1 && n >= 0 && n <= 255) {
                        state->memory[state->ptr] = (uint8_t)n;
                        if (DEBUG_MODE) {
                            fprintf(stderr, "Read input %d for CMD_IN_NUM at PC %u\n", n, state->pc);
                        }
                    } else {
                        if (DEBUG_MODE) {
                            fprintf(stderr, "Warning: Invalid input for CMD_IN_NUM at PC %u, setting to 0\n", state->pc);
                        }
                        state->memory[state->ptr] = 0;
                    }
                    while (getchar() != '\n'); // Clear input buffer
                }
                break;
            case CMD_OUT_1: case CMD_OUT_2: case CMD_OUT_3: case CMD_OUT_4:
            case CMD_OUT_5: case CMD_OUT_6: case CMD_OUT_7: case CMD_OUT_8:
            case CMD_OUT_9:
                for (int i = 0; i < cmd->value; i++) putchar(state->memory[state->ptr]);
                fflush(stdout);
                break;
            case CMD_IN_1: case CMD_IN_2: case CMD_IN_3: case CMD_IN_4:
            case CMD_IN_5: case CMD_IN_6: case CMD_IN_7: case CMD_IN_8:
            case CMD_IN_9:
                for (int i = 0; i < cmd->value; i++) {
                    if (!check_memory_bounds(state, state->ptr + i, 1)) return 0;
                    int c = getchar();
                    if (c != EOF) state->memory[state->ptr + i] = (uint8_t)c;
                }
                break;

            // Control Flow
            case CMD_LOOP_START:
                if (state->memory[state->ptr] == 0) {
                    int depth = 1;
                    while (state->pc < state->prog_len && depth > 0) {
                        state->pc++;
                        if (state->pc >= state->prog_len) {
                            set_error(state, ERR_LOOP_MISMATCH, "Mismatched loop brackets");
                            return 0;
                        }
                        if (state->program[state->pc].type == CMD_LOOP_START) depth++;
                        if (state->program[state->pc].type == CMD_LOOP_END) depth--;
                    }
                } else {
                    if (state->loop_top >= MAX_STACK) {
                        set_error(state, ERR_STACK_OVERFLOW, "Loop stack overflow");
                        return 0;
                    }
                    state->loop_stack[state->loop_top++] = state->pc;
                }
                break;
            case CMD_LOOP_END:
                if (state->memory[state->ptr] != 0) {
                    if (state->loop_top == 0) {
                        set_error(state, ERR_STACK_UNDERFLOW, "Loop stack underflow");
                        return 0;
                    }
                    state->pc = state->loop_stack[state->loop_top - 1];
                } else {
                    if (state->loop_top > 0) state->loop_top--;
                }
                break;
            case CMD_JUMP_FWD:
                if (state->memory[state->ptr] != 0) {
                    int depth = 1;
                    while (state->pc < state->prog_len && depth > 0) {
                        state->pc++;
                        if (state->pc >= state->prog_len) {
                            set_error(state, ERR_LOOP_MISMATCH, "Mismatched jump brackets");
                            return 0;
                        }
                        if (state->program[state->pc].type == CMD_JUMP_FWD) depth++;
                        if (state->program[state->pc].type == CMD_JUMP_BACK) depth--;
                    }
                }
                break;
            case CMD_JUMP_BACK:
                if (state->memory[state->ptr] != 0) {
                    int depth = 1;
                    while (state->pc > 0 && depth > 0) {
                        state->pc--;
                        if (state->program[state->pc].type == CMD_JUMP_BACK) depth++;
                        if (state->program[state->pc].type == CMD_JUMP_FWD) depth--;
                    }
                }
                break;
            case CMD_IF_ZERO:
                if (state->memory[state->ptr] == 0) state->pc++;
                break;
            case CMD_IF_NONZERO:
                if (state->memory[state->ptr] != 0) state->pc++;
                break;
            case CMD_JUMP_FWD_1: case CMD_JUMP_FWD_2: case CMD_JUMP_FWD_3:
            case CMD_JUMP_FWD_4: case CMD_JUMP_FWD_5: case CMD_JUMP_FWD_6:
            case CMD_JUMP_FWD_7: case CMD_JUMP_FWD_8: case CMD_JUMP_FWD_9:
                if (state->memory[state->ptr] != 0) state->pc += cmd->value;
                break;
            case CMD_JUMP_BACK_1: case CMD_JUMP_BACK_2: case CMD_JUMP_BACK_3:
            case CMD_JUMP_BACK_4: case CMD_JUMP_BACK_5: case CMD_JUMP_BACK_6:
            case CMD_JUMP_BACK_7: case CMD_JUMP_BACK_8: case CMD_JUMP_BACK_9:
                if (state->memory[state->ptr] != 0 && state->pc >= (uint32_t)cmd->value)
                    state->pc -= cmd->value;
                break;
            case CMD_SKIP_ZERO_1: case CMD_SKIP_ZERO_2: case CMD_SKIP_ZERO_3:
            case CMD_SKIP_ZERO_4: case CMD_SKIP_ZERO_5: case CMD_SKIP_ZERO_6:
            case CMD_SKIP_ZERO_7: case CMD_SKIP_ZERO_8: case CMD_SKIP_ZERO_9:
                if (state->memory[state->ptr] == 0) state->pc += cmd->value;
                break;
            case CMD_SKIP_NONZERO_1: case CMD_SKIP_NONZERO_2: case CMD_SKIP_NONZERO_3:
            case CMD_SKIP_NONZERO_4: case CMD_SKIP_NONZERO_5: case CMD_SKIP_NONZERO_6:
            case CMD_SKIP_NONZERO_7: case CMD_SKIP_NONZERO_8: case CMD_SKIP_NONZERO_9:
                if (state->memory[state->ptr] != 0) state->pc += cmd->value;
                break;

            // Arithmetic
            case CMD_ADD_REG_ARITH:
                state->memory[state->ptr] += state->reg;
                break;
            case CMD_SUB_REG_ARITH:
                state->memory[state->ptr] -= state->reg;
                break;
            case CMD_MUL_REG:
                state->memory[state->ptr] *= state->reg;
                break;
            case CMD_DIV_REG:
                if (state->reg == 0) {
                    set_error(state, ERR_DIV_BY_ZERO, "Division by zero");
                    return 0;
                }
                state->memory[state->ptr] /= state->reg;
                break;
            case CMD_ADD_1: case CMD_ADD_2: case CMD_ADD_3: case CMD_ADD_4:
            case CMD_ADD_5: case CMD_ADD_6: case CMD_ADD_7: case CMD_ADD_8:
            case CMD_ADD_9:
                state->memory[state->ptr] += cmd->value;
                break;
            case CMD_SUB_1: case CMD_SUB_2: case CMD_SUB_3: case CMD_SUB_4:
            case CMD_SUB_5: case CMD_SUB_6: case CMD_SUB_7: case CMD_SUB_8:
            case CMD_SUB_9:
                state->memory[state->ptr] -= cmd->value;
                break;
            case CMD_MUL_1: case CMD_MUL_2: case CMD_MUL_3: case CMD_MUL_4:
            case CMD_MUL_5: case CMD_MUL_6: case CMD_MUL_7: case CMD_MUL_8:
            case CMD_MUL_9:
                state->memory[state->ptr] *= cmd->value;
                break;
            case CMD_DIV_1: case CMD_DIV_2: case CMD_DIV_3: case CMD_DIV_4:
            case CMD_DIV_5: case CMD_DIV_6: case CMD_DIV_7: case CMD_DIV_8:
            case CMD_DIV_9:
                if (cmd->value == 0) {
                    set_error(state, ERR_DIV_BY_ZERO, "Division by zero");
                    return 0;
                }
                state->memory[state->ptr] /= cmd->value;
                break;
            case CMD_ADD_NEXT:
                if (!check_memory_bounds(state, state->ptr + 1, 1)) return 0;
                state->memory[state->ptr] += state->memory[state->ptr + 1];
                break;
            case CMD_SUB_NEXT:
                if (!check_memory_bounds(state, state->ptr + 1, 1)) return 0;
                state->memory[state->ptr] -= state->memory[state->ptr + 1];
                break;

            // Bitwise Operations
            case CMD_AND_REG:
                state->memory[state->ptr] &= state->reg;
                break;
            case CMD_OR_REG:
                state->memory[state->ptr] |= state->reg;
                break;
            case CMD_XOR_REG:
                state->memory[state->ptr] ^= state->reg;
                break;
            case CMD_LEFT_SHIFT_1: case CMD_LEFT_SHIFT_2: case CMD_LEFT_SHIFT_3:
            case CMD_LEFT_SHIFT_4: case CMD_LEFT_SHIFT_5: case CMD_LEFT_SHIFT_6:
            case CMD_LEFT_SHIFT_7: case CMD_LEFT_SHIFT_8:
                state->memory[state->ptr] <<= cmd->value;
                break;
            case CMD_RIGHT_SHIFT_1: case CMD_RIGHT_SHIFT_2: case CMD_RIGHT_SHIFT_3:
            case CMD_RIGHT_SHIFT_4: case CMD_RIGHT_SHIFT_5: case CMD_RIGHT_SHIFT_6:
            case CMD_RIGHT_SHIFT_7: case CMD_RIGHT_SHIFT_8:
                state->memory[state->ptr] >>= cmd->value;
                break;
            case CMD_NOT_CELL:
                state->memory[state->ptr] = ~state->memory[state->ptr];
                break;
            case CMD_AND_1: case CMD_AND_2: case CMD_AND_3: case CMD_AND_4:
            case CMD_AND_5: case CMD_AND_6: case CMD_AND_7: case CMD_AND_8:
            case CMD_AND_9:
                state->memory[state->ptr] &= cmd->value;
                break;

            // Stack Operations
            case CMD_PUSH_CELL:
                if (state->stack_top >= MAX_STACK) {
                    set_error(state, ERR_STACK_OVERFLOW, "Stack overflow");
                    return 0;
                }
                state->stack[state->stack_top++] = state->memory[state->ptr];
                break;
            case CMD_POP_CELL:
                if (state->stack_top == 0) {
                    set_error(state, ERR_STACK_UNDERFLOW, "Stack underflow");
                    return 0;
                }
                state->memory[state->ptr] = state->stack[--state->stack_top];
                break;
            case CMD_PUSH_REG:
                if (state->stack_top >= MAX_STACK) {
                    set_error(state, ERR_STACK_OVERFLOW, "Stack overflow");
                    return 0;
                }
                state->stack[state->stack_top++] = state->reg;
                break;
            case CMD_POP_REG:
                if (state->stack_top == 0) {
                    set_error(state, ERR_STACK_UNDERFLOW, "Stack underflow");
                    return 0;
                }
                state->reg = state->stack[--state->stack_top];
                break;
            case CMD_SWAP_STACK:
                if (state->stack_top < 2) {
                    set_error(state, ERR_STACK_UNDERFLOW, "Stack underflow for swap");
                    return 0;
                }
                {
                    uint8_t tmp = state->stack[state->stack_top - 1];
                    state->stack[state->stack_top - 1] = state->stack[state->stack_top - 2];
                    state->stack[state->stack_top - 2] = tmp;
                }
                break;
            case CMD_PUSH_1: case CMD_PUSH_2: case CMD_PUSH_3: case CMD_PUSH_4:
            case CMD_PUSH_5: case CMD_PUSH_6: case CMD_PUSH_7: case CMD_PUSH_8:
            case CMD_PUSH_9:
                if (state->stack_top >= MAX_STACK) {
                    set_error(state, ERR_STACK_OVERFLOW, "Stack overflow");
                    return 0;
                }
                state->stack[state->stack_top++] = cmd->value;
                break;
            case CMD_POP_1: case CMD_POP_2: case CMD_POP_3: case CMD_POP_4:
            case CMD_POP_5: case CMD_POP_6: case CMD_POP_7: case CMD_POP_8:
            case CMD_POP_9:
                if (state->stack_top < (uint32_t)cmd->value) {
                    set_error(state, ERR_STACK_UNDERFLOW, "Stack underflow");
                    return 0;
                }
                state->stack_top -= cmd->value;
                break;
            case CMD_CLEAR_STACK:
                state->stack_top = 0;
                break;

            // Register Operations
            case CMD_COPY_CELL:
                state->reg = state->memory[state->ptr];
                break;
            case CMD_PASTE_REG:
                state->memory[state->ptr] = state->reg;
                break;
            case CMD_CLEAR_REG:
                state->reg = 0;
                break;
            case CMD_INC_REG:
                state->reg += 1;
                break;
            case CMD_DEC_REG:
                state->reg -= 1;
                break;
            case CMD_SET_REG_1: case CMD_SET_REG_2: case CMD_SET_REG_3:
            case CMD_SET_REG_4: case CMD_SET_REG_5: case CMD_SET_REG_6:
            case CMD_SET_REG_7: case CMD_SET_REG_8: case CMD_SET_REG_9:
                state->reg = cmd->value;
                break;
            case CMD_ADD_CELL_TO_REG:
                state->reg += state->memory[state->ptr];
                break;
            case CMD_SUB_CELL_FROM_REG:
                state->reg -= state->memory[state->ptr];
                break;

            // Utility
            case CMD_NOP:
                break;
            case CMD_SWAP_CELL:
                if (!check_memory_bounds(state, state->ptr + 1, 1)) return 0;
                {
                    uint8_t tmp = state->memory[state->ptr];
                    state->memory[state->ptr] = state->memory[state->ptr + 1];
                    state->memory[state->ptr + 1] = tmp;
                }
                break;
            case CMD_REVERSE_BITS:
                {
                    uint8_t val = state->memory[state->ptr];
                    uint8_t rev = 0;
                    for (int i = 0; i < 8; i++) {
                        rev = (rev << 1) | (val & 1);
                        val >>= 1;
                    }
                    state->memory[state->ptr] = rev;
                }
                break;
            case CMD_RANDOM:
                state->memory[state->ptr] = rand() % 256;
                break;
            case CMD_HALT:
                return 1;
            case CMD_SWAP_1: case CMD_SWAP_2: case CMD_SWAP_3: case CMD_SWAP_4:
            case CMD_SWAP_5: case CMD_SWAP_6: case CMD_SWAP_7: case CMD_SWAP_8:
            case CMD_SWAP_9:
                {
                    uint32_t idx = state->ptr + cmd->value;
                    if (!check_memory_bounds(state, idx, 1)) return 0;
                    uint8_t tmp = state->memory[state->ptr];
                    state->memory[state->ptr] = state->memory[idx];
                    state->memory[idx] = tmp;
                }
                break;
            case CMD_COPY_1: case CMD_COPY_2: case CMD_COPY_3: case CMD_COPY_4:
            case CMD_COPY_5: case CMD_COPY_6: case CMD_COPY_7: case CMD_COPY_8:
            case CMD_COPY_9:
                if (!check_memory_bounds(state, state->ptr + cmd->value, 1)) return 0;
                state->memory[state->ptr + cmd->value] = state->memory[state->ptr];
                break;
            case CMD_MOVE_1: case CMD_MOVE_2: case CMD_MOVE_3: case CMD_MOVE_4:
            case CMD_MOVE_5: case CMD_MOVE_6: case CMD_MOVE_7: case CMD_MOVE_8:
            case CMD_MOVE_9:
                if (!check_memory_bounds(state, state->ptr + cmd->value, 1)) return 0;
                state->memory[state->ptr + cmd->value] = state->memory[state->ptr];
                state->memory[state->ptr] = 0;
                break;
            case CMD_XOR_1: case CMD_XOR_2: case CMD_XOR_3: case CMD_XOR_4:
            case CMD_XOR_5: case CMD_XOR_6: case CMD_XOR_7: case CMD_XOR_8:
            case CMD_XOR_9:
                if (!check_memory_bounds(state, state->ptr + cmd->value, 1)) return 0;
                state->memory[state->ptr] ^= state->memory[state->ptr + cmd->value];
                break;

            // String commands
            case CMD_STR_OUT:
                {
                    // Print string from current cell until null (0)
                    uint32_t start = state->ptr;
                    while (state->memory[start] != 0) {
                        putchar(state->memory[start]);
                        start++;
                        if (start >= state->mem_size) break;
                    }
                    fflush(stdout);
                }
                break;
            case CMD_STR_LEN:
                {
                    // Store length of null-terminated string into register
                    uint32_t start = state->ptr;
                    uint8_t len = 0;
                    while (state->memory[start] != 0 && len < 255) {
                        len++;
                        start++;
                        if (start >= state->mem_size) break;
                    }
                    state->reg = len;
                }
                break;
            case CMD_STR_IN:
                {
                    // Read a line from input into memory starting at current cell
                    uint32_t start = state->ptr;
                    int c;
                    while ((c = getchar()) != EOF && c != '\n') {
                        if (start >= state->mem_size) break;
                        state->memory[start++] = (uint8_t)c;
                    }
                    if (start < state->mem_size) {
                        state->memory[start] = 0; // Null terminate
                    }
                }
                break;

            // Function commands
            case CMD_FUNC_DEF:
                {
                    // Skip past the function body to 'end'
                    int func_idx = cmd->value;
                    if (func_idx >= 0 && (uint32_t)func_idx < state->func_count &&
                        state->functions[func_idx].end_pc > 0) {
                        state->pc = state->functions[func_idx].end_pc; // Jump to 'end'
                    } else {
                        set_error(state, ERR_FUNC_NOT_FOUND, "Function has no end");
                        return 0;
                    }
                }
                break;
            case CMD_FUNC_END:
                // If we reach this during normal execution (not from function call),
                // just continue. If from function, ret should have been called.
                break;
            case CMD_FUNC_CALL:
                {
                    int func_idx = cmd->value;
                    if (func_idx < 0 || (uint32_t)func_idx >= state->func_count) {
                        set_error(state, ERR_FUNC_NOT_FOUND, "Invalid function index");
                        return 0;
                    }
                    if (state->functions[func_idx].start_pc == 0) {
                        set_error(state, ERR_FUNC_NOT_FOUND, "Function not defined");
                        return 0;
                    }
                    if (state->call_top >= MAX_CALL_STACK) {
                        set_error(state, ERR_CALL_STACK_OVERFLOW, "Call stack overflow");
                        return 0;
                    }
                    // Push return address (next instruction after call)
                    state->call_stack[state->call_top++] = state->pc + 1;
                    // Jump to function start (after the def command)
                    state->pc = state->functions[func_idx].start_pc;
                    continue; // Don't increment pc
                }
            case CMD_FUNC_RET:
                {
                    if (state->call_top == 0) {
                        // Not in a function, just continue (or could be error)
                        break;
                    }
                    // Pop return address and jump
                    state->pc = state->call_stack[--state->call_top];
                    continue; // Don't increment pc
                }

            case CMD_UNKNOWN:
                set_error(state, ERR_INVALID_COMMAND, "Unknown command");
                break;
        }

        state->pc++;
    }
    return 1;
}