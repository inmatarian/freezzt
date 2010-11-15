/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef __ZZT_THING_SCRIPTABLE_H__
#define __ZZT_THING_SCRIPTABLE_H__

#include "zstring.h"
#include "zztThing.h"
#include "zztoopInterp.h"

class TextScrollModel;

// =================

namespace ZZTThing {

// =================

class ScriptableThing : public AbstractThing
{
  public:
    ScriptableThing();

    void setInstructionPointer( signed short ip );
    signed short instructionPointer() const { return m_ip; };

    void setInterpreter( ZZTOOP::Interpreter *interp );
    ZZTOOP::Interpreter *interpreter() const { return m_interpreter; };

    void setPaused( bool p ) { m_paused = p; };
    bool paused() const { return m_paused; };

    void setObjectName( const ZString &name );
    const ZString &objectName() const { return m_name; };

    virtual void setCharacter( unsigned char ch ) { /* */ };

    void seekLabel( const ZString &label );
    void sendLabel( const ZString &to, const ZString &label );
    void playSong( const ZString &label );

    void showMessage( const ZString &mesg );

    virtual bool execMove( int dir ) { return true; };
    virtual void execTry( int dir ) { /* */ };

    virtual void showScroll( TextScrollModel *model );

  protected:
    void run( int cycles );

  private:
    signed short m_ip;
    bool m_paused;
    ZString m_name;
    ZZTOOP::Interpreter *m_interpreter;
};

// =================

} // namespace

// =================

#endif /* __ZZT_THING_SCRIPTABLE_H__ */

