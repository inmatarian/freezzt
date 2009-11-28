// Insert copyright and license information here.

#ifndef DOT_FILE_PARSER_H
#define DOT_FILE_PARSER_H

class DotFileParserPrivate;
class DotFileParser
{
  public:
    DotFileParser( const std::string &filename );
    ~DotFileParser();

    std::string getValue( const std::string &key, int index );

  private:
    DotFileParserPrivate *d;
};


#endif // DOT_FILE_PARSER_H

