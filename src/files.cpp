#include "files.h"

#include "yaml-cpp/yaml.h"
#include <fstream>
#include <iostream>

#include "comic.h"
#include "comhelp.h"

namespace fs = boost::filesystem;

using std::cout;
using std::endl;

files::EXCEPTION_CTOR(E_CreationFailed, message, const std::string& message);

void createFolders(std::string voodoo_home) throw(files::E_CreationFailed)
{
  fs::file_status voodoo_status = fs::status(voodoo_home);
  if(!fs::exists(voodoo_status))
  {
    if(!fs::create_directory(voodoo_home))
    {
      throw files::E_CreationFailed("Failed to create " + voodoo_home);
    }
  }
}

void createCache(Cache* cache) throw(files::E_CreationFailed)
{
  fs::file_status cache_status = fs::status(cache->getDir());
  if(!fs::exists(cache_status))
  {
    try
    {
      cache->createCacheDb();
    }
    catch(Cache::E_CacheDbError e)
    {
      throw files::E_CreationFailed("Failed to create a new database.");
    }
  }
}

int importYaml(Cache* cache, struct arg_file* import_comics)
{
  if(import_comics->count > 0)
  {
    for(signed int i = 0; i < import_comics->count; i++)
    {
      fs::file_status file_status = fs::status(import_comics->filename[i]);
      if(fs::exists(file_status))
      {
        try 
        {
          const char* configFile = import_comics->filename[i];
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
          if(const YAML::Node *pName = doc.FindValue("searchpattern"))
            *pName >> comic.searchpattern;
          else
          {
            cout << "You need a searchpattern " << import_comics->filename[i] << endl;
            return 1;
          }

          try
          {
            if(!cache->hasComic(comic.name))
            {
              comic.mark = -1;
              comic.current_url = comic.first_url;
              comic.current_id = 0;
              comic.watched = 0;
              cache->addComic(comic);
            }
            else
            {
              Comic* oldcomic = cache->getComicConfig(comic.name);
              comic.mark = oldcomic->mark;
              comic.current_url = oldcomic->current_url;
              comic.current_id = oldcomic->current_id;
              comic.watched = oldcomic->watched;
              cache->updateComicConfig(comic);
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
          return 1;
        }
      }
      else
      {
        cout << "\"" << import_comics->filename[i] << "\" does not exist." << endl;
        return 1;
      }
    }
  }
  else
  {
    cout << "You must specify at least one comic file to import." << endl;
  }
  return 0;
}

int exportYaml(Cache* cache, struct arg_str* xport_comics)
{
  if(xport_comics->count > 0)
  {
    for(signed int i = 0; i < xport_comics->count; i++)
    {
      try
      {
        Comic* xport_comic = comicLookup(cache, xport_comics->sval[i], 0);
        if(xport_comic != NULL)
        {
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
          out << YAML::Key << "searchpattern";
          out << YAML::Value << xport_comic->searchpattern;
          out << YAML::EndMap;

          const char* settingsFile = std::string(xport_comic->name + ".yaml").c_str();
          std::fstream fout(settingsFile, std::ios::out|std::ios::trunc);
          fout.write(out.c_str(), out.size());
          fout.close();
          cout << xport_comic->name << ".yaml has been exported." << endl;
        }
      }
      catch(Cache::E_CacheDbError e)
      {
        cout << e.what() << endl;
        return 1;
      }
    }
  }
  else
  {
    cout << "You must specify at least one comic to export." << endl;
  }
  return 0;
}
