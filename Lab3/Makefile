CC = gcc
CXX = gcc
CFLAGS = -Wall -Wextra -std=c11
BUILD_DIR = build

all: $(BUILD_DIR)/reverse $(BUILD_DIR)/view

$(BUILD_DIR)/view.o: view.c
	$(CC) $(CFLAGS) -g -c view.c -o $(BUILD_DIR)/view.o

$(BUILD_DIR)/view: $(BUILD_DIR)/view.o
	$(CC) $(CFLAGS) -g -o view $(BUILD_DIR)/view.o

$(BUILD_DIR)/reverse.o: reverse.c
	$(CC) $(CFLAGS) -g -c reverse.c -o $(BUILD_DIR)/reverse.o

$(BUILD_DIR)/reverse: $(BUILD_DIR)/reverse.o
	$(CC) $(CFLAGS) -g -o reverse $(BUILD_DIR)/reverse.o

clean:
	rm -f build/*
	rm -f *.txt
	rm -f reverse $(BUILD_DIR)/reverse.o view $(BUILD_DIR)/view.o

