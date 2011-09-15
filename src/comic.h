#ifndef COMIC_H
#define COMIC_H

#include <string>
#include <queue>
#include <pcrecpp.h>

struct Comic
{
    Comic(std::string cname, std::string cbase_url, std::string cfirst_url, std::string cimg_regex, std::string cnext_regex, std::string cend_on_url, unsigned short cread_end_url, unsigned short cdownload_imgs, unsigned short vmark, std::string vlast_url, unsigned short vlast_img, std::string vcurrent_url):
        name(cname),
        base_url(cbase_url), 
        first_url(cfirst_url),
        end_on_url(cend_on_url),
        read_end_url(cread_end_url),
        download_imgs(cdownload_imgs),

        mark(vmark),
        last_url(vlast_url),
        last_img(vlast_img),
        current_url(vcurrent_url),

        img_regex(cimg_regex),
        next_regex(cnext_regex)
    {}

    private:
    // config
    std::string name;
    std::string base_url;
    std::string first_url;
    std::string end_on_url;
    unsigned short read_end_url;
    unsigned short download_imgs;

    // vars
    unsigned short mark;
    std::string last_url;
    unsigned short last_img;
    std::string current_url;
    std::queue<std::string> new_imgs;
    std::queue<std::string> new_urls;

    // compiled regexes
    pcrecpp::RE img_regex;
    pcrecpp::RE next_regex;
};

#endif // COMIC_H
