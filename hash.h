#include <stdio.h>
#include <string.h>
#include <stdlib.h>
    
#define MAX_LENGTH 1000

typedef struct node node;

typedef struct node {
    char *key;
    char *path;
    struct node *next;
} node;

typedef struct hashmap {
    node *node_array[MAX_LENGTH];
} hashmap;

int hash_value(char *key, int length) {
    int hash = 0;
    char temp = 0;

    for (int i = 0; i < strlen(key); i++) {
        temp = key[i];
        hash += hash * 31 + (temp - 48);
    }

    return hash % length;
}

char *get(char *key, hashmap *hashmap) {
    int index = hash_value(key, MAX_LENGTH);
    node *temp_node = hashmap->node_array[index];

    if (temp_node != NULL) {
        if (strcmp(temp_node->key, key) == 0) {
            return temp_node->path;
        }
        temp_node = temp_node->next;
    }
    return NULL;
}

void put(char *key, char *path, hashmap *hashmap) {
    int index  = hash_value(key, MAX_LENGTH);
    node *new_node = malloc(sizeof(node));
    if (!new_node) {
        perror("Failed to allocate memory");
        exit(EXIT_FAILURE);
    }
    new_node->key = strdup(key);
    new_node->path = strdup(path);
    new_node->next = NULL;
    if (hashmap->node_array[index] == NULL) {
        hashmap->node_array[index] = new_node;
        return;
    }

    node *temp_node = hashmap->node_array[index];
    while (temp_node->next != NULL) {
        temp_node = temp_node->next;
    }
    temp_node->next = new_node;
}

void remove_value(char *key, hashmap *hashmap) {
    int index = hash_value(key, MAX_LENGTH);
    if (hashmap->node_array[index] == NULL) {
        fprintf(stderr, "No value in given key");
        return;
    }

    node *temp_node = hashmap->node_array[index];
    if (strcmp(temp_node->key, key) == 0) {
        hashmap->node_array[index] = temp_node->next;
    }
    if (temp_node->next != NULL) {
        while (temp_node->next != NULL) {
            if (strcmp(temp_node->next->key,key) == 0) {
                temp_node->next = temp_node->next->next;
                break;
            }
            temp_node = temp_node->next;
        }
    } else {
        hashmap->node_array[index] = NULL;
    }
}

int main(void) {
    hashmap hashmap = { 0 };
    char key[] = "ls";
    char path[] = "/User/bin/path/ls";
    put(key, path, &hashmap);
    char *result = get(key, &hashmap);
    if (result != NULL) {
        printf("%s", result);
    }
}


