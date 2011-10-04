#ifndef CACHE_H
#define CACHE_H

#include <string>
#include <vector>

#include "exception.h"

struct Comic;
struct Strip;

class Cache
{
  // Exceptions
  public:
    EXCEPTION_DEF(E_CacheDbError, const std::string&, const std::string&);
    EXCEPTION_DEF(E_CacheDbSchemaInvalid, const std::string&);
    EXCEPTION_DEF(E_NoComicConfigFound, const std::string&);
    EXCEPTION_DEF(E_NoStripFound, int, const std::string&);

  // Ctor
  public:
    Cache(const std::string&, const std::string&) throw();

  // Cache
  public:
    void createCacheDb() const throw(E_CacheDbError);
    void schemaAssert() const throw(E_CacheDbError, E_CacheDbSchemaInvalid);

    std::string getDir() {return cache_db;}
  private:
    const std::string cache_db;
    const std::string cache_dir;

  // Comics
  public:
    bool hasComic(const std::string&) const throw(E_CacheDbError);
    void addComic(const Comic&) const throw(E_CacheDbError);
    void remComic(const std::string&) const throw(E_CacheDbError);
    Comic* getComicConfig(const std::string&) const throw(E_CacheDbError, E_NoComicConfigFound);
    void updateComicConfig(const Comic&) const throw(E_CacheDbError);

    std::vector<Comic*>* searchComics(const std::string&, bool) const throw(E_CacheDbError);

  // Strips
  public:
    bool hasStrip(int, const std::string&) const throw(E_CacheDbError);
    void addStrip(const Strip&) const throw(E_CacheDbError);
    void remStrip(int, const std::string&) const throw(E_CacheDbError);
    Strip* getStrip(int, const std::string&) const throw(E_CacheDbError, E_NoStripFound);
    void updateStrip(const Strip&) const throw(E_CacheDbError);
};

#endif // CACHE_H
