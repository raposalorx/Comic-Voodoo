#include <boost/filesystem.hpp>
#include <string>
#include <argtable2.h>

#include "cache.h"

#include "exception.h"

namespace files
{
  EXCEPTION_DEF(E_CreationFailed, const std::string&);
};

void createFolders(std::string) throw(files::E_CreationFailed);

void createCache(Cache*) throw(files::E_CreationFailed);

int importYaml(Cache*, struct arg_file*);

int exportYaml(Cache*, struct arg_str*);
