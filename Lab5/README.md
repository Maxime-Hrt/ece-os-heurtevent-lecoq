### 1. Fonction `myAlloc`

```c
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
```
Pour la fonction `initMem`, nous avons codé un mécanisme d'initialisation qui permet de préparer l'environnement pour notre simulation de gestion de mémoire. Cette fonction crée et configure une structure `mem_t`, qui sert de représentation de notre espace mémoire. Dans cette structure, nous avons inclus un tableau représentant la mémoire physique et un espace initial libre, ou 'trou', qui occupe initialement toute la mémoire. En appelant `initMem`, comme dans `mem_t *mem = initMem();`, nous initialisons cet espace mémoire en fixant l'adresse de départ à zéro et en attribuant la taille totale définie par `SIZE`. Ce processus est vital pour établir une base sur laquelle les fonctions subséquentes d'allocation et de libération de mémoire vont opérer, en leur fournissant un cadre clair sur l'état et la disposition de l'espace mémoire disponible.

### 2. Fonction `myAlloc`
```c
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
```
Pour la fonction `myAlloc`, nous avons développé une méthode d'allocation de mémoire qui recherche un espace libre, ou 'trou', suffisamment grand pour satisfaire une demande de taille spécifique. Cette fonction parcourt la liste des trous disponibles dans la structure `mem_t`, débutant par le 'trou' racine, et sélectionne le premier espace qui correspond aux besoins en taille. En appelant `address_t adr = myAlloc(mem, size);`, la fonction alloue de l'espace mémoire et ajuste la taille du trou correspondant, diminuant ainsi sa taille disponible. Cette allocation est essentielle pour simuler la manière dont la mémoire est allouée dans un système réel, en gérant efficacement l'espace disponible.

### 3. Fonction `myFree`
```c
void myFree(mem_t *mp, address_t p, int sz) {
    printf("Mémoire libérée: Adresse = %d, Taille = %d\n", p, sz);
}
```
La fonction `myFree` est conçue pour simuler le processus de libération de mémoire. Lorsque nous appelons `myFree(mem, address, size);`, la fonction prend en charge le segment de mémoire spécifié par `address` et `size`, le marquant comme libre. Cette fonction est importante pour simuler la gestion de l'espace mémoire, permettant la réutilisation de segments de mémoire qui ne sont plus nécessaires. La logique de fusion des trous adjacents, bien qu'essentielle dans un gestionnaire de mémoire réel, n'est pas implémentée dans cette version simplifiée.

### 4. Fonction `myWrite`
```c
void myWrite(mem_t *mp, address_t p, byte_t val) {
    if (p >= 0 && p < SIZE) {
        mp->mem[p] = val;
        printf("Écriture à l'adresse %d avec la valeur %d\n", p, val);
    }
}
```
Dans `myWrite`, nous avons codé une fonction qui permet d'écrire une valeur spécifique dans un emplacement mémoire donné. Lors de l'appel de `myWrite(mem, address, value);`, la fonction stocke la valeur `value` à l'adresse `address` dans le tableau de mémoire. Cette fonction est un élément clé pour simuler l'écriture de données en mémoire, un processus courant dans les opérations de gestion de mémoire.

### 5. Fonction `myRead`
```c
byte_t myRead(mem_t *mp, address_t p) {
    if (p >= 0 && p < SIZE) {
        byte_t val = mp->mem[p];
        printf("Lecture à l'adresse %d, Valeur obtenue: %d\n", p, val);
        return val;
    }
    printf("Erreur de lecture à l'adresse %d\n", p);
    return -1;
}
```
La fonction `myRead` a été conçue pour lire et retourner la valeur stockée à une adresse mémoire spécifique. En appelant `byte_t value = myRead(mem, address);`, elle accède à l'adresse `address` dans la mémoire et renvoie la valeur qui y est stockée. Cette fonction est cruciale pour simuler l'accès aux données stockées en mémoire, reflétant une opération fondamentale dans la gestion de la mémoire.****