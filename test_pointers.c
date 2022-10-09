// pointers

#include <stdio.h>

void nums (int *lmfao);

int main (void) {
    int playerX = 3;
    nums(&playerX);
    
    printf("%d\n", playerX);
    
    return 0;
}

void nums (int *lmfao) {
    *lmfao = *lmfao + 1;
}
    
    
