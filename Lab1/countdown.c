#include <stdio.h>

void calculateFrom10To0(){
    for(int i = 10; i >= 0; i = i - 1){
        printf("%d \n", i);
    }
}

int main() {
    calculateFrom10To0();
}
