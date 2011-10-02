#include <iostream>
#include <fstream>
#include <argtable2.h>
#include <iomanip>
#include "yaml-cpp/yaml.h"

#include "files.h"
#include "spider.h"

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

  struct arg_str *help  = arg_strn(NULL,NULL,"help",1,1,NULL);
  struct arg_end *endhelp   = arg_end(20);
  void* argsHelp[] = {help, endhelp};
  int helpErrors;
  helpErrors = arg_parse(argc,argv,argsHelp);

  struct arg_rex *import  = arg_rex1(NULL,NULL,"import",NULL,0,NULL);
  struct arg_file *import_comics = arg_filen(NULL,NULL,"FILE", 1, 64, "Yaml file to import.");
  struct arg_end *endimport   = arg_end(20);
  void* argsImport[] = {import, import_comics, endimport};
  int importErrors;
  importErrors = arg_parse(argc,argv,argsImport);

  struct arg_rex *xport  = arg_rex1(NULL,NULL,"export",NULL,0,NULL);
  struct arg_str *xport_comics = arg_strn(NULL,NULL,"COMIC", 1, 64, "Exports COMIC to COMIC.yaml");
  struct arg_end *endxport   = arg_end(20);
  void* argsXport[] = {xport, xport_comics, endxport};
  int xportErrors;
  xportErrors = arg_parse(argc,argv,argsXport);

  struct arg_rex *list  = arg_rex1(NULL,NULL,"list",NULL,0,NULL);
  struct arg_lit *list_current = arg_lit0("c","current","TODOcurrent");
  struct arg_end *endlist   = arg_end(20);
  void* argsList[] = {list, list_current, endlist};
  int listErrors;
  listErrors = arg_parse(argc,argv,argsList);

  if (import->count == 1)
  {
    if(import_comics->count > 0)
    {
      for(unsigned int i = 0; i < import_comics->count; i++)
      {
        cout << "    " << import_comics->filename[i] << endl;
        try 
        {
          char* configFile = (char*)malloc(sizeof(import_comics->filename[i])); strcpy(configFile, import_comics->filename[i]);
          std::ifstream fin(configFile);
          YAML::Parser parser(fin);
          Comic comic;

          YAML::Node doc;    // already parsed
          parser.GetNextDocument(doc);
          if(const YAML::Node *pName = doc.FindValue("name")) // mandatory
            *pName >> comic.name;
          else
          {
            cout << "A name field is required in " << import_comics->filename[i] << endl;
            return 1;
          }
          if(const YAML::Node *pName = doc.FindValue("base_url")) // mandatory
            *pName >> comic.base_url;
          else
          {
            cout << "A base_url field is required in " << import_comics->filename[i] << endl;
            return 1;
          }
          if(const YAML::Node *pName = doc.FindValue("first_url")) // mandatory
            *pName >> comic.first_url;
          else
          {
            cout << "A first_url field is required in " << import_comics->filename[i] << endl;
            return 1;
          }
          if(const YAML::Node *pName = doc.FindValue("img_regex")) // mandatory
          {
            *pName >> comic.img_regex;
          }
          else
          {
            cout << "A img_regex field is required in " << import_comics->filename[i] << endl;
            return 1;
          }
          if(const YAML::Node *pName = doc.FindValue("next_regex")) // mandatory
          {
            *pName >> comic.next_regex;
          }
          else
          {
            cout << "A next_regex field is required in " << import_comics->filename[i] << endl;
            return 1;
          }
          if(const YAML::Node *pName = doc.FindValue("end_on_url"))
            *pName >> comic.end_on_url;
          else
            comic.end_on_url = "";
          if(const YAML::Node *pName = doc.FindValue("read_end_url"))
            *pName >> comic.read_end_url;
          else
            comic.read_end_url = 0;
          if(const YAML::Node *pName = doc.FindValue("download_imgs"))
            *pName >> comic.download_imgs;
          else
            comic.download_imgs = 0;
          free(configFile);

          try
          {
            if(!cache->hasComic(comic.name))
            {
              comic.mark = 0;
              comic.current_url = comic.first_url;
              comic.current_id = 0;
              cache->addComic(comic);
            }
            else
            {
              Comic* oldcomic = cache->getComicConfig(comic.name);
              comic.mark = oldcomic->mark;
              comic.current_url = oldcomic->current_url;
              comic.current_id = oldcomic->current_id;
              cache->updateComicConfig(comic.name, comic);
            }
            cout << comic.name << " imported." << endl;
          }
          catch(Cache::E_CacheDbError e)
          {
            cout << e.what() << endl;
            return 1;
          }
        } 
        catch(YAML::ParserException& e)
        {
          cout << import_comics->filename[i] << e.what() << "\n";
        }
      }
    }
    else
    {
      cout << "You must specify at least one comic file to import." << endl;
    }
  }
  else if (xportErrors == 0)
  {
    if(xport_comics->count > 0)
    {
      for(unsigned int i = 0; i < xport_comics->count; i++)
      {
        if(cache->hasComic(xport_comics->sval[i]))
        {
          try
          {
            Comic* xport_comic = cache->getComicConfig(xport_comics->sval[i]);
            YAML::Emitter out;
            out << YAML::BeginMap;
            out << YAML::Key << "name";
            out << YAML::Value << xport_comic->name;
            out << YAML::Key << "base_url";
            out << YAML::Value << xport_comic->base_url;
            out << YAML::Key << "first_url";
            out << YAML::Value << xport_comic->first_url;
            out << YAML::Key << "img_regex";
            out << YAML::Value << xport_comic->img_regex;
            out << YAML::Key << "next_regex";
            out << YAML::Value << xport_comic->next_regex;
            out << YAML::Key << "end_on_url";
            out << YAML::Value << xport_comic->end_on_url;
            out << YAML::Key << "read_end_url";
            out << YAML::Value << xport_comic->read_end_url;
            out << YAML::Key << "download_imgs";
            out << YAML::Value << xport_comic->download_imgs;
            out << YAML::EndMap;

            const char* settingsFile = std::string(xport_comic->name + ".yaml").c_str();
            std::fstream fout(settingsFile, std::ios::out|std::ios::trunc);
            // cout << "saving: " << strcomb(4, folder.c_str(), "/comics/", comic.name.c_str(), "/settings.yaml") << endl;
            fout.write(out.c_str(), out.size());
            fout.close();
            cout << xport_comic->name << ".yaml has been exported." << endl;
          }
          catch(Cache::E_CacheDbError e)
          {
            cout << e.what() << endl;
            return 1;
          }
        }
        else
        {
          cout << xport_comics->sval[i] << " is not a valid comic name." << endl;
        }
      }
    }
    else
    {
      cout << "You must specify at least one comic to export." << endl;
    }
  }
  else if (listErrors == 0)
  {
    cout << "list" << endl;
    if(list_current->count == 1)
    {
      cout << "    " << "current" << endl;
    }
  }

  /* special case: '--help' takes precedence over error reporting */
  else if (help->count > 0)
  {
    //        printf("Usage: %s", progname);
    cout << "comic [command]\n" << endl;
    cout << "Commands:" << endl;
    arg_print_syntax(stdout,argsImport,"\n");
    arg_print_syntax(stdout,argsXport,"\n");
    arg_print_syntax(stdout,argsList,"\n");
    arg_print_syntax(stdout,argsHelp,"\n");
    //        printf("Echo the STRINGs to standard output.\n\n");
    cout << "\nDescriptions:" << endl;
    cout << "  import" << setw(13) << "" << "Import a comic from a yaml file.\n";//"Imports a comic file (ending in .yaml) to the database." << "\n";
    arg_print_glossary(stdout,argsImport,"    %-16s %s\n");
    cout << "  export" << setw(13) << "" << "Export a comic to a yaml file.\n";
    arg_print_glossary(stdout,argsXport,"    %-16s %s\n");
    cout << "  list" << setw(15) << "" << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsList,"    %-16s %s\n");
    cout << "  help" << setw(15) << "" << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsHelp,"    %-16s %s\n");
    cout << endl;
  }
  /* special case: '--version' takes precedence error reporting */
  /*	else if (vers->count > 0)
      {
      printf("'%s' example program for the \"argtable\" command line argument parser.\n",progname);
      printf("September 2003, Stewart Heitmann\n");
      }
      */
  /* deallocate each non-null entry in argtable[] */
  arg_freetable(argsImport,sizeof(argsImport)/sizeof(argsImport[0]));
  arg_freetable(argsXport,sizeof(argsXport)/sizeof(argsXport[0]));
  arg_freetable(argsHelp,sizeof(argsHelp)/sizeof(argsHelp[0]));


  delete cache;

  /*  Comic comic;
      comic.name = "Schlock";
      comic.base_url = "http://www.schlockmercenary.com";
      comic.first_url = "http://www.schlockmercenary.com/2011-09-22";
      comic.end_on_url = "http://www.schlockmercenary.com/index.html";
      comic.read_end_url = 1;
      comic.download_imgs = 0;
      comic.current_url = comic.first_url;
      comic.current_id = 0;
      comic.img_regex = "(/comics/schlock[0-9]{8}[a-z]?.*?\\.(jpg|gif|png|tiff))";
      comic.next_regex = "(/|(/[0-9]{4}-[0-9]{2}-[0-9]{2})). id=.nav-next";

      Cache* cache = new Cache(".", "cache");
      cache->createCacheDb();
      cache->addComic(comic);

      Spider spider("", comic, cache);

      std::string current = comic.current_url;
      Strip* strip;

      do
      {
      strip = spider.fetchStrip();
      if(strip==NULL)
      break;
      std::cout << strip->page << "\n" << strip->imgs << std::endl;
      }while(strip!=NULL);

      delete cache;
      */
  return 0;
}
