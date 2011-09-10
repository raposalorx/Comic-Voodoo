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
  // TODO
  return NULL;
}

const std::string& Spider::getCurrentURL() const throw()
{
  return current_url;
}
