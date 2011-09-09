#include "cache.h"

#include <sqlite3.h>

#include "comic.h"

#define CONFIG_SCHEMA  "(name text primarykey, base_url text, first_url text, img_regex text, next_regex text)"
#define CONFIG_TABLE   "comic-configs"
#define DB_TABLE_COUNT 1


// --------------------------------------------------------------------------------
//  Helper classes
// --------------------------------------------------------------------------------

class SQLite3Db
{
  // Exceptions
  public:
    EXCEPTION(E_OpenFailed, "Could not open database file: " + config_file + "; " + details, const std::string& config_file, const std::string& details);

  // Ctor+dtor
  public:
    SQLite3Db(const std::string& config_file, int flags) throw(E_OpenFailed):
      db(NULL)
    {
      if (sqlite3_open_v2(config_file.c_str(), &db, flags, NULL) != SQLITE_OK)
      {
        std::string details(sqlite3_errmsg(db));
        sqlite3_close(db);
        throw E_OpenFailed(config_file, details);
      }
    }
    ~SQLite3Db() throw()
    {
      sqlite3_close(db);
    }

  // Database
  public:
    operator sqlite3*() throw()
    {
      return db;
    }
    operator const sqlite3*() const throw()
    {
      return db;
    }
  private:
    sqlite3* db;
};

class SQLite3Stmt
{
  // Exceptions
  public:
    EXCEPTION(E_PrepareFailed, details, const std::string& details);
    EXCEPTION(E_StepFailed, details, const std::string& details);

  // Ctor+dtor
  public:
    SQLite3Stmt(SQLite3Db& db, const std::string& statement) throw(E_PrepareFailed):
      db(db),
      stmt(NULL)
    {
      if (sqlite3_prepare_v2(db, statement.c_str(), -1, &stmt, NULL) != SQLITE_OK)
        throw E_PrepareFailed(sqlite3_errmsg(db));
    }
    ~SQLite3Stmt() throw()
    {
      sqlite3_finalize(stmt);
    }

  // Statement
  public:
    operator sqlite3_stmt*() throw()
    {
      return stmt;
    }
    operator const sqlite3_stmt*() const throw()
    {
      return stmt;
    }
    bool step() const throw(E_StepFailed)
    {
      switch (sqlite3_step(stmt))
      {
        case SQLITE_OK:
          return true;
        case SQLITE_DONE:
          return false;
        default:
          throw E_StepFailed(sqlite3_errmsg(db));
      }
    }
  private:
    sqlite3* db;
    sqlite3_stmt* stmt;
};


// --------------------------------------------------------------------------------
//  Exceptions
// --------------------------------------------------------------------------------

Cache::EXCEPTION_CTOR(E_CacheDbOpenFailed, "Could not open config database file '" + config_file + "'; " + details, const std::string& config_file, const std::string& details)
Cache::EXCEPTION_CTOR(E_CacheDbStmtFailed, "Statement '" + statement + "' on config database failed; " + details, const std::string& statement, const std::string& details)
Cache::EXCEPTION_CTOR(E_CacheDbSchemaInvalid, "Config database file '" + config_file + "' has an invalid schema", const std::string& config_file);
Cache::EXCEPTION_CTOR(E_NoComicConfigFound, "Comic '" + comic_name + "' does not exist in database" , const std::string& comic_name)


// --------------------------------------------------------------------------------
//  Ctor
// --------------------------------------------------------------------------------

Cache::Cache(const std::string& cache_dir) throw():
  cache_dir(cache_dir)
{
}


// --------------------------------------------------------------------------------
//  Cache
// --------------------------------------------------------------------------------

void Cache::createCacheDb(const std::string& db_path) const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed)
{
  std::string configtable_stmt_str = "CREATE TABLE " CONFIG_TABLE CONFIG_SCHEMA ";";

  try
  {
    // TODO - throw exception if db file already exists
    SQLite3Db db(db_path, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
    try
    {
      SQLite3Stmt stmt(db, configtable_stmt_str);
      stmt.step();
    }
    catch (SQLite3Stmt::E_PrepareFailed e)
    { throw E_CacheDbStmtFailed(configtable_stmt_str, e.what()); }
    catch (SQLite3Stmt::E_StepFailed e)
    { throw E_CacheDbStmtFailed(configtable_stmt_str, e.what()); }
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbOpenFailed(db_path,  e.what()); }
}

void Cache::schemaAssert(const std::string& db_path) const throw(E_CacheDbOpenFailed, E_CacheDbSchemaInvalid, E_CacheDbStmtFailed)
{
  std::string stmt_str = "SELECT `tbl_name`,`sql` FROM `sqlite_master`;";
  int correct_schemas = 0;

  try
  {
    SQLite3Db db(db_path, SQLITE_OPEN_READONLY);
    SQLite3Stmt stmt(db, stmt_str);
    while (!stmt.step())
    {
      if (std::string((const char*)sqlite3_column_text(stmt, 0)) == CONFIG_TABLE)
      {
        if (std::string((const char*)sqlite3_column_text(stmt, 1)) == "CREATE TABLE " CONFIG_TABLE CONFIG_SCHEMA)
          correct_schemas++;
      }
    }
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbOpenFailed(db_path,  e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbStmtFailed(stmt_str, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbStmtFailed(stmt_str, e.what()); }

  if (correct_schemas < DB_TABLE_COUNT)
    throw E_CacheDbSchemaInvalid(db_path);
}

Comic* Cache::readComicConfig(const std::string& db_path, const std::string& comic_name) const throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed, E_NoComicConfigFound)
{
  Comic comic;
  std::string stmt_str = comic.getSQLSelectStr(CONFIG_TABLE, comic_name);

  try
  {
    schemaAssert(db_path);
    SQLite3Db db(db_path, SQLITE_OPEN_READONLY);
    SQLite3Stmt stmt(db, stmt_str);
    if (!stmt.step())
      throw E_NoComicConfigFound(comic_name);

    comic.base_url.assign((const char*)sqlite3_column_text(stmt, 0));
    comic.first_url.assign((const char*)sqlite3_column_text(stmt, 1));
    comic.img_regex.assign((const char*)sqlite3_column_text(stmt, 2));
    comic.next_regex.assign((const char*)sqlite3_column_text(stmt, 3));
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbOpenFailed(db_path,  e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbStmtFailed(stmt_str, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbStmtFailed(stmt_str, e.what()); }

  return new Comic(comic);
}

void Cache::writeComicConfig(const std::string& db_path, const std::string& comic_name, const Comic& comic) throw(E_CacheDbOpenFailed, E_CacheDbStmtFailed)
{
  std::string stmt_str = comic.getSQLInsertStr(CONFIG_TABLE, comic_name);

  try
  {
    schemaAssert(db_path);
    SQLite3Db db(db_path, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
    SQLite3Stmt stmt(db, stmt_str);
    stmt.step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbOpenFailed(db_path, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbStmtFailed(stmt_str, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbStmtFailed(stmt_str, e.what()); }
}
