CC = gcc
CXX = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build

all: clean printersSystem usersSimulator

$(BUILD_DIR)/printersSystem.o: printersSystem.c
	$(CC) $(CFLAGS) -g -c printersSystem.c -o $(BUILD_DIR)/printersSystem.o

printersSystem: $(BUILD_DIR)/printersSystem.o
	$(CC) $(CFLAGS) -g -o printersSystem $(BUILD_DIR)/printersSystem.o

$(BUILD_DIR)/usersSimulator.o: usersSimulator.c
	$(CC) $(CFLAGS) -g -c usersSimulator.c -o $(BUILD_DIR)/usersSimulator.o

usersSimulator: $(BUILD_DIR)/usersSimulator.o
	$(CC) $(CFLAGS) -g -o usersSimulator $(BUILD_DIR)/usersSimulator.o

clean:
	rm -f build/*
	rm -f printersSystem usersSimulator
