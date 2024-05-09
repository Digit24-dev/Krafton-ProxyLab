#pragma once

#include <stdio.h>

#define MAX_CACHE_SIZE 1049000  // 1MB
#define MAX_CACHENODE_SIZE  30

typedef struct cache_t
{
  int size;
  int refer_cnt;
  char *url;
  void *data;
} cache_node;

typedef struct proxy_cache
{
  unsigned int total_size;
  cache_node *cache[MAX_CACHENODE_SIZE];
} proxy_cache_t;

void init_cache();
void deinit_cache();
int search_cache(char *uri);
void cache_insert(char *url, void *data, size_t data_size);
void cache_remove();