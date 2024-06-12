CC = gcc
CXX = gcc
CFLAGS = -Wall -Wextra
BUILD_DIR = build

PORT = 4997


all: clean create_dir client server

create_dir:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/client.o: client.c
	$(CC) $(CFLAGS) -g -c client.c -o $(BUILD_DIR)/client.o

client: $(BUILD_DIR)/client.o
	$(CC) $(CFLAGS) -g -o client $(BUILD_DIR)/client.o

$(BUILD_DIR)/server.o: server.c
	$(CC) $(CFLAGS) -g -c server.c -o $(BUILD_DIR)/server.o

server: $(BUILD_DIR)/server.o
	$(CC) $(CFLAGS) -g -o server $(BUILD_DIR)/server.o

run_server:
	./server 127.0.0.1 $(PORT)

run_client_1:
	./client client1 127.0.0.1 $(PORT)

run_client_2:
	./client client2 127.0.0.1 $(PORT)

run_client_3:
	./client client3 127.0.0.1 $(PORT)

clean:
	rm -rf build
	rm -f client server