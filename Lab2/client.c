#include <stdio.h>
#include <dlfcn.h>

#ifdef LOAD_DYNAMIC
#define COLLATZ_LIB "/home/f0rest/CLionProjects/SysOpy/Lab2/build/libcollatz_dynamic.so"
#else
#define COLLATZ_LIB "/home/f0rest/CLionProjects/SysOpy/Lab2/build/libcollatz_shared.so"
#endif

int main() {
    void *handle = dlopen(COLLATZ_LIB, RTLD_LAZY);  // wczytanie biblioteki

    if (!handle) {  // obsluga bledow z wczytywaniem biblioteki
        fprintf(stderr, "Error: %s\n", dlerror());
        return 1;
    }

    int (*collatz_conjecture)(int) = dlsym(handle, "collatz_conjecture");  // wczytanie funkcji collatz_conjecture z biblioteki
    int (*test_collatz_convergence)(int, int) = dlsym(handle, "test_collatz_convergence");  // wczytanie funkcji test_collatz_convergence z biblioteki

    if (!collatz_conjecture || !test_collatz_convergence) {  // obsluga bledow z wczytywaniem funkcji z biblioteki
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return 1;
    }

    int numbers[] = {150,20,30};  // testowy zestaw liczb do sprawdzenia dzialania funkcji

    for (int i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++) {  // test
        int result = test_collatz_convergence(numbers[i],1000000);
        if (result == -1) {
            printf("Collatz conjecture not convered within maximum interation for %d\n",numbers[i]);
        } else {
            printf("Collatz conjecture convered for %d after %d interations\n",numbers[i], result);
        }
    }

    dlclose(handle);
    return 0;
}