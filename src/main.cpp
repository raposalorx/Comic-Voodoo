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


  if (help->count > 0 && helpErrors == 0)
  {
    cout << "comic [command]\n" << endl;
    cout << "Commands:" << endl;
    arg_print_syntax(stdout,argsImport,"\n");
    arg_print_syntax(stdout,argsXport,"\n");
    arg_print_syntax(stdout,argsList,"\n");
    arg_print_syntax(stdout,argsWatch,"\n");
    arg_print_syntax(stdout,argsUnwatch,"\n");
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

  delete cache;

  return 0;
}
