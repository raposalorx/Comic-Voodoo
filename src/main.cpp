#include <iostream>
#include <argtable2.h>
#include <iomanip>

#include "spider.h"

int main(int argc, char** argv)
{
  using std::cout;
  using std::endl;
  using std::setw;

  struct arg_str *help  = arg_strn(NULL,NULL,"help",1,1,NULL);
  struct arg_end *endhelp   = arg_end(20);
  void* argsHelp[] = {help, endhelp};
  int helpErrors;

  struct arg_rex *add  = arg_rex1(NULL,NULL,"add",NULL,0,NULL);
  struct arg_str *add_comic = arg_str1(NULL,NULL,"\"comic\"", NULL);
  struct arg_end *endadd   = arg_end(20);
  void* argsAdd[] = {add, add_comic, endadd};
  int addErrors;

  struct arg_rex *rm  = arg_rex1(NULL,NULL,"rm",NULL,0,NULL);
  struct arg_end *endrm   = arg_end(20);
  void* argsRm[] = {rm, endrm};
  int rmErrors;

  struct arg_rex *list  = arg_rex1(NULL,NULL,"list",NULL,0,NULL);
  struct arg_lit *list_c     = arg_lit0("c", NULL, "List currently watched comics.");
  struct arg_end *endlist   = arg_end(20);
  void* argsList[] = {list, list_c, endlist};
  int listErrors;

  struct arg_rex *fetch  = arg_rex1(NULL,NULL,"fetch",NULL,0,NULL);
  struct arg_lit *fetch_r     = arg_lit0("r", NULL,         "Re-fetch an entire comic, starting from the beginning.");
  struct arg_lit *fetch_m     = arg_lit0("m", "mark", "Mark each fetched comic with the earliest newly fetched stip");
  struct arg_str *fetch_comic = arg_str1(NULL,NULL,"[comic]", "Comic to add");
  struct arg_end *endfetch   = arg_end(20);
  void* argsFetch[] = {fetch, fetch_r, fetch_m, fetch_comic, endfetch};
  int fetchErrors;

  struct arg_rex *mark  = arg_rex1(NULL,NULL,"mark",NULL,0,NULL);
  struct arg_lit *mark_f     = arg_lit0("f", NULL, "First");
  struct arg_lit *mark_e     = arg_lit0("e", NULL, "End");
  struct arg_lit *mark_l     = arg_lit0("l", NULL, "List");
  struct arg_str *mark_comic = arg_str1(NULL,NULL,"[comic]", "Comic");
  struct arg_int *mark_n = arg_intn("n", "num", NULL, 0, 1, "Number of the strip");
  struct arg_end *endmark   = arg_end(20);
  void* argsMark[] = {mark, mark_f, mark_e, mark_l, mark_comic, mark_n, endmark};
  int markErrors;

  struct arg_rex *view  = arg_rex1(NULL,NULL,"view",NULL,0,NULL);
  struct arg_lit *view_m = arg_lit0("m", NULL, "View comic at the mark.");
  struct arg_int *view_n = arg_intn("n", "num", NULL, 0, 1, "Number of the strip");
  struct arg_str *view_comic = arg_str1(NULL,NULL,"\"comic\"", "Comic to view");
  struct arg_end *endview   = arg_end(20);
  void* argsView[] = {view, view_m, view_n, view_comic, endview};
  int viewErrors;

  struct arg_rex *set  = arg_rex1(NULL,NULL,"set",NULL,0,NULL);
  struct arg_end *endset   = arg_end(20);
  void* argsSet[] = {set, endset};
  int setErrors;

  struct arg_rex *update  = arg_rex1(NULL,NULL,"update",NULL,0,NULL);
  struct arg_end *endupdate   = arg_end(20);
  void* argsUpdate[] = {update, endupdate};
  int updateErrors;

  /* verify the argtable[] entries were allocated sucessfully */
  /*    if (arg_nullcheck(argtable) != 0)
        {
   * NULL entries were detected, some allocations must have failed *
   printf("%s: insufficient memory\n",progname);
   }
   */
  /* Parse the command line as defined by argtable[] */
  helpErrors = arg_parse(argc,argv,argsHelp);
  addErrors = arg_parse(argc,argv,argsAdd);
  rmErrors = arg_parse(argc,argv,argsRm);
  listErrors = arg_parse(argc,argv,argsList);
  fetchErrors = arg_parse(argc,argv,argsFetch);
  markErrors = arg_parse(argc,argv,argsMark);
  viewErrors = arg_parse(argc,argv,argsView);
  setErrors = arg_parse(argc,argv,argsSet);
  updateErrors = arg_parse(argc,argv,argsUpdate);

  if (addErrors == 0)
  {
    /*		if(add_comic->count == 1)
          {
          std::fstream comics_file(comicsfile.c_str(), std::ios::out|std::ios::trunc); // add comic to comics list
          for(unsigned int i = 0; i < comics.size(); i++)
          {
          const string s = strcomb(3, "- ", comics.at(i).name.c_str(), "\n");
          comics_file.write(s.c_str(), s.size());
          }
          const string s = strcomb(3, "- ", add_comic, "\n");
          comics_file.write(s.c_str(), s.size());
          comics_file.close();
          loadComics(comics); // validate and cull comics
          }
          else
          cout << "You must specify one comic to add." << endl;
          */
  }
  else if (rmErrors == 0)
  {
    cout << "rm";
  }
  else if (listErrors == 0)
  {
    cout << "list";
  }
  else if (fetchErrors == 0)
  {
    cout << "fetch";
    if (fetch_r->count >0)
    {
      cout << " -r";
    }
  }
  else if (markErrors == 0)
  {
    cout << "mark";
  }
  else if (viewErrors == 0)
  {
    cout << "view";
  }
  else if (setErrors == 0)
  {
    cout << "set";
  }
  else if (updateErrors == 0)
  {
    cout << "update";
  }

  /* special case: '--help' takes precedence over error reporting */
  else if (help->count > 0)
  {
    //        printf("Usage: %s", progname);
    cout << "comic [command]\n" << endl;
    cout << "Commands:" << endl;
    arg_print_syntax(stdout,argsAdd,"\n");
    arg_print_syntax(stdout,argsRm,"\n");
    arg_print_syntax(stdout,argsList,"\n");
    arg_print_syntax(stdout,argsFetch,"\n");
    arg_print_syntax(stdout,argsMark,"\n");
    arg_print_syntax(stdout,argsView,"\n");
    arg_print_syntax(stdout,argsSet,"\n");
    arg_print_syntax(stdout,argsUpdate,"\n");
    arg_print_syntax(stdout,argsHelp,"\n");
    //        printf("Echo the STRINGs to standard output.\n\n");
    cout << "\nDescriptions:" << endl;
    cout << "  add" << setw(28) << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsAdd,"    %-16s %s\n");
    cout << "  rm" << setw(29) << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsRm,"  %-16s %s\n");
    cout << "  list" << setw(27) << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsList,"      %-16s %s\n");
    cout << "  fetch" << setw(26) << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsFetch,"      %-16s %s\n");
    cout << "  mark" << setw(27) << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsMark,"      %-16s %s\n");
    cout << "  view" << setw(27) << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsView,"      %-16s %s\n");
    cout << "  set" << setw(28) << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsSet,"      %-16s %s\n");
    cout << "  update" << setw(25) << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsUpdate,"      %-16s %s\n");
    cout << "  help" << setw(27) << "HOLY SHIT!" << "\n";
    arg_print_glossary(stdout,argsHelp,"      %-16s %s\n");
    cout << endl;
  }

  /* special case: '--version' takes precedence error reporting */
  /*	else if (vers->count > 0)
      {
      printf("'%s' example program for the \"argtable\" command line argument parser.\n",progname);
      printf("September 2003, Stewart Heitmann\n");
      }
      */
  /* If the parser returned any errors then display them and exit */
  //	else if (nerrors > 0)
  //      {
  /* Display the error details contained in the arg_end struct.*/
  /*        arg_print_errors(stdout,end,progname);
            printf("Try '%s --help' for more information.\n",progname);
            }
            */
  /* deallocate each non-null entry in argtable[] */
  arg_freetable(argsAdd,sizeof(argsAdd)/sizeof(argsAdd[0]));
  arg_freetable(argsRm,sizeof(argsRm)/sizeof(argsRm[0]));
  arg_freetable(argsList,sizeof(argsList)/sizeof(argsList[0]));
  arg_freetable(argsFetch,sizeof(argsFetch)/sizeof(argsFetch[0]));
  arg_freetable(argsMark,sizeof(argsMark)/sizeof(argsMark[0]));
  arg_freetable(argsView,sizeof(argsView)/sizeof(argsView[0]));
  arg_freetable(argsSet,sizeof(argsSet)/sizeof(argsSet[0]));
  arg_freetable(argsUpdate,sizeof(argsUpdate)/sizeof(argsUpdate[0]));
  arg_freetable(argsHelp,sizeof(argsHelp)/sizeof(argsHelp[0]));

  /*  Comic comic;
      comic.name = "Schlock";
      comic.base_url = "http://www.schlockmercenary.com";
      comic.first_url = "http://www.schlockmercenary.com/2011-09-22";
      comic.end_on_url = "http://www.schlockmercenary.com/index.html";
      comic.read_end_url = 1;
      comic.download_imgs = 0;
      comic.current_url = comic.first_url;
      comic.current_id = 0;
      comic.img_regex = "(/comics/schlock[0-9]{8}[a-z]?.*?\\.(jpg|gif|png|tiff))";
      comic.next_regex = "(/|(/[0-9]{4}-[0-9]{2}-[0-9]{2})). id=.nav-next";

      Cache* cache = new Cache(".", "cache");
      cache->createCacheDb();
      cache->addComic(comic);

      Spider spider("", comic, cache);

      std::string current = comic.current_url;
      Strip* strip;

      do
      {
      strip = spider.fetchStrip();
      if(strip==NULL)
      break;
      std::cout << strip->page << "\n" << strip->imgs << std::endl;
      }while(strip!=NULL);

      delete cache;
      */
  return 0;
}
