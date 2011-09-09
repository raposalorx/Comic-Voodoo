#ifndef COMIC_H
#define COMIC_H

#include <string>

class Comic
{
  // SQL
  public:
    std::string getSQLValuesStr() const throw();

  // Values
  public:
    std::string base_url;
    std::string first_url;
    std::string img_regex;
    std::string next_regex;
};

#endif // COMIC_H
