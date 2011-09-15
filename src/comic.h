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
  std::string last_url;
  unsigned short last_img;
  std::string current_url;
  std::queue<std::string> new_imgs;
  std::queue<std::string> new_urls;

  // compiled regexes
  pcrecpp::RE img_regex;
  pcrecpp::RE next_regex;
};

#endif // COMIC_H
