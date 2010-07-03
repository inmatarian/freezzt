/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef __ZZT_THING_SCRIPTABLE_H__
#define __ZZT_THING_SCRIPTABLE_H__

#include <vector>
#include <list>
#include <string>

class ZString;

#include "zztThing.h"

namespace Crunch {
  enum Code {
    None,
    Become,
    Bind,
    Change,
    Char,
    Clear,
    Cycle,
    Die,
    End,
    Endgame,
    Give,
    Go,
    Idle,
    If,
    Lock,
    Play,
    Put,
    Restart,
    Restore,
    Send,
    Set,
    Shoot,
    Take,
    Throwstar,
    Try,
    Unlock,
    Walk,
    Zap
  };
};

namespace ZZTOOP {
  enum Command {
    None,
    Name,
    Label,
    Remark,
    Text,
    PrettyText,
    Menu,
    Move,
    Try,
    Crunch
  };
};

// =================

namespace ZZTThing {

// =================

class ProgramBank : public std::vector<unsigned char>
{
  public:
    signed short length() const
    {
      const size_type s = size();
      return ( s > 32767 ? 32767 : s );
    }
};

// ---------

class ScriptableThing : public AbstractThing
{
  public:
    ScriptableThing();

    void setInstructionPointer( signed short ip );
    void setProgram( ProgramBank *program  );
    void run( int cycles );
    bool paused() const { return m_paused; };

    void setObjectName( const ZString &name );
    const ZString &objectName() const;

    virtual void setCharacter( unsigned char ch ) { /* */ };

    void seekLabel( const ZString &label );
    void sendLabel( const ZString &to, const ZString &label );
    void zapLabel( const ZString &label );
    void restoreLabel( const ZString &label );
    void playSong( const ZString &label );

  protected:
    /// Returns the program the object is using, taking into account #bind
    ProgramBank & program();

    void setPaused( bool p ) { m_paused = p; };

    static void parseTokens( const ProgramBank &program,
                             signed short &ip,
                             ZZTOOP::Command &comType,
                             std::list<ZString> &tokens );

    static Crunch::Code tokenizeCrunch( const ZString &token );

    int executeCrunch( Crunch::Code code, std::list<ZString> &tokens, int cycles );

    static bool seekToken( const ProgramBank &program,
                           ZZTOOP::Command seekCom,
                           const ZString &label,
                           signed short &targetIP );

    virtual bool execMove( int dir ) { return true; };
    virtual void execTry( int dir ) { /* */ };

    virtual void showStrings( const ProgramBank &program,
                              signed short &ip );

    void throwError( const ZString &text );

  private:
    signed short m_ip;
    bool m_paused;
    ZString m_name;
    ProgramBank *m_program;
};

// =================

} // namespace

// =================

#endif /* __ZZT_THING_SCRIPTABLE_H__ */

