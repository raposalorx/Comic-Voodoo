#include "http.h"

#include <cstring>
#include <curl/curl.h>
#include <curl/easy.h>
#include <stdlib.h>

HTTP::HTTP():
  mem(NULL),
  size(0)
{
}

void* myrealloc(void* ptr, size_t size)
{
  /* There might be a realloc() out there that doesn't like reallocing
     NULL pointers, so we take care of it here */
  if(ptr)
    return realloc(ptr, size);
  else
    return malloc(size);
}

size_t WriteMemoryCallback(void* ptr, size_t size, size_t nmemb, void* data)
{
  size_t realsize = size * nmemb;
  HTTP* mem = (HTTP*)data;

  mem->mem = (char*)myrealloc(mem->mem, mem->size + realsize + 1);
  if (mem->mem) {
    memcpy(&(mem->mem[mem->size]), ptr, realsize);
    mem->size += realsize;
    mem->mem[mem->size] = 0;
  }
  return realsize;
}

void get_http(HTTP& page, const std::string& url)
{
  if(page.mem != NULL) // take page.mem out behind the shed, it has been sacrificed to appease the god of mem leaks
  {
    free(page.mem);
    page.mem=NULL;
    page.size = 0;
  }

  CURL* curl_handle;
  curl_handle = curl_easy_init();
  curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
  curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void*)&page);
  curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "ComicVoodoo");
  curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());

  curl_easy_perform(curl_handle);

  long error = 0;
  CURLcode status = curl_easy_getinfo(curl_handle, CURLINFO_OS_ERRNO, &error);
  if(error != 0)
  {
    page.size = 0;
  }
  if(CURLE_OK != status)
  {
    page.size = 0;
  }
  if(CURLE_COULDNT_CONNECT == status)
  {
    page.size = 0;
  }
  if(CURLE_PARTIAL_FILE == status)
  {
    page.size = 0;
  }
  if(CURLE_OPERATION_TIMEDOUT == status)
  {
    page.size = 0;
  }
  if(CURLE_ABORTED_BY_CALLBACK == status)
  {
    page.size = 0;
  }
  if(CURLE_GOT_NOTHING == status)
  {
    page.size = 0;
  }
  if(CURLE_RECV_ERROR == status)
  {
    page.size = 0;
  }

  curl_easy_cleanup(curl_handle);
  curl_global_cleanup();
}
