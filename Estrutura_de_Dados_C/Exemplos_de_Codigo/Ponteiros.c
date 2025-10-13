/**
 * UFMT - Ciencia da computacao
 * Prof. Ivairton
 * 
 * Exericio com ponteiros
 */

#include<stdio.h>
#include<stdlib.h>

void dobra(int *pt_a);

int main() {
    int x;
    int *pt_x, *pt_y;

    x = 2;
    pt_x = NULL;
    pt_x = &x;
    printf("Endereco de &x: %d\n", &x);
    printf("Valor armazenado em pt_x: %d\n", pt_x);
    printf("Endereco de &pt_x: %d\n", &pt_x);
    printf("Valor em memoria acessado por *pt_x: %d\n", *pt_x);

    printf("Valor de x = %d\n", x);
    dobra (&x);
    printf("Valor de x apos dobra = %d\n", x);
    
    pt_y = (int*) malloc( sizeof( int ) );
    *pt_y = 10;

    printf("valor armazenado em *pt_y: %d\n", *pt_y);
    free(pt_y);
    
    return 0;
}

void dobra( int *pt_a ) {
    *pt_a = *pt_a * 2;
}
