#ifndef CACHE_H
#define CACHE_H

#include <string>

#include "exception.h"

struct Comic;

class Cache
{
  // Exceptions
  public:
    EXCEPTION_DEF(E_CacheDbOpenFailed, const std::string&, const std::string&);
    EXCEPTION_DEF(E_CacheDbSchemaInvalid, const std::string&);
    EXCEPTION_DEF(E_CacheDbStmtFailed, const std::string&, const std::string&);
    EXCEPTION_DEF(E_NoComicConfigFound, const std::string&);

  // Ctor
  public:
    Cache(const std::string&, const std::string&) throw();

  // Cache
  public:
    void createCacheDb() const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed);
    void schemaAssert() const throw(E_CacheDbOpenFailed, E_CacheDbSchemaInvalid, E_CacheDbStmtFailed);
  private:
    const std::string cache_db;
    const std::string cache_dir;

  // Comics
  public:
    bool hasComic(const std::string&) const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed);
    void addComic(const std::string&, const Comic&) const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed);
    void remComic(const std::string&) const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed);
    Comic* getComicConfig(const std::string&) const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed, E_NoComicConfigFound);
    void updateComicConfig(const std::string&, const Comic&) const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed);
};

#endif // CACHE_H
