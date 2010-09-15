#ifndef CONVERSION_H
#define CONVERSION_H

#include <vector>
#include <string>

int mygetch();
char* strcomb(int num, ... );
void reverse(char* s);
char* itoa(int n);
void StringExplode(std::string str, std::string separator, std::vector<std::string>* results);
bool FileExists(std::string strFilename);
char* lower(const char* s);

#endif
