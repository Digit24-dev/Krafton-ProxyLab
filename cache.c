#include "sbuf.h"
#include "cache.h"

extern proxy_cache_t pcache;
sbuf_t sbuf_cache;

void init_cache()
{
  pcache.total_size = 0;
  for (size_t i = 0; i < MAX_CACHENODE_SIZE; i++)
  {
    pcache.cache[i] = NULL;
  }
}

void deinit_cache()
{
  for (size_t i = 0; i < MAX_CACHENODE_SIZE; i++)
  {
    if (pcache.cache[i] == NULL) continue;
    Free(pcache.cache[i]->data);
    Free(pcache.cache[i]->url);
  }
  // Free(pcache.cache);
}

// return hit(1) or miss(0)
int search_cache(char *uri)
{
  for (size_t i = 0; i < MAX_CACHENODE_SIZE; i++)
  {
    if (pcache.cache[i] == NULL) continue;
    else if (strcmp(pcache.cache[i]->url, uri)) {
      pcache.cache[i]->refer_cnt++;
      return 1;
    }
  }
  return 0;
}

void cache_insert(char *url, void *data, size_t data_size)
{
  // POLICY : LRU
  int max = 0, idx = 0;
  
  // pass if data_size is bigger than MAX_CACHE_SIZE
  if (data_size > MAX_CACHE_SIZE) return;

  // remove while affordable.
  while (data_size > MAX_CACHE_SIZE - pcache.total_size)
  {
    cache_remove();
  }

  for (size_t i = 0; i < MAX_CACHENODE_SIZE; i++)
  {
    if (pcache.cache[i] != NULL) continue;

    P(&sbuf_cache.slots);
    P(&sbuf_cache.mutex);

    pcache.cache[i]->url = url;
    pcache.cache[i]->refer_cnt = 0;
    pcache.cache[i]->size = data_size;
    pcache.cache[i]->data = data;
    pcache.total_size += data_size;
    
    V(&sbuf_cache.items);
    V(&sbuf_cache.mutex);

    break;
  }
}

void cache_remove()
{
  // POLICY : LRU
  int min = __INT32_MAX__, idx = 0;
  P(&sbuf_cache.items);
  P(&sbuf_cache.mutex);
  
  V(&sbuf_cache.mutex);
  V(&sbuf_cache.slots);
}