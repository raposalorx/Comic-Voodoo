#ifndef COMIC_H
#define COMIC_H

#include <queue>
#include <pcrecpp.h>
#include <string>

struct Comic
{
  // Config
  std::string name;
  std::string base_url;
  std::string first_url;
  std::string end_on_url;
  bool read_end_url;
  bool download_imgs;
  std::string searchpattern;

  // Regex
  std::string img_regex;
  std::string next_regex;

  // Spider state data
  signed short mark;
  bool watched;
  std::string current_url;
  unsigned short current_id;
  std::queue<std::string> new_imgs;
  std::queue<std::string> new_urls;
};

#endif // COMIC_H
