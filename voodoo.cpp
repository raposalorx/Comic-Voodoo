#include "voodoo.h"

#include <pcrecpp.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <fstream>
#include "http.h"

#include "conversion.h"

using namespace std;

void get_img(Comic &comic, char *mem)
{
	string html = mem;
	string img;

	string value;
	string found;
	string base;
	string swap = "!";
	pcrecpp::StringPiece input(html);

	while(swap != found)
	{	
		swap = found;
		comic.img_regex.FindAndConsume(&input, &found);
		if(found == swap)
		break;

		if(found[0] != '/')
		{
			base = comic.last_url;
			base = base.substr(0, base.find_last_of('/')+1);
		}
		else
		{
			base = comic.base_url;
		}

		// not global
		if(found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.")
		value = base;
		else
			value = "";
		value += found;

		img += value += ";";
	}

	comic.new_imgs.push(img);
	if(comic.imgs.size() == 0 || comic.new_imgs.size() > 1) // cut out the duplicate that happens with each respider
	comic.imgs.push_back(img);
	printf("%s\n", img.c_str());
	fflush(stdout);
}

string get_next(Comic &comic, char *mem, string url)
{	
	string html = mem;
	string found;
	string base;
	
	comic.next_regex.PartialMatch(html, &found);
	
	if(found == "")
		return "";
	
	if(found[0] != '/')
	{
		base = url;
		base = base.substr(0, base.find_last_of('/')+1);
	}
	else
		base = comic.base_url;

	// found url is the same as the current url, with or without the #
	if
	(
		(
			(found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.") 
			&&
			(
				(!strcmp(strcomb(2, base.c_str(), found.c_str()) , url.c_str()))
				|| 
				(!strcmp(strcomb(2, base.c_str(), found.c_str()) , strcomb(2, url.c_str(), "#")))
			)
		)
		||
		(
			(found.substr(0, 7) == "http://" || found.substr(0, 4) == "www.") 
			&&
			(
				(!strcmp(found.c_str() , url.c_str()))
				|| 
				(!strcmp(found.c_str() , strcomb(2, url.c_str(), "#")))
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
			(!strcmp(strcomb(2, base.c_str(), found.c_str()), comic.end_on_url.c_str())) 
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
			string url_swap2 = url;
			
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
			get_img(comic, page.mem);
			
			// keep the url before end_on_url
			comic.url_swap = url_swap2;
			
			comic.last_url = url_swap2;
			
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
	
	comic.last_url = url;
	comic.is_new_imgs = true;
	
	printf("%s\n", url.c_str());
	fflush(stdout);
	
	return url;
}

void Spider(Comic &comic)
{
	bool done = false;
	HTTP page;
	string next = comic.last_url;
	
	get_http(page, next);
	while(!done && page.mem != NULL)
	{
		get_img(comic, page.mem);
		
		next = get_next(comic, page.mem, next);
		if(next == "")
			done = true;
		else
			get_http(page, next);
	}
	while(comic.is_new_imgs && !comic.new_imgs.empty() && comic.download_imgs)
		download_img(comic);
}

void get_http(HTTP &page, string url)
{
	if(page.mem != NULL) // take page.mem out behind the shed, it has been sacrificed to appease the gods of mem leaks
	{
		free(page.mem);
		page.mem=NULL;
		page.size = 0;
	}
	CURL *curl_handle;
	curl_handle = curl_easy_init();
	curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
	curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&page);
	curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "ComicVoodoo");
	curl_easy_setopt(curl_handle, CURLOPT_URL, url.c_str());
	
	curl_easy_perform(curl_handle);
	
	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
}

void download_img(Comic& comic)
{
	string folder = strcomb(2, getenv("HOME"), "/.comics");
	
	HTTP page;
	CURL *curl_handle;
	vector<string> img_urls;
	StringExplode(comic.new_imgs.front(), ";", &img_urls);
	comic.new_imgs.pop();
	unsigned int i = comic.last_img;

	for(unsigned int j = 0; j < img_urls.size(); j++)
	{
		string n = itoa(i);
		// pad n with 0's till it is of length 4 ex: 0001
		n.insert(0, 4-n.size(), '0');

		// add a suffix based on the sub img counter ex: 0001b
		string suffix = "";
		if(j > 0)
			suffix.push_back('a'+j-1);
	
		string filename = strcomb(7, folder.c_str(), "/comics/", comic.name.c_str(), "/",  n.c_str(), suffix.c_str(), img_urls.at(j).substr(img_urls.at(j).find_last_of('.')).c_str());
		
		if(!FileExists(filename))
		{
			curl_handle = curl_easy_init();
			curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, WriteMemoryCallback);
			curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *)&page);
			curl_easy_setopt(curl_handle, CURLOPT_USERAGENT, "ComicVoodoo");
			curl_easy_setopt(curl_handle, CURLOPT_URL, img_urls.at(j).c_str());

			curl_easy_perform(curl_handle);

			fstream fout(filename.c_str(), ios::out|ios::binary|ios::trunc);
			fout.write((char *)page.mem, page.size);
			fout.close();

			curl_easy_cleanup(curl_handle);
			curl_global_cleanup();

			printf("%s -> %s\n", img_urls.at(j).c_str(), filename.c_str());
			fflush(stdout);

			free(page.mem);
			page.mem=NULL;
			page.size = 0;
		}
	}
	comic.last_img++;
}
