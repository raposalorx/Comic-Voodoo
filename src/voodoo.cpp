#include "voodoo.h"

#include <pcrecpp.h>

#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include <fstream>
#include "http.h"

#include "conversion.h"
#include "comic_globals.h"
#include "yaml-cpp/yaml.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

void get_img(Comic &comic, const char *mem, const string url)
{
	const string img = get_img_urls(comic, mem);

	if(comic.imgs.size() == 0 || (img!=comic.imgs.back() && comic.skipped))
	{	// cut out the duplicate that happens with each respider and duplicates from reading the end_on url
		comic.imgs.push_back(img);
		comic.new_imgs.push(img);
		comic.urls.push_back(url);
	}
	else
		comic.skipped = true;

	//DEBUG
	printf("%s\n", img.c_str());
	fflush(stdout);
}

string get_img_urls(const Comic &comic, const string html)
{
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

		if(found[0] == '/')
			base = comic.base_url;
		else
		{
			base = comic.last_url;
			base = base.substr(0, base.find_last_of('/')+1);
		}

		// not global
		if(found.substr(0, 7) != "http://" && found.substr(0, 4) != "www.")
			value = base;
		else
			value = "";
		value += found;

		// sepparate images with ;
		img += value += ";";
	}

	return img;
}

string get_next(Comic &comic, char *mem, string url)
{	
	const string html = mem;
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
			get_img(comic, page.mem, url);
			
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

	//DEBUG	
	printf("%s\n", url.c_str());
	fflush(stdout);
	
	return url;
}

void Spider(Comic &comic)
{
	cout << "spidering " << comic.name << endl;
	bool done = false;
	HTTP page;
	string next = comic.last_url;
	
	get_http(page, next);
	while(!done && page.mem != NULL)
	{
		get_img(comic, page.mem, next);
		
		next = get_next(comic, page.mem, next);
		if(next == "")
			done = true;
		else
			get_http(page, next);
	}
	cout << comic.download_imgs << endl; //DEBUG
	while(comic.is_new_imgs && !comic.new_imgs.empty() && comic.download_imgs)
		download_img(comic);
	saveImgFile(comic);
	saveUrlFile(comic);
	saveSettingsFile(comic);
}

void get_http(HTTP &page, const string url)
{
	if(page.mem != NULL) // take page.mem out behind the shed, it has been sacrificed to appease the god of mem leaks
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
	const string folder = strcomb(2, getenv("HOME"), "/.comics");
	
	HTTP page;
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
		if(chdir("/tmp/comics"))
			system("mkdir -p /tmp/comics");
		const string tmpfile = strcomb(7, "/tmp/comics/",  n.c_str(), suffix.c_str(), img_urls.at(j).substr(img_urls.at(j).find_last_of('.')).c_str());
		const string filename = strcomb(7, folder.c_str(), "/comics/", comic.name.c_str(), "/",  n.c_str(), suffix.c_str(), ".png");

		if(!FileExists(filename))
		{
			get_http(page, img_urls.at(j));
			
			std::fstream fout(tmpfile.c_str(), std::ios::out|std::ios::binary|std::ios::trunc);
			fout.write((char *)page.mem, page.size);
			fout.close();

			system(strcomb(0, "convert ", tmpfile.c_str(), " \"", filename.c_str(), "\""));

			printf("%s -> %s\n", img_urls.at(j).c_str(), filename.c_str());
			fflush(stdout);

			free(page.mem);
			page.mem=NULL;
			page.size = 0;
		}
	}
	comic.last_img++;
}

void saveImgFile(Comic& comic)
{
	std::fstream comics_file(strcomb(4, folder.c_str(), "/comics/", comic.name.c_str(), "/imgs.yaml"), std::ios::out|std::ios::trunc);
	for(unsigned int i = 0; i < comic.imgs.size(); i++)
	{
		string s = strcomb(3, "- ", comic.imgs.at(i).c_str(), "\n");
		comics_file.write(s.c_str(), s.size());
	}
	comics_file.close();
}

void saveUrlFile(Comic& comic)
{
	std::fstream comics_file(strcomb(4, folder.c_str(), "/comics/", comic.name.c_str(), "/urls.yaml"), std::ios::out|std::ios::trunc);
	for(unsigned int i = 0; i < comic.urls.size(); i++)
	{
		string s = strcomb(3, "- ", comic.urls.at(i).c_str(), "\n");
		comics_file.write(s.c_str(), s.size());
	}
	comics_file.close();
}

void saveSettingsFile(Comic &comic)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "last_url";
	out << YAML::Value << comic.last_url;
	out << YAML::Key << "download_urls";
	out << YAML::Value << comic.download_imgs;
	out << YAML::Key << "mark";
	out << YAML::Value << comic.mark;
	out << YAML::EndMap;

	std::fstream fout(strcomb(4, folder.c_str(), "/comics/", comic.name.c_str(), "/settings.yaml"), std::ios::out|std::ios::trunc);
	// cout << "saving: " << strcomb(4, folder.c_str(), "/comics/", comic.name.c_str(), "/settings.yaml") << endl;
	fout.write(out.c_str(), out.size());
	fout.close();
}
