#ifndef HTTP_H
#define HTTP_H

#include <string>

struct HTTP
{
  HTTP();
  char *mem;
  size_t size;
};

void *myrealloc(void* ptr, size_t size);
size_t WriteMemoryCallback(void* ptr, size_t size, size_t nmemb, void* data);
void get_http(HTTP& page, const std::string& url);

#endif
