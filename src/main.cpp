#include <iostream>
#include "spider.h"

int main(int, char**)
{
  Comic comic;
  comic.name = "Schlock";
  comic.base_url = "http://www.schlockmercenary.com";
  comic.first_url = "http://www.schlockmercenary.com/2011-09-22";
  comic.end_on_url = "http://www.schlockmercenary.com/index.html";
  comic.read_end_url = 1;
  comic.download_imgs = 0;
  comic.current_url = comic.first_url;
  comic.current_id = 0;
  comic.img_regex = "(/comics/schlock[0-9]{8}[a-z]?.*?\\.(jpg|gif|png|tiff))";
  comic.next_regex = "(/|(/[0-9]{4}-[0-9]{2}-[0-9]{2})). id=.nav-next";

  Cache* cache = new Cache(".", "cache");
  cache->createCacheDb();
  cache->addComic(comic);

  Spider spider("", comic, cache);

  std::string current = comic.current_url;
  Strip* strip;
  
  do
  {
    strip = spider.fetchStrip();
    if(strip==NULL)
      break;
    std::cout << strip->page << "\n" << strip->imgs << std::endl;
  }while(strip!=NULL);
  
  delete cache;

  return 0;
}
