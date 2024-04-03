CC = gcc
CXX = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build

all: ex1 ex2_catcher ex2_sender

$(BUILD_DIR)/ex1.o: ex1.c
	$(CC) $(CFLAGS) -g -c ex1.c -o $(BUILD_DIR)/ex1.o

$(BUILD_DIR)/ex2_sender.o: ex2_sender.c
	$(CC) $(CFLAGS) -g -c ex2_sender.c -o $(BUILD_DIR)/ex2_sender.o

$(BUILD_DIR)/ex2_catcher.o: ex2_catcher.c
	$(CC) $(CFLAGS) -g -c ex2_catcher.c -o $(BUILD_DIR)/ex2_catcher.o

ex1: $(BUILD_DIR)/ex1.o
	$(CC) $(CFLAGS) -g -o ex1 $(BUILD_DIR)/ex1.o

ex2_catcher: $(BUILD_DIR)/ex2_catcher.o
	$(CC) $(CFLAGS) -g -o ex2_catcher $(BUILD_DIR)/ex2_catcher.o

ex2_sender: $(BUILD_DIR)/ex2_sender.o
	$(CC) $(CFLAGS) -g -o ex2_sender $(BUILD_DIR)/ex2_sender.o

clean:
	rm -f build/*
	rm -f ex1 ex2_catcher ex2_sender
