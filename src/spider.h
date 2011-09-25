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
    EXCEPTION(E_ConnectionFailed, "Could not connect to the url: " + url + "; ", const std::string& url);
    EXCEPTION(E_ImgFindFailed, "Could not find any images on " + url + "; ", const std::string& url);
    EXCEPTION(E_ImgWriteFailed, "Could not write the image(s) to disk at: " + dir + "; ", const std::string& dir);

    Spider(const std::string&, Comic&, Cache*) throw();

  // Spidering
  public:
    Strip* fetchStrip() throw(E_ConnectionFailed, E_ImgFindFailed, E_ImgWriteFailed, Cache::E_CacheDbError);
    std::vector<Strip*>* fetchAllStrips() throw(E_ConnectionFailed, E_ImgFindFailed, E_ImgWriteFailed, Cache::E_CacheDbError);
    const std::string& getCurrentURL() const throw();
  private:
    Strip* getImgs(const char *mem, const std::string url) throw(E_ImgFindFailed, Cache::E_CacheDbError);
    std::string getNext(char *mem, const std::string url) throw();
    const std::string picture_dir;
    Comic comic;
    int current_id;
    
    std::string current_url;

    std::string last_imgs;

    Cache* cache;

    pcrecpp::RE img_regex;
    pcrecpp::RE next_regex;
};

#endif // SPIDER_H
