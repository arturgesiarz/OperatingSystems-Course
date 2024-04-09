CC = gcc
CXX = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build

all: ex1 ex2

$(BUILD_DIR)/ex1.o: ex1.c
	$(CC) $(CFLAGS) -g -c ex1.c -o $(BUILD_DIR)/ex1.o

$(BUILD_DIR)/ex2.o: ex2.c
	$(CC) $(CFLAGS) -g -c ex2.c -o $(BUILD_DIR)/ex2.o

ex1: $(BUILD_DIR)/ex1.o
	$(CC) $(CFLAGS) -g -o ex1 $(BUILD_DIR)/ex1.o

ex2: $(BUILD_DIR)/ex2.o
	$(CC) $(CFLAGS) -g -o ex2 $(BUILD_DIR)/ex2.o

clean:
	rm -f build/*
	rm -f ex1 ex2
