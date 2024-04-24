CC = gcc
CXX = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build

all: clean client server

$(BUILD_DIR)/client.o: client.c
	$(CC) $(CFLAGS) -g -c client.c -o $(BUILD_DIR)/client.o

$(BUILD_DIR)/server.o: server.c
	$(CC) $(CFLAGS) -g -c server.c -o $(BUILD_DIR)/server.o

client: $(BUILD_DIR)/client.o
	$(CC) $(CFLAGS) -g -o client $(BUILD_DIR)/client.o

server: $(BUILD_DIR)/server.o
	$(CC) $(CFLAGS) -g -o server $(BUILD_DIR)/server.o

clean:
	rm -f build/*
	rm -f client server
