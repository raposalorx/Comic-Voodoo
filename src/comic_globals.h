#ifndef COMIC_GLOBALS
#define COMIC_GLOBALS

#include <string>
#include "conversion.h"

const static std::string folder = strcomb(2, getenv("HOME"), "/.comics");
const static std::string comicsfile = strcomb(2, getenv("HOME"), "/.comics/comics.yaml");
const static std::string settingsfile = strcomb(2, getenv("HOME"), "/.comics/config.yaml");

#endif
