#ifndef SPIDER_H
#define SPIDER_H

#include <string>

#include "comic.h"

struct Strip;

class Spider
{
  // Ctor
  public:
    Spider(const std::string&, const Comic&) throw();

  // Spidering
  public:
    Strip* fetchNextStrip() throw(/* TODO */);
    const std::string& getCurrentURL() const throw();
  private:
    const std::string picture_dir;
    const Comic comic;
    int current_id;
    std::string current_url;
};

#endif // SPIDER_H
