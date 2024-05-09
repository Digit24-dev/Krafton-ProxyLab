#include <stdio.h>

#define INITIAL_CAPACITY 7
#define MAX_LOAD_FACTOR 0.5

typedef struct pair
{
	char* key;
	int value;
} pair;

typedef enum state
{
	EMPTY,
	USED,
	DELETED
} state;

typedef struct node
{
	pair* data;
	unsigned int hash_value;
	state state;
} node;

typedef struct hash
{
	node* bucket;
	int size;
	int capacity;
} hash;