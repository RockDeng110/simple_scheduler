# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pedantic -std=c11 -g
LDFLAGS = -lrt

# Source files
SRC = scheduler.c

# Object files
OBJ = $(SRC:.c=.o)

# Executable name
EXEC = scheduler

# Default target
all: $(EXEC)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files into executable
$(EXEC): $(OBJ)
	$(CC) $(OBJ) -o $(EXEC) $(LDFLAGS)

# Clean up
clean:
	rm -f $(OBJ) $(EXEC)

