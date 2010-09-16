#include "yaml-cpp/yaml.h"
#include <fstream>
#include <iostream>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "conversion.h"
#include <pcrecpp.h>

#include "voodoo.h"
#include "comic.h"
#include "http.h"
#include "comic_globals.h"
#include "settings.h"

using namespace std;

void loadSettings(Settings &settings);
void loadComics(vector<Comic> &comics);

bool cmdcmp(const string x, const string y){
	return !strcmp(lower(x.c_str()), lower(y.c_str()));}

size_t findComic(const vector<Comic> comics, const string name)
{
	size_t found = false;
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		if(cmdcmp(name, comics.at(i).name))
		{
			found = i;
		}
	}
	if(!found)
	{
		cout << name << " is not a valid comic" << endl;
	}
	return found;
}

int main (int argc, char * const argv[])
{
	vector<Comic> comics;
	Settings settings;

	loadSettings(settings);
	loadComics(comics);

	if(argc>1)
	{
		if(cmdcmp(argv[1], "fetch"))
			if(argc>2)
				if(cmdcmp(argv[2], "all")) // fetch all
					for(unsigned int i = 0; i < comics.size(); i++)
						Spider(comics.at(i));
				else if(const size_t i = findComic(comics, argv[2])) // fetch comic
					Spider(comics.at(i));
				else
					cout << argv[2] << " is not a valid comic" << endl;
			else // fetch all
				for(unsigned int i = 0; i < comics.size(); i++)
					Spider(comics.at(i));
		else if(cmdcmp(argv[1], "list"))
			if(argc>2)
				if(cmdcmp(argv[2], "all")) // list all
					system(strcomb(3, "ls -m ", folder.c_str(), "/config | sed -e 's/\\.yaml//g' -e 's/, $//g' -e 's/, /\\\n/g'"));
				else if(cmdcmp(argv[2], "cur")) // list current
					for(unsigned int i = 0; i < comics.size(); i++)
						cout << comics.at(i).name << endl;
				else
					cout << argv[2] << " is not a valid command" << endl;
			else // list current
				for(unsigned int i = 0; i < comics.size(); i++)
					cout << comics.at(i).name << endl;
		else if(cmdcmp(argv[1], "add"))
		{
			if(argc>2)
			{
				std::fstream comics_file(comicsfile.c_str(), std::ios::out|std::ios::trunc); // add comic to comics list
				for(unsigned int i = 0; i < comics.size(); i++)
				{
					const string s = strcomb(3, "- ", comics.at(i).name.c_str(), "\n");
					comics_file.write(s.c_str(), s.size());
				}
				const string s = strcomb(3, "- ", argv[2], "\n");
				comics_file.write(s.c_str(), s.size());
				comics_file.close();
				loadComics(comics); // validate and cull comics
			}
			else
				cout << "You must specify a comic to add." << endl;
		}
		else if(cmdcmp(argv[1], "view"))
		{
			if(argc>2)
			{
				const int found = findComic(comics, argv[2]);

				if(argc>3) // view strips
				{
					const string filenum = string( // get number either from mark or from cli directly
							(cmdcmp(argv[3], "mark")) 
							? itoa(comics.at(found).mark) 
							: argv[3]).insert(0, 4-filenum.size(), '0');
					cout << filenum << endl; //DEBUG
					
					if(FileExists(strcomb(6, folder.c_str(), "/comics/",  argv[2], "/", filenum.c_str(), ".png"))) // read an image file if there's a local one
						system(strcomb(6, settings.viewer.c_str(), " ", folder.c_str(), "/comics/\"",  argv[2], "\"/*"));
					else // open the comic's url in a browser
						system(strcomb(3, settings.browser.c_str(), " ", comics.at(found).urls.at(atoi(filenum.c_str())).c_str()));
				}
				else
					cout << "You must specify what to view from this comic." << endl;
			}
			else
				cout << "You must specify a comic to view." << endl;
		}
		else if(cmdcmp(argv[1], "mark"))
		{
			if(argc>2)
			{
				const size_t found = findComic(comics, argv[2]);
				
				if(found && argc>3) // set mark to a url for the specified comic
				{
					HTTP page;

					get_http(page, argv[3]);
					const string imgs = get_img_urls(comics.at(found), page.mem);

					cout << imgs << endl; //DEBUG

					for(unsigned int i = 0; i < comics.at(found).imgs.size(); i++)
						if(!strcmp(imgs.c_str(), comics.at(found).imgs.at(i).c_str()))
						{
							cout << i << endl; //DEBUG
							comics.at(found).mark = i;
						}
				}
				else // tell the user the current value for mark for the specified comic
				{
					cout << comics.at(found).imgs.at(comics.at(found).mark) << endl;
					cout << comics.at(found).mark << endl;
				}
			}
			else
				cout << "You must specify a comic to place the bookmark in." << endl;
		}
		else if(cmdcmp(argv[1], "help"))
		{
			if(argc>2)
			{
				cout << "TODO!" << endl;
			}
			else
			{
				cout << settings.viewer << "\n" 
					<< "comics COMMAND [SUBCOMMAND]\n"
					<< "command tree is as follows:\n"
					<< "  fetch \t-- Downloads new images from all comics\n"
					<< "    COMIC \t-- Downloads new images from COMIC\n"
					<< "    all \t-- Downloads new images from all comics\n"
					<< "  list \t\t-- List the comics that are currently being followed\n"
					<< "    all \t-- Lists all possible comics\n"
					<< "  view\n"
					<< "    COMIC\n"
					<< "      STRIP# \t-- View strip number STRIP from COMIC\n"
					<< "  add\n"
					<< "    COMIC \t-- Start following COMIC\n"
					<< "  mark\n"
					<< "  help \t\t-- list this page\n"
					<< "\n"
					<< "Example: comics fetch all\n"
					<< endl;
			}
		}
		else
			cout << argv[1] << " is not a valid command." << endl;
	}

/*	// save imgs file
	for(unsigned i=0; i<comics.size(); i++)
	{
		saveImgFile(comics.at(i));
	}

	// save settings file
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		saveSettingsFile(comics.at(i));
	}
*/
	return 0;
}

