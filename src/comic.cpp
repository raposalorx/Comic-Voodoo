#include "comic.h"

// --------------------------------------------------------------------------------
//  SQL
// --------------------------------------------------------------------------------

std::string Comic::getSQLValuesStr() const throw()
{
  return "'" + base_url + "','" + first_url + "','" + img_regex + "','" + next_regex + "'";
}
