#include <stdio.h>
#include "collatz.h"

int main() {

    int numbers[] = {150,20,30};  // testowy zestaw liczb do sprawdzenia dzialania funkcji

    for (int i = 0; i < sizeof(numbers) / sizeof(numbers[0]); i++) {  // test
        int result = test_collatz_convergence(numbers[i],1000000);
        if (result == -1) {
            printf("Collatz conjecture not convered within maximum interation for %d\n",numbers[i]);
        } else {
            printf("Collatz conjecture convered for %d after %d interations\n",numbers[i], result);
        }
    }

    return 0;
}