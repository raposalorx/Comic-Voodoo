#include <iostream>
#include <argtable2.h>
#include <iomanip>

#include "files.h"
#include "spider.h"
#include "comhelp.h"

int main(int argc, char** argv)
{
  using std::cout;
  using std::endl;
  using std::setw;
  
  std::string env_home = getenv("HOME");
  std::string voodoo_home = env_home + "/.voodoo";
  std::string picture_dir = voodoo_home + "/pics";

  Cache* cache = new Cache(voodoo_home, "cache");

  try
  {
    createFolders(voodoo_home);
    createCache(cache);
  }
  catch(files::E_CreationFailed e)
  {
    cout << e.what() << endl;
    return 1;
  }

  struct arg_rex *help  = arg_rex1(NULL,NULL,"^help",NULL,0,NULL);
  struct arg_end *endhelp   = arg_end(20);
  void* argsHelp[] = {help, endhelp};
  int helpErrors;
  helpErrors = arg_parse(argc,argv,argsHelp);

  struct arg_rex *import  = arg_rex1(NULL,NULL,"^import",NULL,0,NULL);
  struct arg_file *import_comics = arg_filen(NULL,NULL,"FILE", 1, 64, "Yaml file to import.");
  struct arg_end *endimport   = arg_end(20);
  void* argsImport[] = {import, import_comics, endimport};
  int importErrors;
  importErrors = arg_parse(argc,argv,argsImport);

  struct arg_rex *xport  = arg_rex1(NULL,NULL,"^export",NULL,0,NULL);
  struct arg_str *xport_comics = arg_strn(NULL,NULL,"COMIC", 1, 64, "Exports COMIC to COMIC.yaml");
  struct arg_end *endxport   = arg_end(20);
  void* argsXport[] = {xport, xport_comics, endxport};
  int xportErrors;
  xportErrors = arg_parse(argc,argv,argsXport);

  struct arg_rex *list  = arg_rex1(NULL,NULL,"^list",NULL,0,NULL);
  struct arg_lit *list_watched = arg_lit0("w","watched","Only search watched comics.");
  struct arg_str *list_comic = arg_str0(NULL,NULL,"QUERY", "Search for comics starting with QUERY.");
  struct arg_end *endlist   = arg_end(20);
  void* argsList[] = {list, list_watched, list_comic, endlist};
  int listErrors;
  listErrors = arg_parse(argc,argv,argsList);

  struct arg_rex *watch  = arg_rex1(NULL,NULL,"^watch",NULL,0,NULL);
  struct arg_str *watch_comic = arg_str0(NULL,NULL,"COMIC", "Start following COMIC.");
  struct arg_end *endwatch   = arg_end(20);
  void* argsWatch[] = {watch, watch_comic, endwatch};
  int watchErrors;
  watchErrors = arg_parse(argc,argv,argsWatch);

  struct arg_rex *unwatch  = arg_rex1(NULL,NULL,"^unwatch",NULL,0,NULL);
  struct arg_str *unwatch_comic = arg_str0(NULL,NULL,"COMIC", "Stop following COMIC.");
  struct arg_end *endunwatch   = arg_end(20);
  void* argsUnwatch[] = {unwatch, unwatch_comic, endunwatch};
  int unwatchErrors;
  unwatchErrors = arg_parse(argc,argv,argsUnwatch);

  struct arg_rex *fetch  = arg_rex1(NULL,NULL,"^fetch",NULL,0,NULL);
  struct arg_str *fetch_comics = arg_strn(NULL,NULL,"COMIC", 0, 64, "Fetch the watched comic COMIC.");
  struct arg_lit *fetch_all = arg_lit0("a","all","Fetch all watched comics.");
  struct arg_end *endfetch   = arg_end(20);
  void* argsFetch[] = {fetch, fetch_comics, fetch_all, endfetch};
  int fetchErrors;
  fetchErrors = arg_parse(argc,argv,argsFetch);

  struct arg_rex *set  = arg_rex1(NULL,NULL,"^set",NULL,0,NULL);
  struct arg_str *set_option = arg_str1(NULL,NULL,"OPTION", "Option to set.");
  struct arg_str *set_value = arg_str0(NULL,NULL,"VALUE", "Option's new value.");
  struct arg_end *endset   = arg_end(20);
  void* argsSet[] = {set, set_option, set_value, endset};
  int setErrors;
  setErrors = arg_parse(argc,argv,argsSet);

  struct arg_rex *mark  = arg_rex1(NULL,NULL,"^mark",NULL,0,NULL);
  struct arg_str *mark_comic = arg_str1(NULL,NULL,"COMIC", "Comic to bookmark.");
  struct arg_lit *mark_newest = arg_lit0("n","newest","Mark the newest strip.");
  struct arg_str *mark_query = arg_str0(NULL,NULL,"QUERY", "URL or strip number/date to mark.");
  struct arg_end *endmark   = arg_end(20);
  void* argsMark[] = {mark, mark_comic, mark_newest, mark_query, endmark};
  int markErrors;
  markErrors = arg_parse(argc,argv,argsMark);

  struct arg_rex *read  = arg_rex1(NULL,NULL,"^read",NULL,0,NULL);
  struct arg_str *read_comic = arg_str1(NULL,NULL,"COMIC", "Comic to read.");
  struct arg_lit *read_newest = arg_lit0("n","newest","Read the newest strip.");
  struct arg_lit *read_mark = arg_lit0("m","mark","Read the bookmarked strip.");
  struct arg_str *read_query = arg_str0(NULL,NULL,"QUERY", "URL or strip number/date to read.");
  struct arg_end *endread   = arg_end(20);
  void* argsRead[] = {read, read_comic, read_newest, read_mark, read_query, endread};
  int readErrors;
  readErrors = arg_parse(argc,argv,argsRead);


  if (help->count > 0 && helpErrors == 0)
  {
    cout << "comic [command]\n" << endl;
    cout << "Commands:" << endl;
    arg_print_syntax(stdout,argsImport,"\n");
    arg_print_syntax(stdout,argsXport,"\n");
    arg_print_syntax(stdout,argsList,"\n");
    arg_print_syntax(stdout,argsWatch,"\n");
    arg_print_syntax(stdout,argsUnwatch,"\n");
    arg_print_syntax(stdout,argsFetch,"\n");
    arg_print_syntax(stdout,argsSet,"\n");
    arg_print_syntax(stdout,argsMark,"\n");
    arg_print_syntax(stdout,argsRead,"\n");
    arg_print_syntax(stdout,argsHelp,"\n");

    cout << "\nDescriptions:" << endl;
    cout << "  import" << setw(13) << "" << "Import a comic from a yaml file.\n";
    arg_print_glossary(stdout,argsImport,"    %-16s %s\n");
    cout << "  export" << setw(13) << "" << "Export a comic to a yaml file.\n";
    arg_print_glossary(stdout,argsXport,"    %-16s %s\n");
    cout << "  list" << setw(15) << "" << "Search the comic database." << "\n";
    arg_print_glossary(stdout,argsList,"    %-16s %s\n");
    cout << "  watch" << setw(14) << "" << "Start following a comic." << "\n";
    arg_print_glossary(stdout,argsWatch,"    %-16s %s\n");
    cout << "  unwatch" << setw(12) << "" << "Stop following a comic." << "\n";
    arg_print_glossary(stdout,argsUnwatch,"    %-16s %s\n");
    cout << "  fetch" << setw(14) << "" << "Fetch watched comics." << "\n";
    arg_print_glossary(stdout,argsFetch,"    %-16s %s\n");
    cout << "  set" << setw(16) << "" << "Set global options." << "\n";
    arg_print_glossary(stdout,argsSet,"    %-16s %s\n");
    cout << "  mark" << setw(15) << "" << "Bookmark a strip." << "\n";
    arg_print_glossary(stdout,argsMark,"    %-16s %s\n");
    cout << "  read" << setw(15) << "" << "Read a strip in your favorite browser." << "\n";
    arg_print_glossary(stdout,argsRead,"    %-16s %s\n");
    cout << "  help" << setw(15) << "" << "Show this help dialog." << "\n";
    arg_print_glossary(stdout,argsHelp,"    %-16s %s\n");
    cout << endl;
  }
  else if (import->count == 1 && importErrors == 0)
  {
    return importYaml(cache, import_comics);
  }
  else if (xport->count == 1 && xportErrors == 0)
  {
    return exportYaml(cache, xport_comics);
  }
  else if (list->count == 1 && listErrors == 0)
  {
    std::vector<Comic*>* comics;
    if(list_watched->count == 1)
    {
      comics = cache->searchComics(list_comic->sval[0], 1);
    }
    else
    {
      comics = cache->searchComics(list_comic->sval[0], 0);
    }
    for(unsigned int i = 0; i < comics->size(); i++)
    {
      cout << "\"" << comics->at(i)->name << "\"" << endl;
    }
  }
  else if (watch->count == 1 && watchErrors == 0)
  {
    Comic* comic = comicLookup(cache, watch_comic->sval[0], 0);
    if(comic != NULL)
    {
      if(comic->watched)
      {
        cout << "\"" << comic->name << "\" is already being watched." << endl;
      }
      else
      {
        comic->watched = true;
        cache->updateComicConfig(*comic);
        cout << "Watching " << comic->name << endl;
      }
    }
  }
  else if (unwatch->count == 1 && unwatchErrors == 0)
  {
    Comic* comic = comicLookup(cache, unwatch_comic->sval[0], 0);
    if(comic != NULL)
    {
      if(!comic->watched)
      {
        cout << "\"" << comic->name << "\" isn't watched." << endl;
      }
      else
      {
        comic->watched = false;
        cache->updateComicConfig(*comic);
        cout << "No longer watching " << comic->name << endl;
      }
    }
  }
  else if (fetchErrors == 0)
  {
    if(fetch_all->count == 1)
    {
      std::vector<Comic*>* comics = cache->searchComics("", 1);
      for(unsigned int i = 0; i < comics->size(); i++)
      {
        cout << "Fetching " << comics->at(i)->name << "..." << endl;
        Spider spider(picture_dir, *(comics->at(i)), cache);
        std::vector<Strip*>* fetched_strips = spider.fetchAllStrips();
        saveStrips(cache, fetched_strips);
        if(fetched_strips->size()==0)
          cout << "Nothing new for " << comics->at(i)->name << endl;
        else
          cout << "Fetched " << fetched_strips->size() << " new strips of " << comics->at(i)->name << endl;
      }
    }
    else
    {
      if(fetch_comics->count > 0)
      {
        std::vector<Comic*> comics;
        int errors = 0;
        for(unsigned int i = 0; i < fetch_comics->count; i++)
        {
          Comic* comic = comicLookup(cache, fetch_comics->sval[i], 1);
          if(comic != NULL)
            comics.push_back(comic);
          else
            errors++;
        }
        if(errors)
          return 1;
        for(unsigned int i = 0; i < comics.size(); i++)
        {
          cout << "Fetching " << comics.at(i)->name << "..." << endl;
          Spider spider(picture_dir, *(comics.at(i)), cache);
          std::vector<Strip*>* fetched_strips = spider.fetchAllStrips();
          saveStrips(cache, fetched_strips);
          if(fetched_strips->size()==0)
            cout << "Nothing new for " << comics.at(i)->name << endl;
          else
            cout << "Fetched " << fetched_strips->size() << " strips of " << comics.at(i)->name << endl;
        }
      }
      else
      {
        cout << "You must specify at least one comic to fetch, or use --all." << endl;
      }
    }
  }
  else if (set->count == 1 && setErrors == 0)
  {
    if(set_value->count == 1)
    {
      cache->setOption(set_option->sval[0], set_value->sval[0]);
      cout << set_option->sval[0] << " = " << set_value->sval[0] << endl;
    }
    else
    {
      std::string value = cache->getOption(set_option->sval[0]);
      if(value=="")
      {
        cout << set_option->sval[0] << " is unset." << endl;
      }
      else
      {
        cout << set_option->sval[0] << " = " << value << endl;
      }
    }
  }
  else if (mark->count == 1 && markErrors == 0)
  {
    Comic* comic = comicLookup(cache, mark_comic->sval[0], 1);
    if(comic!=NULL)
    {
      if(mark_newest->count == 1)
      {
        int newest_id = comic->current_id;
        if(comic->read_end_url)
          newest_id++;
        comic->mark = newest_id;
        cache->updateComicConfig(*comic);
        Strip* strip = cache->getStrip(newest_id, comic->name);
        cout << "Set " << comic->name << "'s mark to " << strip->page << endl;
      }
      else if(mark_query->count == 1)
      {
        std::vector<Strip*>* strips = cache->searchStrips(*comic,mark_query->sval[0]);
        if(strips->size()==0)
        {
          cout << "No strip was found with that query" << endl;
        }
        else if(strips->size()>1)
        {
          cout << "Ambiguous query, please search so that only one remains:\n";
          for(signed int i = 0; i<strips->size();i++)
          {
            cout << strips->at(i)->page << "\n";
          }
          cout << endl;
        }
        else
        {
          comic->mark = strips->at(0)->id;
          cache->updateComicConfig(*comic);
          cout << "Set " << comic->name << "'s mark to " << strips->at(0)->page << endl;
        }
      }
      else
      {
        if(comic->mark == -1)
        {
          cout << comic->name << "'s mark is unset." << endl;
        }
        else
        {
          Strip* strip = cache->getStrip(comic->mark, comic->name);
          cout << comic->name << " is marked at " << strip->page << endl;
        }
      }
    }
  }
  else if (read->count == 1 && readErrors == 0)
  {
    Comic* comic = comicLookup(cache, read_comic->sval[0], 1);
    std::string browser = cache->getOption("browser");
    Strip* strip = NULL;
    if(browser == "")
    {
      cout << "Please set a web browser to use\nExample: voodoo set browser firefox" << endl;
    }
    if(comic!=NULL && browser != "")
    {
      if(read_mark->count == 1)
      {
        strip = cache->getStrip(comic->mark, comic->name);
      }
      else if(read_newest->count == 1)
      {
        int newest_id = comic->current_id;
        if(comic->read_end_url)
          newest_id++;
        strip = cache->getStrip(newest_id, comic->name);
      }
      else if(read_query->count == 1)
      {
        std::vector<Strip*>* strips = cache->searchStrips(*comic,read_query->sval[0]);
        if(strips->size()==0)
        {
          cout << "No strip was found with that query" << endl;
        }
        else if(strips->size()>1)
        {
          cout << "Ambiguous query, please search so that only one remains:\n";
          for(signed int i = 0; i<strips->size();i++)
          {
            cout << strips->at(i)->page << "\n";
          }
          cout << endl;
        }
        else
        {
          strip = strips->at(0);
        }
      }
      else
      {
        strip = cache->getStrip(comic->mark, comic->name);
      }
      if(strip != NULL)
      {
        system(std::string(browser + " " + strip->page).c_str());
      }
    }
  }

  /* special case: '--version' takes precedence error reporting */
  /*	else if (vers->count > 0)
      {
      printf("'%s' example program for the \"argtable\" command line argument parser.\n",progname);
      printf("September 2003, Stewart Heitmann\n");
      }
      */

  arg_freetable(argsImport,sizeof(argsImport)/sizeof(argsImport[0]));
  arg_freetable(argsXport,sizeof(argsXport)/sizeof(argsXport[0]));
  arg_freetable(argsList,sizeof(argsList)/sizeof(argsList[0]));
  arg_freetable(argsHelp,sizeof(argsHelp)/sizeof(argsHelp[0]));
  arg_freetable(argsWatch,sizeof(argsWatch)/sizeof(argsWatch[0]));
  arg_freetable(argsUnwatch,sizeof(argsUnwatch)/sizeof(argsUnwatch[0]));
  arg_freetable(argsSet,sizeof(argsSet)/sizeof(argsSet[0]));
  arg_freetable(argsMark,sizeof(argsMark)/sizeof(argsMark[0]));
  arg_freetable(argsRead,sizeof(argsRead)/sizeof(argsRead[0]));

  delete cache;

  return 0;
}
