#ifndef CACHE_H
#define CACHE_H

#include <string>

#include "exception.h"

struct Comic;

class Cache
{
  // Exceptions
  public:
    EXCEPTION_DEF(E_ConfigDbOpenFailed, const std::string&, const std::string&);
    EXCEPTION_DEF(E_ConfigDbSchemaInvalid, const std::string&);
    EXCEPTION_DEF(E_ConfigDbStmtFailed, const std::string&, const std::string&);
    EXCEPTION_DEF(E_NoComicConfigFound, const std::string&);

  // Ctor
  public:
    explicit Cache(const std::string&) throw();

  // Cache
  public:
    void schemaAssert() const throw(E_ConfigDbOpenFailed, E_ConfigDbSchemaInvalid, E_ConfigDbStmtFailed);
    Comic* readComicConfig(const std::string&) const throw(E_ConfigDbOpenFailed, E_ConfigDbStmtFailed, E_NoComicConfigFound);
    void writeComicConfig(const std::string&, const Comic&) throw(E_ConfigDbOpenFailed, E_ConfigDbStmtFailed);
  private:
    const std::string cache_dir;
};

#endif // CACHE_H
