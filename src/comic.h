#ifndef COMIC_H
#define COMIC_H

#include <string>
#include <queue>
#include <vector>
#include <pcrecpp.h>

struct Comic
{
	Comic() : name(""), 
	base_url(""),
	first_url(""),
	last_url(""),
	last_img(1),
	end_on_url(""),
	mark(0),
	is_new_imgs(false),
	download_imgs(false),
	imgs(0),
	urls(0),
	url_swap(""),
	url(""),
	unread(0),
	img_regex(""),
	next_regex("")
	{}
	
	std::string name;
	std::string base_url;
	std::string first_url;
	std::string last_url;
	unsigned short last_img;
	std::string end_on_url;
	unsigned short read_end_url;
	unsigned short mark;
	
	bool is_new_imgs;
	bool download_imgs;
	
	std::queue<std::string> new_imgs;
	std::vector<std::string> imgs;
	std::vector<std::string> urls;
	
	std::string url_swap;
	std::string url;
	
	unsigned short unread;
	
	pcrecpp::RE img_regex; // regex command to find images
	pcrecpp::RE next_regex; // regex command to find next links
	// pcrecpp::RE redirect_regex;
};

#endif
