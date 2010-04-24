#ifndef CONVERSION_H
#define CONVERSION_H

#include <vector>
#include <string>

using namespace std;

int mygetch();
char* strcomb(int num, ... );
void reverse(char* s);
char* itoa(int n);
void StringExplode(string str, string separator, vector<string>* results);
bool FileExists(string strFilename);
char* lower(const char* s);

#endif
