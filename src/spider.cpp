#include "spider.h"


// --------------------------------------------------------------------------------
//  Ctor
// --------------------------------------------------------------------------------

Spider::Spider(const Comic& comic) throw():
  comic(comic),
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
