/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef DOT_FILE_PARSER_H
#define DOT_FILE_PARSER_H

class DotFileParserPrivate;

/// Loads a config file following the INI spec.
class DotFileParser
{
  public:
    DotFileParser();
    ~DotFileParser();

    /// declare the keys before loading
    void addKey( const std::string &key );

    /// grabs the keys and options from a file
    void load( const std::string &filename );

    /// access the options according to key and index starting at 1
    std::string getValue( const std::string &key, int index ) const;

    /// access the interger options according to key and index starting at 1
    int getInt( const std::string &key, int index, int defaultValue ) const;

    /// access the boolean options according to key and index starting at 1
    bool getBool( const std::string &key, int index, bool defaultValue ) const;

  private:
    DotFileParserPrivate *d;
};

#endif // DOT_FILE_PARSER_H

