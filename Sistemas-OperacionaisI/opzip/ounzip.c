// ounzip.c — descompressor de referencia
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "uso: %s arquivo.oz [arquivo2.oz ...]\n", argv[0]);
        exit(1);
    }
    for (int i = 1; i < argc; i++) {
        FILE *f = fopen(argv[i], "rb");
        if (f == NULL) {
            fprintf(stderr, "erro ao abrir %s\n", argv[i]);
            exit(1);
        }
        int count;
        char c;
        while (fread(&count, sizeof(int), 1, f) == 1) {
            if (fread(&c, sizeof(char), 1, f) != 1) {
                fprintf(stderr, "arquivo comprimido corrompido: %s\n", argv[i]);
                exit(1);
            }
            for (int j = 0; j < count; j++) {
                putchar(c);
            }
        }
        fclose(f);
    }
    return 0;
}
