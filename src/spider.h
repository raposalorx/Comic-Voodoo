#ifndef SPIDER_H
#define SPIDER_H

#include <string>
#include <vector>

#include "comic.h"
#include "http.h"
#include "strip.h"

#include "cache.h"

class Spider
{
  // Ctor
  public:
    Spider(const std::string&, Comic*, Cache*) throw();

  // Spidering
  public:
    Strip* fetchStrip() throw(/* TODO */);
    std::vector<Strip*>* fetchAllStrips() throw(/* TODO */);
    const std::string& getCurrentURL() const throw();
  private:
    Strip* getImgs(const char *mem, const std::string url) throw(/* TODO */);
    std::string getNext(char *mem, const std::string url) throw(/* TODO */);
    const std::string picture_dir;
    Comic* comic;
    int current_id;
    
    std::string current_url;

    Cache* cache;

    pcrecpp::RE img_regex;
    pcrecpp::RE next_regex;
};

#endif // SPIDER_H
