#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

struct Settings
{
	Settings() : 
		viewer("display"),
		browser("firefox")
	{}

	std::string viewer;
	std::string browser;

};

#endif

