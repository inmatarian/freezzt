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

typedef std::vector<unsigned char> ProgramBank;

class ScriptableThing : public AbstractThing
{
  public:
    ScriptableThing();

    void setInstructionPointer( signed short ip );
    void setProgram( const ProgramBank &program  );
    void run( int cycles );
    bool paused() const { return m_paused; };

    void setObjectName( const std::string &name );
    const std::string &objectName() const;

    virtual void setCharacter( unsigned char ch ) { /* */ };

  protected:
    void setPaused( bool p ) { m_paused = p; };


    static void parseTokens( const ProgramBank &program,
                             signed short &ip,
                             ZZTOOP::Command &comType,
                             std::list<std::string> &tokens );

    static Crunch::Code tokenizeCrunch( const std::string &token );

    virtual bool execMove( int dir ) { return true; };
    virtual void execTry( int dir ) { /* */ };
    void throwError( const std::string &text );

  private:
    unsigned int m_ip;
    bool m_paused;
    std::string m_name;
    ProgramBank m_program;
};

// =================

} // namespace

// =================

#endif /* __ZZT_THING_SCRIPTABLE_H__ */

