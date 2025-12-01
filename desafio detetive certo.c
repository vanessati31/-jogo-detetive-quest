#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 101
#define MAX_NOME 64
#define MAX_DESC 128

// -------------------- ESTRUTURA: Mapa da Mansao (Arvore Binaria) --------------------

typedef struct Room {
    char nome[MAX_NOME];
    struct Room* esquerda;
    struct Room* direita;
} Room;

Room* criarRoom(const char* nome) {
    Room* r = (Room*) malloc(sizeof(Room));
    if (!r) {
        printf("Erro ao alocar memoria para room.\n");
        exit(1);
    }
    strncpy(r->nome, nome, MAX_NOME-1);
    r->nome[MAX_NOME-1] = '\0';
    r->esquerda = r->direita = NULL;
    return r;
}

// Insercao manual orientada
void anexarEsquerda(Room* pai, Room* filho) {
    if (!pai) return;
    pai->esquerda = filho;
}
void anexarDireita(Room* pai, Room* filho) {
    if (!pai) return;
    pai->direita = filho;
}

void mostrarMapaRec(Room* raiz, int nivel) {
    if (!raiz) return;
    mostrarMapaRec(raiz->direita, nivel + 1);
    for (int i = 0; i < nivel; i++) printf("    ");
    printf("|- %s\n", raiz->nome);
    mostrarMapaRec(raiz->esquerda, nivel + 1);
}

Room* buscarRoom(Room* raiz, const char* nome) {
    if (!raiz) return NULL;
    if (strcmp(raiz->nome, nome) == 0) return raiz;
    Room* r = buscarRoom(raiz->esquerda, nome);
    if (r) return r;
    return buscarRoom(raiz->direita, nome);
}

void liberarMapa(Room* raiz) {
    if (!raiz) return;
    liberarMapa(raiz->esquerda);
    liberarMapa(raiz->direita);
    free(raiz);
}

// -------------------- ESTRUTURA: Pistas (Arvore de Busca) --------------------

typedef struct Pista {
    char descricao[MAX_DESC];
    char categoria[40];
    struct Pista* esquerda;
    struct Pista* direita;
} Pista;

Pista* criarPista(const char* descricao, const char* categoria) {
    Pista* p = (Pista*) malloc(sizeof(Pista));
    if (!p) {
        printf("Erro ao alocar memoria para pista.\n");
        exit(1);
    }
    strncpy(p->descricao, descricao, MAX_DESC-1);
    p->descricao[MAX_DESC-1] = '\0';
    strncpy(p->categoria, categoria, 39);
    p->categoria[39] = '\0';
    p->esquerda = p->direita = NULL;
    return p;
}

Pista* inserirPista(Pista* raiz, const char* descricao, const char* categoria) {
    if (!raiz) return criarPista(descricao, categoria);
    int cmp = strcmp(descricao, raiz->descricao);
    if (cmp < 0) raiz->esquerda = inserirPista(raiz->esquerda, descricao, categoria);
    else if (cmp > 0) raiz->direita = inserirPista(raiz->direita, descricao, categoria);
    else {
        strncpy(raiz->categoria, categoria, 39);
        raiz->categoria[39] = '\0';
    }
    return raiz;
}

Pista* buscarPista(Pista* raiz, const char* descricao) {
    if (!raiz) return NULL;
    int cmp = strcmp(descricao, raiz->descricao);
    if (cmp == 0) return raiz;
    if (cmp < 0) return buscarPista(raiz->esquerda, descricao);
    return buscarPista(raiz->direita, descricao);
}

void mostrarPistasInOrder(Pista* raiz) {
    if (!raiz) return;
    mostrarPistasInOrder(raiz->esquerda);
    printf("Descricao: %s | Categoria: %s\n", raiz->descricao, raiz->categoria);
    mostrarPistasInOrder(raiz->direita);
}

void liberarPistas(Pista* raiz) {
    if (!raiz) return;
    liberarPistas(raiz->esquerda);
    liberarPistas(raiz->direita);
    free(raiz);
}

// -------------------- ESTRUTURA: Tabela Hash (pista -> suspeito) --------------------

typedef struct Evidencia {
    char pista[MAX_DESC];
    char suspeito[MAX_NOME];
    struct Evidencia* proximo;
} Evidencia;

typedef struct {
    Evidencia* buckets[HASH_SIZE];
} HashTable;

unsigned int hash(const char* s) {
    unsigned long h = 5381;
    int c;
    while ((c = *s++))
        h = ((h << 5) + h) + c;
    return (unsigned int)(h % HASH_SIZE);
}

void inicializarHash(HashTable* ht) {
    for (int i = 0; i < HASH_SIZE; i++) ht->buckets[i] = NULL;
}

void adicionarEvidencia(HashTable* ht, const char* pista, const char* suspeito) {
    unsigned int idx = hash(pista);
    Evidencia* node = (Evidencia*) malloc
