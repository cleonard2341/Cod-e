# Cod-e Programming Language

A powerful esoteric programming language inspired by Brainfuck, featuring ~300 commands for memory manipulation, arithmetic, bitwise operations, stack operations, functions, and string handling.

## Building

### Prerequisites
- GCC or any C compiler
- Make (optional)

### Compile
```bash
gcc -o code main.c code.c -Wall -Wextra -O2
```

Or using Make:
```bash
make
```

## Usage

```bash
./code -f program.code    # Run a program file
./code -i                 # Interactive mode
./code -d -f program.code # Run with debug tracing
```

## Language Overview

Cod-e uses a memory tape of 30,000 cells (expandable), each holding a value 0-255. It has a pointer, a register, and a stack.

### Command Categories

#### Pointer Movement
| Command | Description |
|---------|-------------|
| `>` | Move pointer right by 1 |
| `<` | Move pointer left by 1 |
| `>0` | Go to cell 0 (absolute) |
| `>1` - `>9` | Move right N cells (relative) |
| `<1` - `<9` | Move left N cells (relative) |
| `>a` | Move right by register value |
| `<a` | Move left by register value |
| `>b` | Move right by current cell value |
| `<b` | Move left by current cell value |

#### Memory Manipulation
| Command | Description |
|---------|-------------|
| `++` | Increment current cell |
| `--` | Decrement current cell |
| `+N` | Add N (1-9 or multi-digit) |
| `-N` | Subtract N |
| `=N` | Set cell to N (0-255) |
| `=0` | Clear cell |
| `=a` | Set cell to register value |

#### Input/Output
| Command | Description |
|---------|-------------|
| `.` | Output cell as ASCII character |
| `,` | Input character to cell |
| `:n` | Output cell as number |
| `:r` | Output register as character |
| `;n` | Input number to cell |

#### Control Flow
| Command | Description |
|---------|-------------|
| `[` | Start loop (while cell != 0) |
| `]` | End loop |
| `if0` | Skip next command if cell is 0 |
| `ifn` | Skip next command if cell is non-zero |
| `j1` - `j9` | Jump forward N commands if non-zero |
| `k1` - `k9` | Jump backward N commands if non-zero |
| `i1` - `i9` | Skip N commands if zero |

#### Arithmetic
| Command | Description |
|---------|-------------|
| `+a` | Add register to cell |
| `-a` | Subtract register from cell |
| `m*` | Multiply cell by register |
| `d/` | Divide cell by register |
| `m1` - `m9` | Multiply cell by N |
| `d1` - `d9` | Divide cell by N |

#### Bitwise Operations
| Command | Description |
|---------|-------------|
| `&r` | AND cell with register |
| `\|r` | OR cell with register |
| `^r` | XOR cell with register |
| `<<1` - `<<8` | Left shift by N bits |
| `>>1` - `>>8` | Right shift by N bits |
| `~c` | NOT (complement) cell |
| `&1` - `&9` | AND cell with N |
| `x1` - `x9` | XOR cell with cell at offset N |

#### Stack Operations
| Command | Description |
|---------|-------------|
| `ps` | Push cell to stack |
| `pp` | Pop stack to cell |
| `pr` | Push register to stack |
| `sr` | Pop stack to register |
| `sp` | Swap top two stack items |
| `p1` - `p9` | Push literal N to stack |
| `sc` | Clear stack |

#### Register Operations
| Command | Description |
|---------|-------------|
| `cp` | Copy cell to register |
| `pt` | Paste register to cell |
| `cr` | Clear register |
| `r+` | Increment register |
| `r-` | Decrement register |
| `r1` - `r9` | Set register to N |
| `ra` | Add cell to register |
| `rs` | Subtract cell from register |

#### Utility Commands
| Command | Description |
|---------|-------------|
| `sw` | Swap cell with next cell |
| `sw1` - `sw9` | Swap cell with cell at offset N |
| `c1` - `c9` | Copy cell to cell at offset N |
| `mv1` - `mv9` | Move cell to offset N (clears source) |
| `rv` | Reverse bits in cell |
| `rnd` | Set cell to random value 0-255 |
| `nop` | No operation |
| `hlt` | Halt execution |

#### Functions
| Command | Description |
|---------|-------------|
| `def name` | Define function with given name |
| `end` | End function definition |
| `call name` | Call function by name |
| `ret` | Return from function |

#### String Operations
| Command | Description |
|---------|-------------|
| `sout` | Print string from current cell until null |
| `slen` | Store string length in register |
| `sin` | Read line from input to memory |

## Examples

### Hello World
```
# Print "Hello"
=72 . =101 . =108 . =108 . =111 .
=10 .  # Newline
```

### Loop Example
```
# Print 5 asterisks
>0 =5     # Counter
>1 =42    # '*' character
[
  >1 .    # Print '*'
  >0 --   # Decrement counter
]
```

### Function Example
```
def greet
  =72 . =105 . =33 .  # "Hi!"
  ret
end

call greet
```

### Recursive Countdown
```
def countdown
  >0 :n      # Print number
  >1 =32 .   # Print space
  >0 --      # Decrement
  i2         # If zero, skip call
  call countdown
  ret
  ret
end

>0 =5
call countdown  # Prints: 5 4 3 2 1
```

## Running Tests

```bash
for f in tests/*.code; do ./code -f "$f"; done
```

## Safety Features

- Memory bounds checking
- Instruction limit (10 million) to prevent infinite loops
- 30-second execution timeout
- Stack overflow protection
- Division by zero handling

## License

MIT License
