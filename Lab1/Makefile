CC = gcc
CXX = gcc
CFLAGS = -Wall -Wextra -std=c11
BUILD_DIR = build

all: countdown

$(BUILD_DIR)/countdown.o: countdown.c
	$(CC) $(CFLAGS) -g -c countdown.c -o $(BUILD_DIR)/countdown.o

countdown: $(BUILD_DIR)/countdown.o
	$(CC) $(CFLAGS) -g -o countdown $(BUILD_DIR)/countdown.o

clean:
	rm -f countdown $(BUILD_DIR)/countdown.o