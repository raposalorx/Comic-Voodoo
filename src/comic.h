#ifndef COMIC_H
#define COMIC_H

#include <string>
#include <queue>
#include <pcrecpp.h>

struct Comic
{
  // config
  std::string name;
  std::string base_url;
  std::string first_url;
  std::string end_on_url;
  unsigned short read_end_url;
  unsigned short download_imgs;

  // vars
  unsigned short mark;
  std::string current_url;
  unsigned short current_id;
  std::queue<std::string> new_imgs;
  std::queue<std::string> new_urls;
  
  std::string img_regex;
  std::string next_regex;
};

#endif // COMIC_H
