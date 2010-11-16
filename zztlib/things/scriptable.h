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

    void setPaused( bool pause ) { m_paused = pause; };
    bool paused() const { return m_paused; };

    void setLocked( bool lock ) { m_locked = lock; };
    bool locked() const { return m_locked; };

    void setObjectName( const ZString &name );
    const ZString &objectName() const { return m_name; };

    void seekLabel( const ZString &label );

    void showMessage( const ZString &mesg );
    virtual void showScroll( TextScrollModel *model );

    virtual void execBecome( unsigned char id, unsigned char color ) { /* */ };
    virtual void execBind( const ZString &name ) { /* */ };
    virtual void execChange( unsigned char frId, unsigned char frColor,
                             unsigned char toId, unsigned char toColor ) { /* */ };
    virtual void execChar( unsigned char ch ) { /* */ };
    virtual void execClear( const ZString &flag );
    virtual void execCycle( int number ) { /* */ };
    virtual void execDie() { /* */ };
    virtual void execEnd() { /* */ };
    virtual void execEndgame() { /* */ };
    virtual void execGive( int id, int amount ) { /* */ };
    virtual bool execGo( int dir ) { return true; };
    virtual void execIdle() { /* */ };
    virtual void execIf( const ZString &flag, const ZString &message ) { /* */ };
    virtual void execLock() { /* */ };
    virtual void execPlay( const ZString &label );
    virtual void execPut( int dir, unsigned char id, unsigned char color ) { /* */ };
    virtual void execRestart() { /* */ };
    virtual void execRestore( const ZString &label );
    virtual void execSend( const ZString &message );
    virtual void execSend( const ZString &to, const ZString &message );
    virtual void execSet( const ZString &flag );
    virtual void execShoot( int dir ) { /* */ };
    virtual void execTake( int item, int amount, const ZString &message ) { /* */ };
    virtual void execThrowstar( int dir ) { /* */ };
    virtual void execTry( int dir ) { /* */ };
    virtual void execTry( int dir, const ZString &label ) { /* */ };
    virtual void execUnlock() { /* */ };
    virtual void execWalk( int dir ) { /* */ };
    virtual void execZap( const ZString &label );

  protected:
    void run( int cycles );

  private:
    signed short m_ip;
    bool m_paused;
    bool m_locked;
    ZString m_name;
    ZZTOOP::Interpreter *m_interpreter;
};

// -------------------------------------

class Scroll : public ScriptableThing
{
  public:
    Scroll();
    virtual unsigned char entityID() const { return ZZTEntity::Scroll; };
    virtual unsigned char tile() const { return 0xE8; };

    virtual void handleTouched();

 protected:
    virtual void exec_impl();
};

// -------------------------------------

class Object : public ScriptableThing
{
  public:
    Object();
    virtual unsigned char entityID() const { return ZZTEntity::Object; };
    virtual unsigned char tile() const { return m_char; };

    virtual void setCharacter( unsigned char ch ) { m_char = ch; };

    virtual void handleTouched();

  protected:
    virtual void exec_impl();

  private:
    unsigned char m_char;
};

// =================

} // namespace

// =================

#endif /* __ZZT_THING_SCRIPTABLE_H__ */

