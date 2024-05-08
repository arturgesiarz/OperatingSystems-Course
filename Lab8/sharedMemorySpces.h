#ifndef SHARED_MEMORY_SPEC_H
#define SHARED_MEMORY_SPEC_H

#define SHARED_MEMORY_DESCRIPTOR_NAME "printer_system_shared_memory"
#include <semaphore.h>

#define MAX_PRINTERS 256
#define MAX_PRINTER_BUFFER_SIZE 256
#define MAX_SEMAPHORE_NAME 40

typedef enum {
    WAITING = 0,
    PRINTING = 1
} printerStateT;

typedef struct {
    sem_t printerSemaphore;
    char printerBuffer[MAX_PRINTER_BUFFER_SIZE];
    size_t printerBufferSize;
    printerStateT printerState;
} printerT;

typedef struct {
    printerT printers[MAX_PRINTERS];
    int numberOfPrinters;
} memoryMapT;

#endif //SHARED_MEMORY_SPEC_H