// Insert copyright and license information here.

#ifndef DOT_FILE_PARSER_H
#define DOT_FILE_PARSER_H

class DotFileParserPrivate;
class DotFileParser
{
  public:
    DotFileParser();
    ~DotFileParser();

    void addKey( const std::string &key );

    void load( const std::string &filename );

    std::string getValue( const std::string &key, int index ) const;
    int getInt( const std::string &key, int index ) const;

  private:
    DotFileParserPrivate *d;
};


#endif // DOT_FILE_PARSER_H

