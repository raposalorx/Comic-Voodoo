#include "cache.h"

#include <memory>
#include <sqlite3.h>

#include "comic.h"

#define COMIC_TABLE   "pages"
#define COMIC_SCHEMA  "(`comic_name` TEXT)"
#define CONFIG_TABLE  "configs"
#define CONFIG_SCHEMA "(`name` TEXT PRIMARY KEY, `base_url` TEXT, `first_url` TEXT, `img_regex` TEXT, `next_regex` TEXT)"


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

Cache::EXCEPTION_CTOR(E_CacheDbError, "Error while accessing the database file '" + config_file + "'; " + details, const std::string& config_file, const std::string& details)
Cache::EXCEPTION_CTOR(E_CacheDbSchemaInvalid, "Config database file '" + config_file + "' has an invalid schema", const std::string& config_file);
Cache::EXCEPTION_CTOR(E_NoComicConfigFound, "Comic '" + comic_name + "' does not exist in database" , const std::string& comic_name)


// --------------------------------------------------------------------------------
//  Ctor
// --------------------------------------------------------------------------------

Cache::Cache(const std::string& cache_dir, const std::string& cache_file) throw():
  cache_db(cache_dir + '/' + cache_file),
  cache_dir(cache_dir)
{
}


// --------------------------------------------------------------------------------
//  Cache
// --------------------------------------------------------------------------------

void Cache::createCacheDb() const throw(E_CacheDbError)
{
  const std::string& configtable_stmt_str = "CREATE TABLE " CONFIG_TABLE CONFIG_SCHEMA ";";

  try
  {
    // TODO - throw exception if db file already exists
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
    SQLite3Stmt stmt(db, configtable_stmt_str);
    stmt.step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db,  e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

void Cache::schemaAssert() const throw(E_CacheDbError, E_CacheDbSchemaInvalid)
{
  const std::string& comicschema_stmt_str = "SELECT `sql` FROM `sqlite_master` WHERE `tbl_name`='" COMIC_TABLE "';";
  const std::string& configschema_stmt_str = "SELECT `sql` FROM `sqlite_master` WHERE `tbl_name`='" CONFIG_TABLE "';";

  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);

    // Assert that the comics table exists and is valid
    {
      SQLite3Stmt stmt(db, comicschema_stmt_str);
      if (!stmt.step() || std::string((const char*)sqlite3_column_text(stmt, 0)) != "CREATE TABLE " COMIC_TABLE COMIC_SCHEMA)
        throw E_CacheDbSchemaInvalid(cache_db);
    }

    // Assert that the config table exists and is valid
    {
      SQLite3Stmt stmt(db, configschema_stmt_str);
      if (!stmt.step() || std::string((const char*)sqlite3_column_text(stmt, 0)) != "CREATE TABLE " CONFIG_TABLE CONFIG_SCHEMA)
        throw E_CacheDbSchemaInvalid(cache_db);
    }
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db,  e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}


// --------------------------------------------------------------------------------
//  Comics
// --------------------------------------------------------------------------------

bool Cache::hasComic(const std::string& comic_name) const throw(E_CacheDbError)
{
  const std::string stmt_str = "SELECT `name` FROM `" CONFIG_TABLE "` WHERE `name`='" + comic_name + "';";

  try
  {
    schemaAssert();
    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
    SQLite3Stmt stmt(db, stmt_str);
    stmt.step();
    return (!std::string((const char*)sqlite3_column_text(stmt, 0)).empty());
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

void Cache::addComic(const std::string& comic_name, const Comic& comic) const throw(E_CacheDbError)
{
  // TODO
}

void Cache::remComic(const std::string& comic_name) const throw(E_CacheDbError)
{
  // TODO
}

Comic* Cache::getComicConfig(const std::string& comic_name) const throw(E_CacheDbError, E_NoComicConfigFound)
{
  std::auto_ptr<Comic> comic(new Comic);
  const std::string stmt_str = comic->getSQLSelectStr(CONFIG_TABLE, comic_name);

  try
  {
    schemaAssert();
    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
    SQLite3Stmt stmt(db, stmt_str);
    if (!stmt.step())
      throw E_NoComicConfigFound(comic_name);

    comic->base_url.assign((const char*)sqlite3_column_text(stmt, 0));
    comic->first_url.assign((const char*)sqlite3_column_text(stmt, 1));
    comic->img_regex.assign((const char*)sqlite3_column_text(stmt, 2));
    comic->next_regex.assign((const char*)sqlite3_column_text(stmt, 3));
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db,  e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }

  return comic.release();
}

void Cache::updateComicConfig(const std::string& comic_name, const Comic& comic) const throw(E_CacheDbError)
{
  const std::string stmt_str = comic.getSQLUpdateStr(CONFIG_TABLE, comic_name);

  try
  {
    schemaAssert();
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt stmt(db, stmt_str);
    stmt.step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}
