#include "comic.h"

// --------------------------------------------------------------------------------
//  SQL
// --------------------------------------------------------------------------------

std::string Comic::getSQLInsertStr(const std::string& table_name, const std::string& comic_name) const throw()
{
  return "INSERT INTO `" + table_name + "` "
         "("
           "`name`,"
           "`base_url`,"
           "`first_url`,"
           "`img_regex`,"
           "`next_regex`"
         ") "
         "VALUES "
         "("
           "'" + comic_name + "',"
           "'" + base_url   + "',"
           "'" + first_url  + "',"
           "'" + img_regex  + "',"
           "'" + next_regex + "'"
         ");";
}

std::string Comic::getSQLSelectStr(const std::string& table_name, const std::string& comic_name) const throw()
{
  return "SELECT "
           "`base_url`,"
           "`first_url`,"
           "`img_regex`,"
           "`next_regex` "
         "FROM `" + table_name + "` "
         "WHERE `name`='" + comic_name + "';";
}

std::string Comic::getSQLUpdateStr(const std::string& table_name, const std::string& comic_name) const throw()
{
  return "UPDATE `" + table_name + "` "
         "SET "
           "`base_url`='"   + base_url   + "',"
           "`first_url`='"  + first_url  + "',"
           "`img_regex`='"  + img_regex  + "',"
           "`next_regex`='" + next-regex + "' "
         "WHERE `name`='" + comic_name + "';";
}
