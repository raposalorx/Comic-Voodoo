#ifndef SPIDER_H
#define SPIDER_H

#include <string>
#include <vector>

#include "cache.h"
#include "comic.h"
#include "http.h"
#include "strip.h"

class Spider
{
  // Exceptions
  public:
    EXCEPTION_DEF(E_ConnectionFailed, const std::string&);
    EXCEPTION_DEF(E_ImgFindFailed, const std::string&);
    EXCEPTION_DEF(E_ImgWriteFailed, const std::string&);

  // Ctor
  public:
    Spider(const std::string&, Comic&, Cache*) throw();

  // Spidering
  public:
    Strip* fetchStrip() throw(E_ConnectionFailed, E_ImgFindFailed, E_ImgWriteFailed, Cache::E_CacheDbError);
    std::vector<Strip*>* fetchAllStrips() throw(E_ConnectionFailed, E_ImgFindFailed, E_ImgWriteFailed, Cache::E_CacheDbError);
    const std::string& getCurrentURL() const throw();
  private:
    Strip* getImgs(const char* mem, const std::string url) throw(E_ImgFindFailed, Cache::E_CacheDbError);
    std::string getNext(char* mem, const std::string url) throw();
    int current_id;
    std::string current_url;
    std::string last_imgs;
    Cache* cache;

  // Comic settings
  private:
    Comic comic;
    const std::string picture_dir;
    pcrecpp::RE img_regex;
    pcrecpp::RE next_regex;
};

#endif // SPIDER_H
