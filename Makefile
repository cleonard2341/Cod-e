CC = gcc
CFLAGS = -Wall -Wextra -O2
TARGET = code
SRCS = main.c code.c
OBJS = $(SRCS:.c=.o)

.PHONY: all clean test install

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)

test: $(TARGET)
	@echo "Running all tests..."
	@passed=0; failed=0; \
	for f in tests/*.code; do \
		output=$$(./$(TARGET) -f "$$f" 2>&1); \
		if echo "$$output" | grep -q "PASS\|DONE"; then \
			echo "[PASS] $$f"; \
			passed=$$((passed + 1)); \
		else \
			echo "[FAIL] $$f"; \
			echo "$$output" | tail -3; \
			failed=$$((failed + 1)); \
		fi; \
	done; \
	echo ""; \
	echo "Results: $$passed passed, $$failed failed"

install: $(TARGET)
	install -m 755 $(TARGET) /usr/local/bin/

# Debug build
debug: CFLAGS += -g -DDEBUG
debug: $(TARGET)
