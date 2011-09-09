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
    explicit Cache(const std::string&) throw();

  // Cache
  public:
    void createCacheDb(const std::string&) const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed);
    void schemaAssert(const std::string&) const throw(E_CacheDbOpenFailed, E_CacheDbSchemaInvalid, E_CacheDbStmtFailed);
    Comic* readComicConfig(const std::string&, const std::string&) const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed, E_NoComicConfigFound);
    void writeComicConfig(const std::string&, const std::string&, const Comic&) throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed);
  private:
    const std::string cache_dir;
};

#endif // CACHE_H
