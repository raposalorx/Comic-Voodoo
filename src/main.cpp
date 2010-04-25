#include "yaml-cpp/yaml.h"
#include <fstream>
#include <iostream>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "conversion.h"

#include "voodoo.h"
#include "comic.h"
#include "http.h"
#include "comic_globals.h"

using namespace std;

void loadComics(vector<Comic> &comics);

int main (int argc, char * const argv[])
{
	vector<Comic> comics;
	
	loadComics(comics);
	
	if(argc>1)
	{
		if(!strcmp(lower(argv[1]), "fetch")) // fetch
		{
			if(argc>2)
			{
				if(!strcmp(lower(argv[2]), "all")) // fetch all
				{
					for(unsigned int i = 0; i < comics.size(); i++)
					{
						Spider(comics.at(i));
					}
				}
				else // fetch specific comic
				{
					bool found = false;
					for(unsigned int i = 0; i < comics.size(); i++)
					{
						if(!strcmp(lower(argv[2]), lower(comics.at(i).name.c_str())))
						{
							Spider(comics.at(i));
							found = true;
						}
					}
					if(!found)
					{
						cout << argv[2] << " is not a valid comic" << endl;
					}
				}
			}
			else // fetch all
			{
				for(unsigned int i = 0; i < comics.size(); i++)
				{
					Spider(comics.at(i));
				}
			}
		}
		else if(!strcmp(lower(argv[1]), "list"))
		{	
			if(argc>2)
			{
				if(!strcmp(lower(argv[2]), "all"))
				{
					system(strcomb(3, "ls -m ", folder.c_str(), "/config | sed -e 's/\\.yaml//g' | sed -e 's/, $//g' | sed -e 's/, /\\\n/g'"));
				}
				else
				{
					cout << argv[2] << " is not a valid command" << endl;
				}
			}
			else
			{
				for(unsigned int i = 0; i < comics.size(); i++)
				{
					cout << comics.at(i).name << endl;
				}
			}
		}
		else if(!strcmp(lower(argv[1]), "add"))
		{
			if(argc>2)
			{
				// save comics.yaml
				std::fstream comics_file(comicsfile.c_str(), std::ios::out|std::ios::trunc);
				for(unsigned int i = 0; i < comics.size(); i++)
				{
					string s = strcomb(3, "- ", comics.at(i).name.c_str(), "\n");
					comics_file.write(s.c_str(), s.size());
				}
				string s = strcomb(3, "- ", argv[2], "\n");
				comics_file.write(s.c_str(), s.size());
				comics_file.close();
				loadComics(comics);
			}
			else
			{
				cout << "You must specify a comic to add." << endl;
			}
		}
		else if(!strcmp(lower(argv[1]), "help"))
		{
			if(argc>2)
			{
				cout << "TODO!" << endl;
			}
			else
			{
				cout 
					<< "comics COMMAND [SUBCOMMAND]\n"
					<< "command tree is as follows:\n"
					<< "  fetch \t-- Downloads new images from all comics\n"
					<< "    COMIC \t-- Downloads new images from COMIC\n"
					<< "    all \t-- Downloads new images from all comics\n"
					<< "  list \t\t-- List the comics that are currently being followed\n"
					<< "    all \t-- Lists all possible comics\n"
					<< "  add\n"
					<< "    COMIC \t-- Start following COMIC\n"
					<< "  help \t\t-- list this page\n"
					<< "\n"
					<< "Example: comics fetch all\n"
					<< endl;
			}
		}
		else
		{
			cout << argv[1] << " is not a valid command" << endl;
		}
	}
	
	// save imgs file
	for(unsigned i=0; i<comics.size(); i++)
	{
		saveImgFile(comics.at(i));
	}
	
	// save settings file
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		saveSettingsFile(comics.at(i));
	}
	
	return 0;
}

void loadComics(vector<Comic> &comics)
{
	comics.clear();
	// mk folder if it doesn't exist
	if(chdir(folder.c_str()))
	{
		cout<< "mkdir " << folder << endl;
		system(strcomb(2, "mkdir ", folder.c_str()));
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
			// cout << "loading: " << comics.at(i).name << endl;

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
			if(const YAML::Node *pName = doc.FindValue("base_url"))
				*pName >> comics.at(i).base_url;
			if(const YAML::Node *pName = doc.FindValue("first_url"))
				*pName >> comics.at(i).first_url;
			if(const YAML::Node *pName = doc.FindValue("img_regex"))
			{
				string img_regex;
				*pName >> img_regex;
				comics.at(i).img_regex = img_regex;
			}
			if(const YAML::Node *pName = doc.FindValue("next_regex"))
			{
				string next_regex;
				*pName >> next_regex;
				comics.at(i).next_regex = next_regex;
			}
			// if(const YAML::Node *pName = doc.FindValue("redirect_regex"))
			// {
			// 	string redirect_regex;
			// 	*pName >> redirect_regex;
			// 	comics.at(i).redirect_regex = redirect_regex;
			// }
			if(const YAML::Node *pName = doc.FindValue("end_on_url"))
				*pName >> comics.at(i).end_on_url;
			if(const YAML::Node *pName = doc.FindValue("read_end_url"))
				*pName >> comics.at(i).read_end_url;
			if(const YAML::Node *pName = doc.FindValue("download_imgs"))
				*pName >> comics.at(i).download_imgs;
		} 
		catch(YAML::ParserException& e)
		{
			cout << comics.at(i).name.c_str() << ".yaml" << e.what() << "\n";
		}
	}
	
	// cull comics list of invalid comics
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		if( comics.at(i).base_url.empty() )
		{
			cout << comics.at(i).name << "is an invalid comic." << endl;
			comics.erase(comics.begin()+i);
		}
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
	if(chdir(strcomb(2, folder.c_str(), "/comics")))
	{
		cout<< "mkdir " << folder << "/comics" << endl;
		system(strcomb(3, "mkdir ", folder.c_str(), "/comics"));
	}
	
	// mk comics/name folders if they don't exist yet
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		if(chdir(strcomb(3, folder.c_str(), "/comics/", comics.at(i).name.c_str())))
		{
			cout<< "mkdir " << folder << "/comics/" << comics.at(i).name << endl;
			string name = comics.at(i).name;
			while(name.find(' ') != string::npos && name.at(name.find(' ')-1) != '\\')
				name.insert(name.find(' '), 1, '\\');
			system(strcomb(4, "mkdir ", folder.c_str(), "/comics/", name.c_str()));
		}
	}
	
	// load settings and imgs files
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
	}
	
	// set last_url if it wasn't already
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		if(comics.at(i).last_url == "")
			comics.at(i).last_url = comics.at(i).first_url;
	}
}
