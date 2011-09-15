#include "cache.h"

#include <memory>
#include <sstream>
#include <sqlite3.h>

#include "comic.h"
#include "strip.h"

#define CONFIG_TABLE   "options"
#define CONFIG_SCHEMA  "(`option` TEXT, `value` TEXT)"
#define STRIP_TABLE   "strips"
#define STRIP_SCHEMA  "(`id` INTEGER, `comic_name` TEXT, `page` TEXT, `imgs` TEXT)"
#define COMIC_TABLE  "comics"
#define COMIC_SCHEMA "(`name` TEXT PRIMARY KEY,  `base_url` TEXT, `first_url` TEXT, `img_regex` TEXT, `next_regex` TEXT, `end_on_url` TEXT, `read_end_url` INTEGER, `download_imgs` INTEGER, `mark` INTEGER, `last_url` TEXT, `last_img` TEXT, `current_url` TEXT)"


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
//  Helper functions
// --------------------------------------------------------------------------------

std::string intToString(int i) throw()
{
  return ((std::stringstream&)(std::stringstream() << i)).str();
}

std::string getComicSQLInsertStr(const std::string& table_name, const Strip& strip) throw()
{
  return "INSERT INTO `" + table_name + "` "
         "("
           "`id`,"
           "`comic_name`"
         ") "
         "VALUES "
         "("
           "'" + intToString(strip.id) + "',"
           "'" + strip.comic_name      + "'"
         ");";
}

std::string getComicSQLSelectStr(const std::string& table_name, int id, const std::string& comic_name) throw()
{
  return "SELECT "
           "`id`,"
           "`comic_name` "
         "FROM `" + table_name + "` "
         "WHERE `id`=" + intToString(id) + " AND `comic_name`='" + comic_name + "';";
}

std::string getComicSQLUpdateStr(const std::string& table_name, int id, const std::string& comic_name, const Strip& strip) throw()
{
  return "UPDATE `" + table_name + "` "
         "SET "
           "`id`='"         + intToString(strip.id) + "',"
           "`comic_name`='" + strip.comic_name      + "' "
         "WHERE `id`=" + intToString(id) + " AND `comic_name`='" + comic_name + "';";
}

std::string getConfigSQLInsertStr(const std::string& table_name, const Comic& comic) throw()
{
  return "INSERT INTO `" + table_name + "` "
         "("
           "`name`,"
           "`base_url`,"
           "`first_url`,"
           "`current_url`,"
           "`current_id`,"
           "`img_regex`,"
           "`next_regex`"
         ") "
         "VALUES "
         "("
           "'" + comic.name                    + "',"
           "'" + comic.base_url                + "',"
           "'" + comic.first_url               + "',"
           "'" + comic.current_url             + "',"
           "'" + intToString(comic.current_id) + "',"
           "'" + comic.img_regex               + "',"
           "'" + comic.next_regex              + "'"
         ");";
}

std::string getConfigSQLSelectStr(const std::string& table_name, const std::string& comic_name) throw()
{
  return "SELECT "
           "`name`,"
           "`base_url`,"
           "`first_url`,"
           "`current_url`,"
           "`current_id`,"
           "`img_regex`,"
           "`next_regex` "
         "FROM `" + table_name + "` "
         "WHERE `name`='" + comic_name + "';";
}

std::string getConfigSQLUpdateStr(const std::string& table_name, const std::string& comic_name, const Comic& comic) throw()
{
  return "UPDATE `" + table_name + "` "
         "SET "
           "`name`='"        + comic.name                    + "',"
           "`base_url`='"    + comic.base_url                + "',"
           "`first_url`='"   + comic.first_url               + "',"
           "`current_url`='" + comic.current_url             + "',"
           "`current_id`='"  + intToString(comic.current_id) + "',"
           "`img_regex`='"   + comic.img_regex               + "',"
           "`next_regex`='"  + comic.next_regex              + "' "
         "WHERE `name`='" + comic_name + "';";
}


// --------------------------------------------------------------------------------
//  Exceptions
// --------------------------------------------------------------------------------

