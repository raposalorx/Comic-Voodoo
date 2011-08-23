#include "yaml-cpp/yaml.h"
#include <fstream>
#include <iostream>
#include <curl/curl.h>
#include <curl/types.h>
#include <curl/easy.h>
#include "conversion.h"
#include <pcrecpp.h>
#include <argtable2.h>

#include "comic.h"
#include "http.h"
#include "comic_globals.h"
#include "settings.h"

using namespace std;

void loadSettings(Settings &settings);
void loadComics(vector<Comic> &comics);

bool cmdcmp(const string x, const string y){
	return !strcmp(lower(x.c_str()), lower(y.c_str()));}

int findComic(const vector<Comic> comics, const string name)
{
	int found = -1;
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		if(cmdcmp(name, comics.at(i).name))
		{
			found = i;
		}
	}
	if(found < 0)
	{
		cout << name << " is not a valid comic" << endl;
	}
	return found;
}

int main (int argc, char ** const argv)
{
//	vector<Comic> comics;
//	Settings settings;

//	loadSettings(settings);
//	loadComics(comics);

	/* Define the allowable command line options, collecting them in argtable[] */
//    struct arg_lit *n     = arg_lit0("n", NULL,         "do not output the trailing newline");
    struct arg_str *help  = arg_strn(NULL,NULL,"help",1,1,"print this help.");
    struct arg_end *endhelp   = arg_end(20);
	void* argsHelp[] = {help, endhelp};
	int helpErrors;

    struct arg_rex *add  = arg_rex1(NULL,NULL,"add",NULL,0,NULL);
	struct arg_str *add_comic = arg_str1(NULL,NULL,"\"comic\"", "Comic to add");
    struct arg_end *endadd   = arg_end(20);
	void* argsAdd[] = {add, add_comic, endadd};
    int addErrors;

	struct arg_rex *rm  = arg_rex1(NULL,NULL,"rm",NULL,0,NULL);
    struct arg_end *endrm   = arg_end(20);
	void* argsRm[] = {rm, endrm};
    int rmErrors;

	struct arg_rex *list  = arg_rex1(NULL,NULL,"list",NULL,0,NULL);
	struct arg_lit *list_c     = arg_lit0("c", NULL, "List currently watched comics.");
    struct arg_end *endlist   = arg_end(20);
	void* argsList[] = {list, list_c, endlist};
    int listErrors;

	struct arg_rex *fetch  = arg_rex1(NULL,NULL,"fetch",NULL,0,NULL);
	struct arg_lit *fetch_r     = arg_lit0("r", NULL,         "Re-fetch an entire comic, starting from the beginning.");
	struct arg_lit *fetch_m     = arg_lit0("m", "mark", "Mark each fetched comic with the earliest newly fetched stip");
	struct arg_str *fetch_comic = arg_str1(NULL,NULL,"[comic]", "Comic to add");
    struct arg_end *endfetch   = arg_end(20);
	void* argsFetch[] = {fetch, fetch_r, fetch_m, fetch_comic, endfetch};
    int fetchErrors;

	struct arg_rex *mark  = arg_rex1(NULL,NULL,"mark",NULL,0,NULL);
	struct arg_lit *mark_f     = arg_lit0("f", NULL, "First");
	struct arg_lit *mark_e     = arg_lit0("e", NULL, "End");
	struct arg_lit *mark_l     = arg_lit0("l", NULL, "List");
	struct arg_str *mark_comic = arg_str1(NULL,NULL,"[comic]", "Comic");
	struct arg_int *mark_n = arg_intn("n", "num", NULL, 0, 1, "Number of the strip");
    struct arg_end *endmark   = arg_end(20);
	void* argsMark[] = {mark, mark_f, mark_e, mark_l, mark_comic, mark_n, endmark};
    int markErrors;

	struct arg_rex *view  = arg_rex1(NULL,NULL,"view",NULL,0,NULL);
	struct arg_lit *view_m = arg_lit0("m", NULL, "View comic at the mark.");
	struct arg_int *view_n = arg_intn("n", "num", NULL, 0, 1, "Number of the strip");
	struct arg_str *view_comic = arg_str1(NULL,NULL,"\"comic\"", "Comic to view");
    struct arg_end *endview   = arg_end(20);
	void* argsView[] = {view, view_m, view_n, view_comic, endview};
    int viewErrors;

	struct arg_rex *set  = arg_rex1(NULL,NULL,"set",NULL,0,NULL);
    struct arg_end *endset   = arg_end(20);
	void* argsSet[] = {set, endset};
    int setErrors;

	struct arg_rex *update  = arg_rex1(NULL,NULL,"update",NULL,0,NULL);
    struct arg_end *endupdate   = arg_end(20);
	void* argsUpdate[] = {update, endupdate};
    int updateErrors;

    /* verify the argtable[] entries were allocated sucessfully */
/*    if (arg_nullcheck(argtable) != 0)
        {
        * NULL entries were detected, some allocations must have failed *
        printf("%s: insufficient memory\n",progname);
        }
*/
    /* Parse the command line as defined by argtable[] */
    helpErrors = arg_parse(argc,argv,argsHelp);
    addErrors = arg_parse(argc,argv,argsAdd);
    rmErrors = arg_parse(argc,argv,argsRm);
    listErrors = arg_parse(argc,argv,argsList);
    fetchErrors = arg_parse(argc,argv,argsFetch);
    markErrors = arg_parse(argc,argv,argsMark);
    viewErrors = arg_parse(argc,argv,argsView);
    setErrors = arg_parse(argc,argv,argsSet);
    updateErrors = arg_parse(argc,argv,argsUpdate);

	if (addErrors == 0)
	{
/*		if(add_comic->count == 1)
		{
				std::fstream comics_file(comicsfile.c_str(), std::ios::out|std::ios::trunc); // add comic to comics list
				for(unsigned int i = 0; i < comics.size(); i++)
				{
					const string s = strcomb(3, "- ", comics.at(i).name.c_str(), "\n");
					comics_file.write(s.c_str(), s.size());
				}
				const string s = strcomb(3, "- ", add_comic, "\n");
				comics_file.write(s.c_str(), s.size());
				comics_file.close();
				loadComics(comics); // validate and cull comics
		}
		else
			cout << "You must specify one comic to add." << endl;
*/
	}
	else if (rmErrors == 0)
	{
		cout << "rm";
	}
	else if (listErrors == 0)
	{
		cout << "list";
	}
	else if (fetchErrors == 0)
	{
		cout << "fetch";
		if (fetch_r->count >0)
		{
			cout << " -r";
		}
	}
	else if (markErrors == 0)
	{
		cout << "mark";
	}
	else if (viewErrors == 0)
	{
		cout << "view";
	}
	else if (setErrors == 0)
	{
		cout << "set";
	}
	else if (updateErrors == 0)
	{
		cout << "update";
	}

    /* special case: '--help' takes precedence over error reporting */
	else if (help->count > 0)
        {
//        printf("Usage: %s", progname);
		cout << "Usage:" << endl;
		arg_print_syntax(stdout,argsAdd,"\n");
        arg_print_syntax(stdout,argsRm,"\n");
        arg_print_syntax(stdout,argsList,"\n");
        arg_print_syntax(stdout,argsFetch,"\n");
        arg_print_syntax(stdout,argsMark,"\n");
        arg_print_syntax(stdout,argsView,"\n");
        arg_print_syntax(stdout,argsSet,"\n");
        arg_print_syntax(stdout,argsUpdate,"\n");
        arg_print_syntax(stdout,argsHelp,"\n");
//        printf("Echo the STRINGs to standard output.\n\n");
		cout << "Descriptions:" << endl;
        arg_print_glossary(stdout,argsAdd,"  %-10s %s\n");
        arg_print_glossary(stdout,argsRm,"  %-10s %s\n");
        arg_print_glossary(stdout,argsList,"  %-10s %s\n");
        arg_print_glossary(stdout,argsFetch,"  %-10s %s\n");
        arg_print_glossary(stdout,argsMark,"  %-10s %s\n");
        arg_print_glossary(stdout,argsView,"  %-10s %s\n");
        arg_print_glossary(stdout,argsSet,"  %-10s %s\n");
        arg_print_glossary(stdout,argsUpdate,"  %-10s %s\n");
        arg_print_glossary(stdout,argsHelp,"  %-10s %s\n");
        }

    /* special case: '--version' takes precedence error reporting */
/*	else if (vers->count > 0)
        {
        printf("'%s' example program for the \"argtable\" command line argument parser.\n",progname);
        printf("September 2003, Stewart Heitmann\n");
        }
*/
    /* If the parser returned any errors then display them and exit */
//	else if (nerrors > 0)
  //      {
        /* Display the error details contained in the arg_end struct.*/
/*        arg_print_errors(stdout,end,progname);
        printf("Try '%s --help' for more information.\n",progname);
        }
*/
    /* deallocate each non-null entry in argtable[] */
    arg_freetable(argsAdd,sizeof(argsAdd)/sizeof(argsAdd[0]));
    arg_freetable(argsRm,sizeof(argsRm)/sizeof(argsRm[0]));
    arg_freetable(argsList,sizeof(argsList)/sizeof(argsList[0]));
    arg_freetable(argsFetch,sizeof(argsFetch)/sizeof(argsFetch[0]));
    arg_freetable(argsMark,sizeof(argsMark)/sizeof(argsMark[0]));
    arg_freetable(argsView,sizeof(argsView)/sizeof(argsView[0]));
    arg_freetable(argsSet,sizeof(argsSet)/sizeof(argsSet[0]));
    arg_freetable(argsUpdate,sizeof(argsUpdate)/sizeof(argsUpdate[0]));
    arg_freetable(argsHelp,sizeof(argsHelp)/sizeof(argsHelp[0]));

/*	if(argc>1)
	{
		if(cmdcmp(argv[1], "fetch"))
			if(argc>2)
				if(cmdcmp(argv[2], "all")) // fetch all
					for(unsigned int i = 0; i < comics.size(); i++)
						comics.at(i).Spider();
				else if(const size_t i = findComic(comics, argv[2])) // fetch comic
					comics.at(i).Spider();
				else
					cout << argv[2] << " is not a valid comic" << endl;
			else // fetch all
				for(unsigned int i = 0; i < comics.size(); i++)
					comics.at(i).Spider();
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
				if(found >=0)
				{
					if(argc>3) // view strips
					{
						string filenum = (cmdcmp(argv[3], "mark")) // get number either from mark or from cli directly
							? itoa(comics.at(found).mark) 
							: argv[3];
						filenum.insert(0, 4-filenum.size(), '0');
						cout << filenum << endl; //DEBUG

						if(FileExists(strcomb(6, folder.c_str(), "/comics/",  argv[2], "/", filenum.c_str(), ".png"))) // read an image file if there's a local one
							system(strcomb(6, settings.viewer.c_str(), " ", folder.c_str(), "/comics/\"",  argv[2], "\"/*"));
						else // open the comic's url in a browser
							system(strcomb(3, settings.browser.c_str(), " ", comics.at(found).urls.at(atoi(filenum.c_str())).c_str()));
					}
					else
						cout << "You must specify what to view from this comic." << endl;
				}
			}
			else
				cout << "You must specify a comic to view." << endl;
		}
		else if(cmdcmp(argv[1], "mark"))
		{
			if(argc>2)
			{
				const int found = findComic(comics, argv[2]);
				if(found >=0)
				{
					if(argc>3)
					{
						if(cmdcmp(argv[3],"new"))
						{
							cout << "new" << endl;
							comics.at(found).mark = comics.at(found).imgs.size()-1;
						}
						else if(cmdcmp(argv[3],"first"))
						{
							cout << "first" << endl;
							comics.at(found).mark = 0;
						}
						else
						{
							HTTP page;

							get_http(page, argv[3]);
							const string imgs = comics.at(found).get_img_urls(page.mem);

							cout << imgs << endl; //DEBUG

							for(unsigned int i = 0; i < comics.at(found).imgs.size(); i++)
								if(!strcmp(imgs.c_str(), comics.at(found).imgs.at(i).c_str()))
								{
									cout << i << endl; //DEBUG
									comics.at(found).mark = i;
								}
						}
					}
					else // tell the user the current value for mark for the specified comic
					{
						cout << comics.at(found).imgs.at(comics.at(found).mark) << endl;
						cout << comics.at(found).mark << endl;
					}
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
*/
/*	// save imgs file
	for(unsigned i=0; i<comics.size(); i++)
	{
		saveImgFile(comics.at(i));
	}
*/
	// save settings file
/*	for(unsigned int i = 0; i < comics.size(); i++)
	{
		comics.at(i).saveSettingsFile();
	}*/

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

void initComics(vector<Comic> &comics)
{
	comics.clear();
	// mk folder if it doesn't exist
	if(chdir(folder.c_str()))
	{
		cout<< "mkdir -p " << folder << endl; //DEBUG
		char* mkdirp = strcomb(2, "mkdir -p ", folder.c_str());
		system(mkdirp);
		free(mkdirp);
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
}

void loadComics(vector<Comic> &comics)
{
	comics.clear();
	// mk folder if it doesn't exist
	if(chdir(folder.c_str()))
	{
		cout<< "mkdir -p " << folder << endl; //DEBUG
		char* mkdirp = strcomb(2, "mkdir -p ", folder.c_str());
		system(mkdirp);
		free(mkdirp);
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
			char* configFile = strcomb(4, folder.c_str(), "/config/", comics.at(i).name.c_str(), ".yaml");
			ifstream fin(configFile);
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
			free(configFile);
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
		char* s = strcomb(3, "- ", comics.at(i).name.c_str(), "\n");
		comics_file.write(s, strlen(s));
		free(s);
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
		char* comicFolder = strcomb(3, folder.c_str(), "/comics/", comics.at(i).name.c_str());
		if(chdir(comicFolder))
		{
			cout<< "mkdir -p " << folder << "/comics/" << comics.at(i).name << endl; //DEBUG
			char* mkdirComicFolder = strcomb(5, "mkdir -p \"", comicFolder, "\"");
			system(mkdirComicFolder);
			free(mkdirComicFolder);
		}
		free(comicFolder);
	}

	// load settings, urls and imgs files
	for(unsigned i=0; i<comics.size(); i++)
	{
		char* settingsFile = strcomb(4, folder.c_str(), "/comics/", comics.at(i).name.c_str(), "/settings.yaml");
		try 
		{
			ifstream fin(settingsFile);
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
			cout << settingsFile << e.what() << "\n";
		}
		free(settingsFile);
		char* imgsFile = strcomb(4, folder.c_str(), "/comics/", comics.at(i).name.c_str(), "/imgs.yaml");
		try 
		{
			ifstream fin(imgsFile);
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
			cout << imgsFile << e.what() << "\n";mygetch();	
		}
		comics.at(i).last_img = comics.at(i).imgs.size();
		free(imgsFile);
		char* urlsFile = strcomb(4, folder.c_str(), "/comics/", comics.at(i).name.c_str(), "/urls.yaml");
		try 
		{
			ifstream fin(urlsFile);
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
			cout << urlsFile  << e.what() << "\n";mygetch();	
		}
		free(urlsFile);
	}
	// set last_url if it wasn't already
	for(unsigned int i = 0; i < comics.size(); i++)
	{
		if(comics.at(i).last_url == "")
			comics.at(i).last_url = comics.at(i).first_url;
	}
}
