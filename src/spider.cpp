#include "spider.h"

#include <iostream>
#include <memory>

#include "strip.h"


// --------------------------------------------------------------------------------
//  Exceptions
// --------------------------------------------------------------------------------

Spider::EXCEPTION_CTOR(E_ConnectionFailed, "Could not connect to the url: " + url + "; ", const std::string& url);
Spider::EXCEPTION_CTOR(E_ImgFindFailed, "Could not find any images on " + url + "; ", const std::string& url);
Spider::EXCEPTION_CTOR(E_ImgWriteFailed, "Could not write the image(s) to disk at: " + dir + "; ", const std::string& dir);


// --------------------------------------------------------------------------------
//  Ctor
// --------------------------------------------------------------------------------

Spider::Spider(const std::string& picture_dir, const Comic& comic, Cache* cache) throw():
  current_id(comic.current_id),
  current_url(comic.current_url),
  done(false),
  cache(cache),
  comic(comic),
  picture_dir(picture_dir + '/' + comic.name),
  img_regex(comic.img_regex),
  next_regex(comic.next_regex),
  read_end_url_swap(NULL)
{
}


// --------------------------------------------------------------------------------
//  Spidering
// --------------------------------------------------------------------------------

std::vector<Strip*>* Spider::fetchAllStrips() throw(E_ConnectionFailed, E_ImgFindFailed, E_ImgWriteFailed, Cache::E_CacheDbError)
{
  std::auto_ptr<std::vector<Strip*> > strips(new std::vector<Strip*>);
  Strip* swap;
  while((swap = fetchStrip()) != NULL)
  {
    strips->push_back(swap);
  }

  return strips.release();
}

Strip* Spider::fetchStrip() throw(E_ConnectionFailed, E_ImgFindFailed, E_ImgWriteFailed, Cache::E_CacheDbError)
{
  // TODO - should fetch the page at current_url, create a Strip object from the contents, download the comic images to picture_dir, and figure out what the next value of current_url should be
  //      - should return NULL when there are no more strips to fetch; you'll probably have to change around the data used for update checks that is stored in struct Comic (currently only Comic::current_url is used for update checks)
  //      - check src/cache.h and src/cache.cpp for examples of how the exception system works in this project
  //      - also check docs/adding_columns for a list of places to update when adding members to struct Comic or struct Strip
  if(done)
  {
    if(read_end_url_swap != NULL)
    {
      Strip* swap = read_end_url_swap;
      read_end_url_swap = NULL;
      swap->id = swap->id+1;
      return swap;
    }
    return NULL;
  }

  HTTP page;
  get_http(page, current_url);
  if(page.mem != NULL)
  {
    Strip* strip = getImgs(page.mem, current_url);
    std::string next = getNext(page.mem, current_url);
//    std::cout << "next: " << next << std::endl;
    if(next != "")
    {
      current_url = next;
      current_id += 1;
    }
    else
    {
      done = true;
      if(comic.read_end_url)
      {
      }
    }
    return strip;
  }
  return NULL;
}

Strip* Spider::getImgs(const char* mem, const std::string url) throw(E_ImgFindFailed, Cache::E_CacheDbError)
{
  using std::string;
  pcrecpp::StringPiece input(mem);
  string found;
  string found_swap = "!";
  string imgs;

  while(found != found_swap) // Get ; seperated list of images.
  {
    string base;
    string value;

    found_swap = found;
    img_regex.FindAndConsume(&input, &found);

    if(found == found_swap)
      break; // No images found.
    if(found[0] == '/') // Links to the root dir?
      base = comic.base_url;
    else
    {
      base = url;
      base = base.substr(0, base.find_last_of('/')+1);
    }
    if(found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.") // Not global?
      value = base;
    else
      value = "";

    value += found;
    imgs += value += ";";
  }

  if(imgs=="")
  {
    throw E_ImgFindFailed(url);
  }

  bool no_previous = false;
  Strip* previous;
  try
  {
    previous = cache->getStrip(current_id, comic.name);
  }
  catch(Cache::E_NoStripFound e)
  {
    no_previous = true;
  }

  if(last_imgs=="" || imgs!=last_imgs)//no_previous || ((last_imgs=="" || imgs!=last_imgs) && imgs!=previous->imgs))
  { // cut out the duplicate that happens with each respider and duplicates from reading the end_on url
    std::auto_ptr<Strip> strip(new Strip);
    strip->id = current_id;
    strip->comic_name = comic.name;
    strip->page = url;
    strip->imgs = imgs;

    last_imgs = imgs;

    return strip.release();
  }
    return NULL;
}

std::string Spider::getNext(char* mem, std::string url) throw()
{
  using std::string;
  string found;
  string base;

  next_regex.PartialMatch(mem, &found);

  if(found == "")
    return "";

  if(found[0] != '/') // Doesn't point to root.
  {
      base = url;
      base = base.substr(0, base.find_last_of('/')+1);
  }
  else
    base = comic.base_url;

  // found url is the same as the current url, with or without the #¬
  if(
     (
      (found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.")
      &&
      (
       (!strcmp((base + found).c_str(), url.c_str()))//(!strcmp(strcomb(2, base.c_str(), found.c_str()) , url.c_str()))
       ||
       (!strcmp((base + found).c_str(), (url + "#").c_str()))//(!strcmp(strcomb(2, base.c_str(), found.c_str()) , strcomb(2, url.c_str(), "#")))
      )
     )
     ||
     (
      (found.substr(0, 7) == "http://" || found.substr(0, 4) == "www.")
      &&
      (
       (!strcmp(found.c_str() , url.c_str()))
       ||
       (!strcmp(found.c_str() , (url + "#").c_str()))//(!strcmp(found.c_str() , strcomb(2, url.c_str(), "#")))
      )
     )
    )
    return "";

  // found url is the end_on_url
  if
    (
     (
      (found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.")
      &&
      (!strcmp((base + found).c_str(), comic.end_on_url.c_str()))//(!strcmp(strcomb(2, base.c_str(), found.c_str()), comic.end_on_url.c_str()))
     )
     ||
     (
      (found.substr(0, 7) == "http://" || found.substr(0, 4) == "www.")
      &&
      (!strcmp(found.c_str(), comic.end_on_url.c_str()))
     )
    )
    {
      // you want to download the img on end_on_url?
      if(comic.read_end_url)
      {
        string url_swap = url;

        // not global
        if(found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.")
          url = base;
        else
          url = "";

        url += found;

        HTTP page;
        get_http(page, url);
        if(page.mem == NULL)
          return "";

        // img on end_on_url
        read_end_url_swap = getImgs(page.mem, url);

        // keep the url before end_on_url
        current_url = url_swap;

        free(page.mem);
        page.mem=NULL;
        page.size = 0;
      }
      return "";
    }

  // not global
  if(found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.")
    url = base;
  else
    url = "";

  url += found;

 current_url = url;

  return url;
}

const std::string& Spider::getCurrentURL() const throw()
{
  return current_url;
}
