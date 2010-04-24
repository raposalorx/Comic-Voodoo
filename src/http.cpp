#include "http.h"

#include <stdlib.h>

void *myrealloc(void *ptr, size_t size)
{
  /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

size_t WriteMemoryCallback(void *ptr, size_t size, size_t nmemb, void *data)
{
  size_t realsize = size * nmemb;
  HTTP *mem = (HTTP *)data;

  mem->mem = (char*)myrealloc(mem->mem, mem->size + realsize + 1);
  if (mem->mem) {
    memcpy(&(mem->mem[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->mem[mem->size] = 0;
  }
  return realsize;
}
