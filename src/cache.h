#ifndef CACHE_H
#define CACHE_H

#include <string>

struct Comic;

class Cache
{
	// Ctor
	public:
		explicit Cache(const std::string&) throw();

	// Cache
	public:
		Comic* readComicConfig(const std::string&) const throw(E_ConfigDbLocked);
		void writeComicConfig(const Comic&) throw(E_ConfigDbLocked);
	private:
		const std::string cache_dir;
};

#endif // CACHE_H
