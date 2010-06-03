#ifndef  __ZZT_STRING_H__
#define  __ZZT_STRING_H__

#include <string>

class ZString : public std::string
{
  public:
    explicit ZString() : std::string() { /* */ };
    ZString( const std::string& s, size_t p, size_t n = npos ) : std::string(s, p, n) {};
    ZString( const char *s, size_t n ) : std::string(s, n) {};
    ZString( const std::string &s ) : std::string(s) {};
    ZString( const char *s ) : std::string(s) {};
    ZString( size_t n, char c ) : std::string(n,c) {};

    void toUpper();
    ZString upper() const;

    void toLower();
    ZString lower() const;
};

#endif // __ZZT_STRING_H__