void loadSettings(Settings &settings)
{
	// load settings file
	try 
	{
		ifstream fin(settingsfile.c_str());
		YAML::Parser parser(fin);

		YAML::Node doc;    // already parsed
		parser.GetNextDocument(doc);
		if(const YAML::Node *pName = doc.FindValue("viewer"))
			*pName >> settings.viewer;
		if(const YAML::Node *pName = doc.FindValue("browser"))
			*pName >> settings.browser;
	} 
	catch(YAML::ParserException& e)
	{
		cout << "config.yaml; " << e.what() << "\n";
	}

	// write the defaults to the settings file for ease of editing
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "viewer";
	out << YAML::Value << settings.viewer;
	out << YAML::Key << "browser";
	out << YAML::Value << settings.browser;
	out << YAML::EndMap;

	std::fstream fout(settingsfile.c_str(), std::ios::out|std::ios::trunc);
	fout.write(out.c_str(), out.size());
	fout.close();
}

void loadComics(vector<Comic> &comics)
{
	comics.clear();
	// mk folder if it doesn't exist
	if(chdir(folder.c_str()))
	{
		cout<< "mkdir -p " << folder << endl; //DEBUG
		system(strcomb(2, "mkdir -p ", folder.c_str()));
	}

	// load comics.yaml
	try 
	{
		ifstream fin(comicsfile.c_str());
		YAML::Parser parser(fin);

		YAML::Node doc;    // already parsed
		parser.GetNextDocument(doc);
		for(unsigned i=0;i<doc.size();i++) 
		{
			comics.push_back(Comic());
			doc[i] >> comics.at(i).name;
		}
	} 
	catch(YAML::ParserException& e)
	{
		cout << "comics.yaml; " << e.what() << "\n";mygetch();	
	}

	// load config files
	for(unsigned i=0; i<comics.size(); i++)
	{
		try 
		{
			ifstream fin(strcomb(4, folder.c_str(), "/config/", comics.at(i).name.c_str(), ".yaml"));
			YAML::Parser parser(fin);

			YAML::Node doc;    // already parsed
			parser.GetNextDocument(doc);
			if(const YAML::Node *pName = doc.FindValue("base_url")) // mandatory
				*pName >> comics.at(i).base_url;
			if(const YAML::Node *pName = doc.FindValue("first_url")) // mandatory
				*pName >> comics.at(i).first_url;
			else
				comics.at(i).base_url = "";
			if(const YAML::Node *pName = doc.FindValue("img_regex")) // mandatory
			{
				string img_regex;
				*pName >> img_regex;
				comics.at(i).img_regex = img_regex;
			}
			else
				comics.at(i).base_url = "";
			if(const YAML::Node *pName = doc.FindValue("next_regex")) // mandatory
			{
				string next_regex;
				*pName >> next_regex;
				comics.at(i).next_regex = next_regex;
			}
			else
				comics.at(i).base_url = "";
			if(const YAML::Node *pName = doc.FindValue("end_on_url"))
				*pName >> comics.at(i).end_on_url;
			if(const YAML::Node *pName = doc.FindValue("read_end_url"))
				*pName >> comics.at(i).read_end_url;
		} 
		catch(YAML::ParserException& e)
		{
			cout << comics.at(i).name.c_str() << ".yaml" << e.what() << "\n";
		}
	}

	// cull comics list of invalid comics
	for(unsigned int i = 0; i < comics.size(); i++)
		for(unsigned int ii = 0; ii < comics.size(); ii++)
			if(i==ii)
				continue;
			else if(comics.at(i).name == comics.at(ii).name)
				comics.at(ii).base_url = "";
	for(unsigned int i = 0; i < comics.size(); i++)
		if( comics.at(i).base_url.empty() )
		{
			cout << comics.at(i).name << "is an invalid comic." << endl;
			comics.erase(comics.begin()+i);
			i--; // make sure it doesn't skip
		}

	// save comics.yaml
	std::fstream comics_file(comicsfile.c_str(), std::ios::out|std::ios::trunc);
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		string s = strcomb(3, "- ", comics.at(i).name.c_str(), "\n");
		comics_file.write(s.c_str(), s.size());
	}	
	comics_file.close();

	// mk .comics/comics dir if it doesn't exist
	/*if(chdir(strcomb(2, folder.c_str(), "/comics")))
	{
		cout<< "mkdir " << folder << "/comics" << endl;
		system(strcomb(3, "mkdir ", folder.c_str(), "/comics"));
	}*/

	// mk comics/name folders if they don't exist yet
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		if(chdir(strcomb(3, folder.c_str(), "/comics/", comics.at(i).name.c_str())))
		{
			cout<< "mkdir -p " << folder << "/comics/" << comics.at(i).name << endl; //DEBUG
			string name = comics.at(i).name;
			system(strcomb(5, "mkdir -p \"", folder.c_str(), "/comics/", name.c_str(), "\""));
		}
	}

	// load settings, urls and imgs files
	for(unsigned i=0; i<comics.size(); i++)
	{
		try 
		{
			ifstream fin(strcomb(4, folder.c_str(), "/comics/", comics.at(i).name.c_str(), "/settings.yaml"));
			YAML::Parser parser(fin);

			YAML::Node doc;    // already parsed
			parser.GetNextDocument(doc);
			if(const YAML::Node *pName = doc.FindValue("last_url"))
				*pName >> comics.at(i).last_url;
			if(const YAML::Node *pName = doc.FindValue("download_imgs"))
				*pName >> comics.at(i).download_imgs;
			if(const YAML::Node *pName = doc.FindValue("mark"))
				*pName >> comics.at(i).mark;
		} 
		catch(YAML::ParserException& e)
		{
			cout << strcomb(4, folder.c_str(), "/comics/", comics.at(i).name.c_str(), "/settings.yaml") << e.what() << "\n";
		}

		try 
		{
			ifstream fin(strcomb(4, folder.c_str(), "/comics/", comics.at(i).name.c_str(), "/imgs.yaml"));
			YAML::Parser parser(fin);

			YAML::Node doc;    // already parsed
			parser.GetNextDocument(doc);
			for(unsigned ii=0;ii<doc.size();ii++) 
			{
				string url;
				doc[ii] >> url;
				comics.at(i).imgs.push_back(url);
			}
		} 
		catch(YAML::ParserException& e)
		{
			cout << strcomb(4, folder.c_str(), "/comics/", comics.at(i).name.c_str(), "/imgs.yaml") << e.what() << "\n";mygetch();	
		}
		comics.at(i).last_img = comics.at(i).imgs.size();

		try 
		{
			ifstream fin(strcomb(4, folder.c_str(), "/comics/", comics.at(i).name.c_str(), "/urls.yaml"));
			YAML::Parser parser(fin);

			YAML::Node doc;    // already parsed
			parser.GetNextDocument(doc);
			for(unsigned ii=0;ii<doc.size();ii++) 
			{
				string url;
				doc[ii] >> url;
				comics.at(i).urls.push_back(url);
			}
		} 
		catch(YAML::ParserException& e)
		{
			cout << strcomb(4, folder.c_str(), "/comics/", comics.at(i).name.c_str(), "/urls.yaml") << e.what() << "\n";mygetch();	
		}
	}

	// set last_url if it wasn't already
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		if(comics.at(i).last_url == "")
			comics.at(i).last_url = comics.at(i).first_url;
	}
}
