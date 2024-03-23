CC = gcc
CXX = gcc
CFLAGS = -Wall -Wextra -std=c11
BUILD_DIR = build

all: process process_2

$(BUILD_DIR)/process.o: process.c
	$(CC) $(CFLAGS) -g -c process.c -o $(BUILD_DIR)/process.o

$(BUILD_DIR)/process_2.o: process_2.c
	$(CC) $(CFLAGS) -g -c process_2.c -o $(BUILD_DIR)/process_2.o

process: $(BUILD_DIR)/process.o
	$(CC) $(CFLAGS) -g -o process $(BUILD_DIR)/process.o

process_2: $(BUILD_DIR)/process_2.o
	$(CC) $(CFLAGS) -g -o process_2 $(BUILD_DIR)/process_2.o

clean:
	rm -f build/*
	rm -f process process_2
