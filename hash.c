#include "hash.h"

pair* make_pair(char* key, int value)
{
	pair* new_pair = (pair*)malloc(sizeof(pair));

	new_pair->key = strdup(key);
	new_pair->value = value;

	return new_pair;
}

void initialize(hash* hash)
{
	hash->bucket = NULL;
	hash->capacity = 0;
	hash->size = 0;
}

static unsigned int hasing(unsigned char *str)
{
	unsigned int hash = 5381;

	while (*str != '\0')
	{
		hash = hash * 33 + *str;
		++str;
	}
	return hash;
}

void insert(hash* hash, pair* data)
{
	if (hash->capacity == 0
		|| (double)hash->size / hash->capacity > MAX_LOAD_FACTOR)
		re_allocate(hash, hash->capacity * 2);
	
	unsigned int hash_value = hasing((unsigned char*)data->key);
	unsigned int idx = hash_value;

	while (hash->bucket[idx % hash->capacity].state == USED)
	{
		if (strcmp(hash->bucket[idx % hash->capacity].data->key, data->key) == 0)
			return;
		++idx;
	}

	hash->bucket[idx % hash->capacity] = (node){data, hash_value, USED};
	++hash->size;
}

static int find_idx(hash* hash, char* key)
{
	unsigned int hash_value = hasing((unsigned char*)key);
	unsigned int idx = hash_value;

	for (int i = 0; i < hash->capacity; ++i, ++idx)
	{
		if (hash->bucket[idx % hash->capacity].state == EMPTY)
			break;
		if (hash->bucket[idx % hash->capacity].state == DELETED)
			continue;
		if (strcmp(hash->bucket[idx % hash->capacity].data->key, key) == 0)
			return idx % hash->capacity;
	}
	return -1;
}

void erase(hash* hash, char* key)
{
	int idx = find_idx(hash, key);
	if (idx == -1)
		return;
	
	free(hash->bucket[idx].data->key);
	free(hash->bucket[idx].data);
	hash->bucket[idx].state = DELETED;
	--hash->size;
}

pair* find(hash* hash, char* key)
{
	int idx = find_idx(hash, key);
	if (idx == -1)
		return NULL;
	
	return hash->bucket[idx].data;
}

void clear(hash* hash)
{
	for (int i = 0; i < hash->capacity; ++i)
	{
		if (hash->bucket[i].state == USED)
		{
			free(hash->bucket[i].data->key);
			free(hash->bucket[i].data);
		}
	}
	free(hash->bucket);
	initialize(hash);
}

static void re_allocate(hash* hash, int size)
{
	int prev_capacity = hash->capacity;

	if (size == 0)
		hash->capacity = INITIAL_CAPACITY;
	else
		hash->capacity = get_next_prime(size);

	node* new_bucket = (node*)malloc(hash->capacity * sizeof(node));
	for (int i = 0; i < hash->capacity; ++i)
		new_bucket[i].state = EMPTY;

	for (int i = 0; i < prev_capacity; ++i)
	{
		if (hash->bucket[i].state != USED)
			continue;
		unsigned int idx = hash->bucket[i].hash_value;

		while (new_bucket[idx % hash->capacity].state == USED)
		{
			if (strcmp(new_bucket[idx % hash->capacity].data->key, hash->bucket[i].data->key) == 0)
				return;
			++idx;
		}

		new_bucket[idx % hash->capacity] =
			(node){hash->bucket[i].data, hash->bucket[i].hash_value, USED};
	}
	free(hash->bucket);
	hash->bucket = new_bucket;
}