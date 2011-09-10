#include "spider.h"

#include "strip.h"


// --------------------------------------------------------------------------------
//  Ctor
// --------------------------------------------------------------------------------

Spider::Spider(const std::string& picture_dir, const Comic& comic) throw():
  picture_dir(picture_dir + '/' + comic.name),
  comic(comic),
  current_id(comic.current_id),
  current_url(comic.current_url.empty() ? comic.first_url : comic.current_url)
{
}


// --------------------------------------------------------------------------------
//  Spidering
// --------------------------------------------------------------------------------

Strip* Spider::fetchNextStrip() throw(/* TODO */)
{
  // TODO - should fetch the page at current_url, create a Strip object from the contents, download the comic images to picture_dir, and figure out what the next value of current_url should be
  //      - should return NULL when there are no more strips to fetch; you'll probably have to change around the data used for update checks that is stored in struct Comic (currently only Comic::current_url is used for update checks)
  //      - check src/cache.h and src/cache.cpp for examples of how the exception system works in this project
  //      - also check docs/adding_columns for a list of places to update when adding members to struct Comic or struct Strip
  return NULL;
}

const std::string& Spider::getCurrentURL() const throw()
{
  return current_url;
}
