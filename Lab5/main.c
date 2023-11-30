#include <stdio.h>
#include <stdlib.h>

#define SIZE 65536
typedef short byte_t;
typedef int address_t;

typedef struct hole {
    address_t adr;
    int sz;
    struct hole *next;
    struct hole *prev;
} hole_t;

typedef struct {
    byte_t mem[SIZE];
    hole_t root;
} mem_t;

mem_t *initMem() {
    mem_t *mp = (mem_t *)malloc(sizeof(mem_t));
    if (mp == NULL) {
        printf("Échec de l'initialisation de la mémoire.\n");
        return NULL;
    }

    mp->root.adr = 0;
    mp->root.sz = SIZE;
    mp->root.next = NULL;
    mp->root.prev = NULL;

    printf("Mémoire initialisée. Taille du trou initial: %d\n", mp->root.sz);
    return mp;
}

address_t myAlloc(mem_t *mp, int sz) {
    hole_t *current = &mp->root;
    while (current != NULL) {
        if (current->sz >= sz) {
            address_t allocAddr = current->adr;
            current->adr += sz;
            current->sz -= sz;
            printf("Allocation réussie: Adresse = %d, Taille = %d\n", allocAddr, sz);
            return allocAddr;
        }
        current = current->next;
    }
    printf("Échec de l'allocation de %d octets\n", sz);
    return -1;
}

void myFree(mem_t *mp, address_t p, int sz) {
    printf("Mémoire libérée: Adresse = %d, Taille = %d\n", p, sz);
}

void myWrite(mem_t *mp, address_t p, byte_t val) {
    if (p >= 0 && p < SIZE) {
        mp->mem[p] = val;
        printf("Écriture à l'adresse %d avec la valeur %d\n", p, val);
    }
}

byte_t myRead(mem_t *mp, address_t p) {
    if (p >= 0 && p < SIZE) {
        byte_t val = mp->mem[p];
        printf("Lecture à l'adresse %d, Valeur obtenue: %d\n", p, val);
        return val;
    }
    printf("Erreur de lecture à l'adresse %d\n", p);
    return -1;
}

int main() {
    mem_t *mem = initMem();
    if (mem == NULL) {
        return 1;
    }

    address_t adr1 = myAlloc(mem, 5);
    address_t adr2 = myAlloc(mem, 10);
    address_t adr3 = myAlloc(mem, 100);

    myFree(mem, adr2, 10);
    myFree(mem, adr1, 5);

    myWrite(mem, adr3, 543); // write on the 1st byte
    myWrite(mem, adr3+9, 34); // write on the 10th byte
    byte_t val1 = myRead(mem, adr3);
    byte_t val2 = myRead(mem, adr3+9);


    free(mem);
    return 0;
}
