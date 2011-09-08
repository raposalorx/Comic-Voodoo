#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <exception>
#include <string>

class Exception: public std::exception
{
  // Ctor
  public:
    explicit Exception(const std::string&) throw();
    virtual ~Exception() throw();

  // What
  public:
    virtual const char* what() const throw();
  private:
    std::string what_str;
};

#define EXCEPTION(name, what, args...) \
  class name: public Exception         \
  {                                    \
    public:                            \
      explicit name(args) throw():     \
        Exception(what)                \
      {                                \
      }                                \
  };

#define EXCEPTION_DEF(name, args...) \
  class name: public Exception       \
  {                                  \
    public:                          \
      explicit name(args) throw();   \
  };

#define EXCEPTION_CTOR(name, what, args...) \
  name::name(args) throw():                 \
    Exception(what)                         \
  {                                         \
  }

#endif // EXCEPTION_H

