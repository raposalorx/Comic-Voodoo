#include "cache.h"

#include <stdio.h>
#include <memory>
#include <sstream>
#include <sqlite3.h>

#include "comic.h"
#include "strip.h"

#define CONFIG_TABLE  "options"
#define CONFIG_SCHEMA "(`option` TEXT, `value` TEXT)"
#define STRIP_TABLE   "strips"
#define STRIP_SCHEMA  "(`id` INTEGER, `comic_name` TEXT, `page` TEXT, `imgs` TEXT)"
#define COMIC_TABLE   "comics"
#define COMIC_SCHEMA  "(`name` TEXT PRIMARY KEY,  `base_url` TEXT, `first_url` TEXT, `img_regex` TEXT, `next_regex` TEXT, `end_on_url` TEXT, `read_end_url` INTEGER, `download_imgs` INTEGER, `mark` INTEGER, `current_url` TEXT, `current_id` INTEGER, `watched` INTEGER, `searchpattern` Text)"


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
        case SQLITE_ROW:
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

std::string escape(std::string str)
{
  for(unsigned int i = 0; i < str.size(); i++)
  {
    if(str[i]=='\'')
    {
      str.replace(i, 1, "''");
      i++;
    }
  }
  return str;
}

std::string intToString(int i) throw()
{
  return ((std::stringstream&)(std::stringstream() << i)).str();
}

std::string getComicSQLInsertStr(const Strip& strip) throw()
{
  return "INSERT INTO `" STRIP_TABLE "` "
         "("
           "`id`,"
           "`comic_name`,"
           "`page`,"
           "`imgs`"
         ") "
         "VALUES "
         "("
           "'" + escape(intToString(strip.id))  + "',"
           "'" + escape(strip.comic_name)       + "',"
           "'" + escape(strip.page)             + "',"
           "'" + escape(strip.imgs)             + "'"
         ");";
}

std::string getComicSQLSelectStr(int id, const std::string& comic_name) throw()
{
  return "SELECT "
           "`id`,"
           "`comic_name`, "
            "`page`,"
            "`imgs`"
         " FROM `" STRIP_TABLE "` "
         "WHERE `id`=" + escape(intToString(id)) + " AND `comic_name`='" + escape(comic_name) + "';";
}

std::string getComicSQLUpdateStr(int id, const std::string& comic_name, const Strip& strip) throw()
{
  return "UPDATE `" STRIP_TABLE "` "
         "SET "
           "`id`='"         + escape(intToString(strip.id)) + "',"
           "`comic_name`='" + escape(strip.comic_name)      + "', "
           "`page`='"       + escape(strip.page)            + "', "
           "`imgs`='"       + escape(strip.imgs)            + "'"
         " WHERE `id`=" + escape(intToString(id)) + " AND `comic_name`='" + escape(comic_name) + "';";
}

std::string getConfigSQLInsertStr(const Comic& comic) throw()
{
  return "INSERT INTO `" COMIC_TABLE "` "
         "("
           "`name`,"
           "`base_url`,"
           "`first_url`,"
           "`img_regex`,"
           "`next_regex`,"
           "`end_on_url`,"
           "`read_end_url`,"
           "`download_imgs`,"
           "`mark`,"
           "`current_url`,"
           "`current_id`,"
           "`watched`,"
           "`searchpattern`"
         ") "
         "VALUES "
         "("
           "'" + escape(comic.name)                       + "',"
           "'" + escape(comic.base_url)                   + "',"
           "'" + escape(comic.first_url)                  + "',"
           "'" + escape(comic.img_regex)                  + "',"
           "'" + escape(comic.next_regex)                 + "',"
           "'" + escape(comic.end_on_url)                 + "',"
           "'" + escape(intToString(comic.read_end_url))  + "',"
           "'" + escape(intToString(comic.download_imgs)) + "',"
           "'" + escape(intToString(comic.mark))          + "',"
           "'" + escape(comic.current_url)                + "',"
           "'" + escape(intToString(comic.current_id))    + "',"
           "'" + escape(intToString(comic.watched))       + "',"
           "'" + escape(comic.searchpattern)              + "'"
         ");";
}

std::string getConfigSQLSelectStr(const std::string& comic_name) throw()
{
  return "SELECT "
           "`name`,"
           "`base_url`,"
           "`first_url`,"
           "`img_regex`,"
           "`next_regex`,"
           "`end_on_url`,"
           "`read_end_url`,"
           "`download_imgs`,"
           "`mark`,"
           "`current_url`,"
           "`current_id`,"
           "`watched`,"
           "`searchpattern`"
           " FROM `" COMIC_TABLE "` "
         "WHERE `name`='" + escape(comic_name) + "';";
}

