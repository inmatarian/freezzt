// Insert copyright and license information here.

#include "debug.h"
#include "freezztManager.h"

#include <string>
#include <map>
#include <fstream>

class DotFileParser
{
  public:
    DotFileParser( const std::string &filename )
    {
      using namespace std;
      fstream file( filename.c_str(), ios::in );
      if (file.is_open() && file.good()) {
        while (!file.eof()) {
          string line;
          getline( file, line );
          if ( line.length() <= 0 ||
               line.at(0) == '#' ) continue;
          string::size_type equalSign = line.find_first_of('=');
          if ( equalSign==string::npos ) continue;
          string key = line.substr(0, equalSign);
          string value = line.substr(equalSign+1);
          if ( key.length() <= 0 || value.length() <= 0 ) continue;
          key = trim(key);
          value = trim(value);
          if ( key.length() <= 0 || value.length() <= 0 ) continue;
          m_map[key] = value;
          zinfo() << key << " = " << value;
        }
        file.close();
      }
    };

    std::string getValue( const std::string &key )
    {
      return m_map[key];
    };

  private:
    static std::string trim( const std::string &str )
    {
      using namespace std;
      size_t start = str.find_first_not_of(" \t\f\v\n\r");
      size_t end = str.find_last_not_of(" \t\f\v\n\r");
      if (start!=string::npos && end!=string::npos)
        return str.substr( start, end - start + 1 );
      else return "";
    };

    std::map<std::string, std::string> m_map;
};

int main( int argc, char ** argv )
{
  DebuggingStream::instance()->setLevel( DebuggingStream::INFORMATIVE );
  zinfo() << "Starting";

  DotFileParser dotFile("freezztrc");

  FreeZZTManager freezztManager;
  freezztManager.parseArgs( argc, argv );
  freezztManager.exec();

  zinfo() << "Done.\n";
  return 0;
}

