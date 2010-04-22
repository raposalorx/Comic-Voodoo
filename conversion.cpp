#include "conversion.h"

#include <cstring>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#include <sys/stat.h> 

int mygetch( ) 
{
	struct termios oldt,
		newt;
	int            ch;
	tcgetattr( STDIN_FILENO, &oldt );
	newt = oldt;
	newt.c_lflag &= ~( ICANON | ECHO );
	tcsetattr( STDIN_FILENO, TCSANOW, &newt );
	ch = getchar();
	tcsetattr( STDIN_FILENO, TCSANOW, &oldt );
	return ch;
}

char* strcomb(int num, ... )
{
	va_list ap;
	va_start(ap, num);
	size_t len = 0;
	for(int i = 0; i < num; i++)
	{
		len += strlen(va_arg(ap, char*));
	}
	char* combined = (char*) malloc(len+1);
	
	va_end(ap);
	va_start(ap, num);
	len = 0;
	
	for(int i = 0; i < num; i++)
	{
		char* swap = va_arg(ap, char*);
		strncpy(combined+len, swap, strlen(swap));
		len += strlen(swap);
	}
	combined[len] = '\0';
	
	va_end(ap);
	return combined;
}

void reverse(char* s)
{
    size_t c, i, j;

    for (i = 0, j = strlen(s)-1; i<j; i++, j--) {
        c = s[i];
        s[i] = s[j];
        s[j] = c;
    }
}

char* itoa(int n)
{
	char* s = (char*)malloc(sizeof(int));
    int i, sign;

    if ((sign = n) < 0)  /* record sign */
        n = -n;          /* make n positive */
    i = 0;
    do {       /* generate digits in reverse order */
        s[i++] = n % 10 + '0';   /* get next digit */
    } while ((n /= 10) > 0);     /* delete it */
    if (sign < 0)
        s[i++] = '-';
    s[i] = '\0';
    reverse(s);

	return s;
}

// stolen functions
void StringExplode(string str, string separator, vector<string>* results)
{
	results->clear();
    int found;
    found = str.find_first_of(separator);
    while(found != (signed) string::npos)
	{
        if(found > 0)
		{
            results->push_back(str.substr(0,found));
        }
        str = str.substr(found+1);
        found = str.find_first_of(separator);
    }
    if(str.length() > 0)
	{
        results->push_back(str);
    }
}

bool FileExists(string strFilename) {
  struct stat stFileInfo;
  bool blnReturn;
  int intStat;

  // Attempt to get the file attributes
  intStat = stat(strFilename.c_str(),&stFileInfo);
  if(intStat == 0) {
    // We were able to get the file attributes
    // so the file obviously exists.
    blnReturn = true;
  } else {
    // We were not able to get the file attributes.
    // This may mean that we don't have permission to
    // access the folder which contains this file. If you
    // need to do that level of checking, lookup the
    // return values of stat which will give you
    // more details on why stat failed.
    blnReturn = false;
  }
  
  return(blnReturn);
}
