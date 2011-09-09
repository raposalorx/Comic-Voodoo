#ifndef CACHE_H
#define CACHE_H

#include <string>

#include "exception.h"

struct Comic;

class Cache
{
  // Exceptions
  public:
    EXCEPTION_DEF(E_CacheDbError, const std::string&, const std::string&);
    EXCEPTION_DEF(E_CacheDbSchemaInvalid, const std::string&);
    EXCEPTION_DEF(E_NoComicConfigFound, const std::string&);

  // Ctor
  public:
    Cache(const std::string&, const std::string&) throw();

  // Cache
  public:
    void createCacheDb() const throw(E_CacheDbError);
    void schemaAssert() const throw(E_CacheDbError, E_CacheDbSchemaInvalid);
  private:
    const std::string cache_db;
    const std::string cache_dir;

  // Comics
  public:
    bool hasComic(const std::string&) const throw(E_CacheDbError);
    void addComic(const std::string&, const Comic&) const throw(E_CacheDbError);
    void remComic(const std::string&) const throw(E_CacheDbError);
    Comic* getComicConfig(const std::string&) const throw(E_CacheDbError, E_NoComicConfigFound);
    void updateComicConfig(const std::string&, const Comic&) const throw(E_CacheDbError);
};

#endif // CACHE_H
