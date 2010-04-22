#ifndef VOODOO_H
#define VOODOO_H

#include <string>

#include "comic.h"
#include "http.h"

void get_img(Comic &comic, char *mem);
std::string get_next(Comic &comic, char *mem, std::string url);
void Spider(Comic &comic);
void get_http(HTTP &page, std::string url);
void download_img(Comic& comic);

#endif
