#include <iostream>
#include "spider.h"

int main(int, char**)
{
  Comic* comic = new Comic;
  comic->name = "Super Effective";
  comic->base_url = "http://www.vgcats.com";
  comic->first_url = "http://www.vgcats.com/super/?strip_id=0";
  comic->read_end_url = 0;
  comic->download_imgs = 0;
  comic->current_url = comic->first_url;
  comic->current_id = 0;
  comic->img_regex = "(images/[0-9]{6}.gif)";
  comic->next_regex = "(\\?strip_id=[0-9]*)\">[^<]{0,}<img[^src]{0,}src=\"next.gif";

  Cache* cache = new Cache(".", "cache");
  cache->createCacheDb();
  cache->addComic(*comic);

  Spider spider("", comic, cache);

  std::string current = comic->current_url;
  Strip* strip = spider.fetchStrip();
  
  std::cout << strip->page << "\n" << strip->imgs << std::endl;

  delete comic;
  delete cache;

  return 0;
}