std::string getConfigSQLUpdateStr(const Comic& comic) throw()
{
  return "UPDATE `" COMIC_TABLE "` "
         "SET "
           "`name`='"           + escape(comic.name)                        + "',"
           "`base_url`='"       + escape(comic.base_url)                    + "',"
           "`first_url`='"      + escape(comic.first_url)                   + "',"
           "`img_regex`='"      + escape(comic.img_regex)                   + "',"
           "`next_regex`='"     + escape(comic.next_regex)                  + "',"
           "`end_on_url`='"     + escape(comic.end_on_url)                  + "',"
           "`read_end_url`='"   + escape(intToString(comic.read_end_url))   + "',"
           "`download_imgs`='"  + escape(intToString(comic.download_imgs))  + "',"
           "`mark`='"           + escape(intToString(comic.mark))           + "',"
           "`current_url`='"    + escape(comic.current_url)                 + "',"
           "`current_id`='"     + escape(intToString(comic.current_id))     + "',"
           "`watched`='"     + escape(intToString(comic.watched))        + "',"
           "`searchpattern`='" + escape(comic.searchpattern) + "'"
         " WHERE `name`='" + escape(comic.name) + "';";
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
    SQLite3Stmt(db, "CREATE TABLE " CONFIG_TABLE CONFIG_SCHEMA ";").step();
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

    // Assert that the strip table exists and is valid
    {
      SQLite3Stmt stmt(db, "SELECT `sql` FROM `sqlite_master` WHERE `tbl_name`='" STRIP_TABLE "';");
      if (!stmt.step() || std::string((const char*)sqlite3_column_text(stmt, 0)) != "CREATE TABLE " STRIP_TABLE STRIP_SCHEMA)
        throw E_CacheDbSchemaInvalid(cache_db);
    }

    // Assert that the comic table exists and is valid
    {
      SQLite3Stmt stmt(db, "SELECT `sql` FROM `sqlite_master` WHERE `tbl_name`='" COMIC_TABLE "';");
      if (!stmt.step() || std::string((const char*)sqlite3_column_text(stmt, 0)) != "CREATE TABLE " COMIC_TABLE COMIC_SCHEMA)
        throw E_CacheDbSchemaInvalid(cache_db);
    }

    // Assert that the config table exists and is valid
    {
      SQLite3Stmt stmt(db, "SELECT `sql` FROM `sqlite_master` WHERE `tbl_name`='" CONFIG_TABLE "';");
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
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
    SQLite3Stmt stmt(db, "SELECT 1 FROM `" COMIC_TABLE "` WHERE `name`='" + escape(comic_name) + "';");
    return stmt.step();
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
    SQLite3Stmt(db, getConfigSQLInsertStr(comic)).step();
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
    SQLite3Stmt(db, "DELETE FROM `" STRIP_TABLE "` WHERE `name`='" + escape(comic_name) + "';").step();
    SQLite3Stmt(db, "DELETE FROM `" COMIC_TABLE "` WHERE `name`='" + escape(comic_name) + "';").step();
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
    SQLite3Stmt stmt(db, getConfigSQLSelectStr(comic_name));
    if (!stmt.step())
      throw E_NoComicConfigFound(comic_name);

    comic->name.assign((const char*)sqlite3_column_text(stmt, 0));
    comic->base_url.assign((const char*)sqlite3_column_text(stmt, 1));
    comic->first_url.assign((const char*)sqlite3_column_text(stmt, 2));
    comic->img_regex.assign((const char*)sqlite3_column_text(stmt, 3));
    comic->next_regex.assign((const char*)sqlite3_column_text(stmt, 4));
    comic->end_on_url.assign((const char*)sqlite3_column_text(stmt, 5));
    comic->read_end_url = sqlite3_column_int(stmt, 6);
    comic->download_imgs = sqlite3_column_int(stmt, 7);
    comic->mark = sqlite3_column_int(stmt, 8);
    comic->current_url.assign((const char*)sqlite3_column_text(stmt, 9));
    comic->current_id = sqlite3_column_int(stmt, 10);
    comic->watched = sqlite3_column_int(stmt, 11);
    comic->searchpattern.assign((const char*)sqlite3_column_text(stmt, 12));

    return comic.release();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db,  e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

void Cache::updateComicConfig(const Comic& comic) const throw(E_CacheDbError)
{
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, getConfigSQLUpdateStr(comic)).step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

std::vector<Comic*>* Cache::searchComics(const std::string& comic_name, bool watched) const throw(E_CacheDbError)
{
  try
  {
    std::auto_ptr<std::vector<Comic*> > comics(new std::vector<Comic*>);
    std::vector<std::string> names;

    {
      SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
      std::string query;
      if(!watched)
        query = "SELECT * FROM `" COMIC_TABLE "` WHERE `name` LIKE '" + escape(comic_name) + "%';";
      else
        query = "SELECT * FROM `" COMIC_TABLE "` WHERE `name` LIKE '" + escape(comic_name) + "%' AND `watched`<>'0';";
      SQLite3Stmt stmt(db, query);
      if (!stmt.step())
        return comics.release();
      
      for(int i = 0;i < sqlite3_column_count(stmt);i++)
      {
        names.push_back((const char*)sqlite3_column_text(stmt, 0));
        if (!stmt.step())
          break;
      }
    }
    for(unsigned int i = 0; i < names.size(); i++)
    {
      comics->push_back(getComicConfig(names[i]));
    }
    return comics.release();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db,  e.what()); }
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
    SQLite3Stmt stmt(db, "SELECT 1 FROM `" STRIP_TABLE "` WHERE `id`=" + intToString(id) + " AND `comic_name`='" + escape(comic_name) + "';");
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
    SQLite3Stmt(db, getComicSQLInsertStr(strip)).step();
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
    SQLite3Stmt(db, "DELETE FROM `" STRIP_TABLE "` WHERE `id`=" + intToString(id) + " AND `comic_name`='" + escape(comic_name) + "';").step();
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
    SQLite3Stmt stmt(db, getComicSQLSelectStr(id, comic_name));
    if (!stmt.step())
      throw E_NoStripFound(id, comic_name);

    strip->id = sqlite3_column_int(stmt, 0);
    strip->comic_name.assign((const char*)sqlite3_column_text(stmt, 1));
    strip->page.assign((const char*)sqlite3_column_text(stmt, 2));
    strip->imgs.assign((const char*)sqlite3_column_text(stmt, 3));

     return strip.release();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db,  e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
   { throw E_CacheDbError(cache_db, e.what()); }
}

void Cache::updateStrip(const Strip& strip) const throw(E_CacheDbError)
{
  try
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, getComicSQLUpdateStr(strip.id, strip.comic_name, strip)).step();
  }
  catch (SQLite3Db::E_OpenFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_PrepareFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
  catch (SQLite3Stmt::E_StepFailed e)
  { throw E_CacheDbError(cache_db, e.what()); }
}

