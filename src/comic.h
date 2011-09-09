#ifndef COMIC_H
#define COMIC_H

#include <string>

class Comic
{
  // SQL
  public:
    std::string getSQLColumnsStr() const throw();
    std::string getSQLValuesStr() const throw();
    static const std::string sql_select_cols;
};

#endif // COMIC_H
