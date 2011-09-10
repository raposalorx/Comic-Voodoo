#ifndef SPIDER_H
#define SPIDER_H

#include <string>

#include "comic.h"
#include "strip.h"

class Spider
{
  // Ctor
  public:
    explicit Spider(const Comic&) throw();

  // Spidering
  public:
    Strip* fetchNextStrip() throw(/* TODO */);
    const std::string& getCurrentURL() const throw();
  private:
    const Comic comic;
    int current_id;
    std::string current_url;
};

#endif // SPIDER_H
