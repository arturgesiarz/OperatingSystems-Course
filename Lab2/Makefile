CC = gcc
CFLAGS = -Wall -Wextra -std=c11
BUILD_DIR = build

all: $(BUILD_DIR)/libstatic $(BUILD_DIR)/libshared $(BUILD_DIR)/libdynamic \
client_static client_shared client_dynamic

$(BUILD_DIR)/collatz.o: collatz.c collatz.h
	$(CC) $(CFLAGS) -g -c collatz.c -o $(BUILD_DIR)/collatz.o

$(BUILD_DIR)/client.o: client.c
	$(CC) $(CFLAGS) -g -c client.c -o $(BUILD_DIR)/client.o

$(BUILD_DIR)/libstatic: $(BUILD_DIR)/collatz.o
	ar rcs $(BUILD_DIR)/libcollatz_static.a $(BUILD_DIR)/collatz.o

$(BUILD_DIR)/libshared: $(BUILD_DIR)/collatz.o
	$(CC) -shared -o $(BUILD_DIR)/libcollatz_shared.so $(BUILD_DIR)/collatz.o

$(BUILD_DIR)/libdynamic: $(BUILD_DIR)/collatz.o
	$(CC) -shared -o $(BUILD_DIR)/libcollatz_dynamic.so -DLOAD_DYNAMIC collatz.c

client_static: $(BUILD_DIR)/libstatic client_static.c
	$(CC) -o client_static client.c -L$(BUILD_DIR) -lcollatz_static

client_shared: $(BUILD_DIR)/libshared client.c
	$(CC) -o client_shared client.c -L$(BUILD_DIR) -lcollatz_shared

client_dynamic: $(BUILD_DIR)/libdynamic client.c
	$(CC) -o client_dynamic client.c -ldl

clean:
	rm -rf build/*
	rm client_static client_shared client_dynamic

.PHONE: all clean