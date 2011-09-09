#include "comic.h"

#define SQL_COLS "'base_url','first_url','img_regex','next_regex'"

// --------------------------------------------------------------------------------
//  SQL
// --------------------------------------------------------------------------------

std::string Comic::getSQLInsertStr(const std::string& table_name, const std::string& comic_name) const throw()
{
  return "insert into `" + table_name + "` ('name'," SQL_COLS ") values ('" + comic_name + "','" + base_url + "','" + first_url + "','" + img_regex + "','" + next_regex + "');";
}

std::string Comic::getSQLSelectStr(const std::string& table_name, const std::string& comic_name) const throw()
{
  return "select " SQL_COLS " from `" + table_name + "` where `name` = '" + comic_name + "';";
}