std::vector<Strip*>* Cache::searchStrips(const Comic& comic, const std::string& pattern)
{
  std::auto_ptr<std::vector<Strip*> > strips(new std::vector<Strip*>);
  std::vector<int> ids;
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
    std::string query;
    if(pattern.substr(0, comic.searchpattern.size()) == comic.searchpattern)
    {
      query = escape(pattern);
    }
    else
    {
      query = escape(comic.searchpattern) + escape(pattern) + "%";
    }
    SQLite3Stmt stmt(db, "SELECT `id` FROM `" STRIP_TABLE "` WHERE `page` LIKE '" + query + "';");
    if (!stmt.step())
      return strips.release();

    for(;;)
    {
      ids.push_back(sqlite3_column_int(stmt, 0));
      if (!stmt.step())
        break;
    }
  }
  for(int i = 0;i < ids.size();i++)
  {
    strips->push_back(getStrip(ids[i], comic.name));
  }
  return strips.release();
}

// --------------------------------------------------------------------------------
//  Config
// --------------------------------------------------------------------------------

std::string Cache::getOption(const std::string& option) const
{
  SQLite3Db db(cache_db, SQLITE_OPEN_READONLY);
  SQLite3Stmt stmt(db, "SELECT `value` FROM `" CONFIG_TABLE "` WHERE `option`='" + escape(option) + "';");
  if (!stmt.step())
    return "";

  std::string value = ((const char*)sqlite3_column_text(stmt, 0));
  return value;
}

void Cache::setOption(const std::string& option, const std::string& value)
{
  if(getOption(option) != "")
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, "UPDATE `" CONFIG_TABLE "` SET `value`='" + escape(value) + "' WHERE `option`='" + escape(option) + "';").step();
  }
  else
  {
    SQLite3Db db(cache_db, SQLITE_OPEN_READWRITE);
    SQLite3Stmt(db, "INSERT INTO `" CONFIG_TABLE "` VALUES ('" + escape(option) + "','" + escape(value) + "');").step();
  }
}
