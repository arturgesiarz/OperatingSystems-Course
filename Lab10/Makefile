CC = gcc
CXX = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build

all: clean create_dir merry_christmas

create_dir:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/merry_christmas.o: merry_christmas.c
	$(CC) $(CFLAGS) -g -c merry_christmas.c -o $(BUILD_DIR)/merry_christmas.o

merry_christmas: $(BUILD_DIR)/merry_christmas.o
	$(CC) $(CFLAGS) -g -o merry_christmas $(BUILD_DIR)/merry_christmas.o

clean:
	rm -rf build
	rm -f merry_christmas