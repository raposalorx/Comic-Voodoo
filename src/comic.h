#ifndef COMIC_H
#define COMIC_H

#include <string>

struct Comic
{
  std::string name;
  std::string base_url;
  std::string first_url;
  std::string current_url;
  std::string img_regex;
  std::string next_regex;
};

#endif // COMIC_H