Cache::EXCEPTION_CTOR(E_CacheDbError, "Error while accessing the database file '" + config_file + "'; " + details, const std::string& config_file, const std::string& details)
Cache::EXCEPTION_CTOR(E_CacheDbSchemaInvalid, "Config database file '" + config_file + "' has an invalid schema", const std::string& config_file);
Cache::EXCEPTION_CTOR(E_NoComicConfigFound, "Comic '" + comic_name + "' does not exist in database" , const std::string& comic_name)
Cache::EXCEPTION_CTOR(E_NoStripFound, "Strip #" + intToString(strip_id) + " from comic '" + comic_name + "' does not exist in database", int strip_id, const std::string& comic_name)


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
  try
  {
    // TODO - throw exception if db file already exists
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE);
    SQLite3Stmt(db, "CREATE TABLE " STRIP_TABLE STRIP_SCHEMA ";").step();
    SQLite3Stmt(db, "CREATE TABLE " COMIC_TABLE COMIC_SCHEMA ";").step();
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
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);

    // Assert that the comics table exists and is valid
    {
      SQLite3Stmt stmt(db, "SELECT `sql` FROM `sqlite_master` WHERE `tbl_name`='" STRIP_TABLE "';");
      if (!stmt.step() || std::string((const char*)sqlite3_column_text(stmt, 0)) != "CREATE TABLE " STRIP_TABLE STRIP_SCHEMA)
        throw E_CacheDbSchemaInvalid(cache_db);
    }

    // Assert that the config table exists and is valid
    {
      SQLite3Stmt stmt(db, "SELECT `sql` FROM `sqlite_master` WHERE `tbl_name`='" COMIC_TABLE "';");
      if (!stmt.step() || std::string((const char*)sqlite3_column_text(stmt, 0)) != "CREATE TABLE " COMIC_TABLE COMIC_SCHEMA)
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
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
    SQLite3Stmt stmt(db, "SELECT 1 FROM `" COMIC_TABLE "` WHERE `name`='" + comic_name + "';");
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

void Cache::addComic(const Comic& comic) const throw(E_CacheDbError)
{
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, getConfigSQLInsertStr(COMIC_TABLE, comic)).step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

void Cache::remComic(const std::string& comic_name) const throw(E_CacheDbError)
{
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, "DELETE FROM `" STRIP_TABLE "` WHERE `name`='" + comic_name + "';").step();
    SQLite3Stmt(db, "DELETE FROM `" COMIC_TABLE "` WHERE `name`='" + comic_name + "';").step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

Comic* Cache::getComicConfig(const std::string& comic_name) const throw(E_CacheDbError, E_NoComicConfigFound)
{
  try
  {
    std::auto_ptr<Comic> comic(new Comic);

    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
    SQLite3Stmt stmt(db, getConfigSQLSelectStr(COMIC_TABLE, comic_name));
    if (!stmt.step())
      throw E_NoComicConfigFound(comic_name);

    comic->name.assign((const char*)sqlite3_column_text(stmt, 0));
    comic->base_url.assign((const char*)sqlite3_column_text(stmt, 1));
    comic->first_url.assign((const char*)sqlite3_column_text(stmt, 2));
    comic->current_url.assign((const char*)sqlite3_column_text(stmt, 3));
    comic->current_id = sqlite3_column_int(stmt, 4);
    comic->img_regex.assign((const char*)sqlite3_column_text(stmt, 5));
    comic->next_regex.assign((const char*)sqlite3_column_text(stmt, 6));

    return comic.release();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db,  e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

void Cache::updateComicConfig(const std::string& comic_name, const Comic& comic) const throw(E_CacheDbError)
{
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, getConfigSQLUpdateStr(COMIC_TABLE, comic_name, comic)).step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}


// --------------------------------------------------------------------------------
//  Strips
// --------------------------------------------------------------------------------

bool Cache::hasStrip(int id, const std::string& comic_name) const throw(E_CacheDbError)
{
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
    SQLite3Stmt stmt(db, "SELECT 1 FROM `" STRIP_TABLE "` WHERE `id`=" + intToString(id) + " AND `comic_name`='" + comic_name + "';");
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

void Cache::addStrip(const Strip& strip) const throw(E_CacheDbError)
{
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, getComicSQLInsertStr(STRIP_TABLE, strip)).step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

void Cache::remStrip(int id, const std::string& comic_name) const throw(E_CacheDbError)
{
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, "DELETE FROM `" STRIP_TABLE "` WHERE `id`=" + intToString(id) + " AND `comic_name`='" + comic_name + "';").step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

Strip* Cache::getStrip(int id, const std::string& comic_name) const throw(E_CacheDbError, E_NoStripFound)
{
  try
  {
    std::auto_ptr<Strip> strip(new Strip);

    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
    SQLite3Stmt stmt(db, getComicSQLSelectStr(STRIP_TABLE, id, comic_name));
    if (!stmt.step())
      throw E_NoStripFound(id, comic_name);

    strip->id = sqlite3_column_int(stmt, 0);
    strip->comic_name.assign((const char*)sqlite3_column_text(stmt, 1));

    return strip.release();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db,  e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

void Cache::updateStrip(int id, const std::string& comic_name, const Strip& strip) const throw(E_CacheDbError)
{
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, getComicSQLUpdateStr(STRIP_TABLE, id, comic_name, strip)).step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}
