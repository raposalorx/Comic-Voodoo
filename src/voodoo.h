#ifndef VOODOO_H
#define VOODOO_H

#include <string>

#include "comic.h"
#include "http.h"

void get_img(Comic &comic, const char *mem, const std::string url);
std::string get_img_urls(const Comic &comic, const std::string html);
std::string get_next(Comic &comic, char *mem, std::string url);
void Spider(Comic &comic);
void get_http(HTTP &page, std::string url);
void download_img(Comic& comic);
void saveImgFile(Comic& comic);
void saveUrlFile(Comic& comic);
void saveSettingsFile(Comic &comic);

#endif
