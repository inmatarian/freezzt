/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef  __ZZTOOPINTERP_H__
#define  __ZZTOOPINTERP_H__

#include <vector>
#include "zstring.h"

namespace ZZTThing {
  class ScriptableThing;
};

// -------------------------------------
namespace ZZTOOP {
// -------------------------------------

typedef std::vector<unsigned char> ProgramBank;

class Interpreter
{
  public:
    Interpreter() : m_board(0) { /* */ };

    void setBoard( GameBoard *board ) { m_board = board; };
    GameBoard *board() const { return m_board; };

    void setProgram( const unsigned char *stream, int length );
    ZString getObjectName() const;

    void run( ZZTThing::ScriptableThing *thing, int cycles );
    void seekLabel( ZZTThing::ScriptableThing *thing, const ZString &label );

    void zapLabel( const ZString &label );
    void restoreLabel( const ZString &label );

  private:
    ProgramBank program;
    GameBoard *m_board;
};

// -------------------------------------
}; // namespace ZZTOOP
// -------------------------------------

#endif

