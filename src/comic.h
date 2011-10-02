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
  unsigned short read_end_url;
  unsigned short download_imgs;

  // Regex
  std::string img_regex;
  std::string next_regex;

  // Spider state data
  unsigned short mark;
  std::string current_url;
  unsigned short current_id;
  unsigned short instance_start_id;
  std::queue<std::string> new_imgs;
  std::queue<std::string> new_urls;
};

#endif // COMIC_H
