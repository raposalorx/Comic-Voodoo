#ifndef CACHE_H
#define CACHE_H

#include <string>

struct Comic;

class Cache
{
	// Ctor
	public:
		explicit Cache(const std::string&);

	// Cache
	public:
		Comic* readComicConfig(const std::string&) const;
		void writeComicConfig(const Comic&);
	private:
		const std::string cache_dir;
};

#endif // CACHE_H
