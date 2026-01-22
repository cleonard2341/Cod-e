#include "code.h"
#include <unistd.h>
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#endif

void print_usage() {
    fprintf(stderr, "Usage: code [-f file | -i] [-d]\n");
    fprintf(stderr, "  -f file: Run Cod-e program from file\n");
    fprintf(stderr, "  -i: Interactive mode\n");
    fprintf(stderr, "  -d: Enable debug/trace mode\n");
}

int main(int argc, char *argv[]) {
#ifdef _WIN32
    // Set console output to UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif
    CodeState *state = init_state();
    if (!state) {
        fprintf(stderr, "Error: Failed to initialize interpreter\n");
        return 1;
    }

    int opt;
    int interactive = 0;
    int debug_mode = 0;
    char *filename = NULL;

    // Parse arguments
    while ((opt = getopt(argc, argv, "f:id")) != -1) {
        switch (opt) {
            case 'f':
                filename = optarg;
                break;
            case 'i':
                interactive = 1;
                break;
            case 'd':
                debug_mode = 1;
                break;
            default:
                print_usage();
                free_state(state);
                return 1;
        }
    }

    // Set debug mode
    state->trace_mode = debug_mode;

    if (filename && interactive) {
        fprintf(stderr, "Error: Cannot use -f and -i together\n");
        print_usage();
        free_state(state);
        return 1;
    }

    if (!filename && !interactive) {
        print_usage();
        free_state(state);
        return 1;
    }

    // Read and run program
    char *program = NULL;
    size_t prog_size = 0;

    if (filename) {
        if (state->trace_mode) fprintf(stderr, "Reading file: %s\n", filename);
        // Read from file
        FILE *fp = fopen(filename, "r");
        if (!fp) {
            fprintf(stderr, "Error: Cannot open file %s\n", filename);
            free_state(state);
            return 1;
        }
        fseek(fp, 0, SEEK_END);
        prog_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        program = (char*)malloc(prog_size + 1);
        if (!program) {
            fclose(fp);
            free_state(state);
            return 1;
        }
        size_t bytes_read = fread(program, 1, prog_size, fp);
        program[bytes_read] = '\0';
        fclose(fp);
    } else if (interactive) {
        if (state->trace_mode) fprintf(stderr, "Entering interactive mode\n");
        // Interactive mode
        printf("Cod-e Interactive Mode (Ctrl+Z or empty line to run)\n");
        #define BUFFER_SIZE 1024
        char line[BUFFER_SIZE];
        while (fgets(line, BUFFER_SIZE, stdin) != NULL) {
            // Check for empty line or Ctrl+C (EOF on Windows)
            size_t len = strlen(line);
            if (len <= 1 || line[0] == '\x1a') break; // Empty line or Ctrl+C
            if (prog_size + len > prog_size) {
                char *new_prog = (char*)realloc(program, prog_size + len + 1);
                if (!new_prog) {
                    free(program);
                    free_state(state);
                    return 1;
                }
                program = new_prog;
            }
            memcpy(program + prog_size, line, len);
            prog_size += len;
            program[prog_size] = '\0';
        }
    }

    if (state->trace_mode) fprintf(stderr, "Parsing program\n");
    // Parse and execute
    if (!parse_program(program, state)) {
        fprintf(stderr, "Error: Failed to parse program\n");
        free(program);
        free_state(state);
        return 1;
    }

    if (state->trace_mode) fprintf(stderr, "Executing program (%u commands)\n", state->prog_len);
    if (!execute_program(state)) {
        fprintf(stderr, "Error: Failed to execute program: %s\n", state->error_message);
        free(program);
        free_state(state);
        return 1;
    }

    if (state->trace_mode) fprintf(stderr, "Execution complete\n");
    fflush(stdout);
    free(program);
    free_state(state);
    return 0;
}