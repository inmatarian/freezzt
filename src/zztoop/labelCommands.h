// Insert copyright and license information here.

#ifndef LABEL_COMMANDS_H
#define LABEL_COMMANDS_H

#include <string>
#include "abstractCommand.h"

class ZZTThing;

class LabelCommand : public AbstractCommand
{
  public:
    LabelCommand( const std::string &name, bool zapped )
      : m_name(name), m_zapped( zapped ) { /* */ };

    virtual void exec( ZZTThing *thing ) { /* */ };
    virtual Command::Type type() const { return Command::LABEL; };

    const std::string & name() const { return m_name; };

    void zap() { m_zapped = false; };
    void restore() { m_zapped = true; };
    bool isZapped() const { return m_zapped; };

  private:
    std::string m_name;
    bool m_zapped;
};

// -----------------------------------------------------------------

class ZapCommand : public AbstractCommand
{
  public:
    ZapCommand( const std::string &value )
      : m_value(value) { /* */ };

    virtual void exec( ZZTThing *thing );
    virtual Command::Type type() const { return Command::ZAP; };

    const std::string & value() const { return m_value; };

  private:
    std::string m_value;
};

// -----------------------------------------------------------------

class RestoreCommand : public AbstractCommand
{
  public:
    RestoreCommand( const std::string &value )
      : m_value(value) { /* */ };

    virtual void exec( ZZTThing *thing );
    virtual Command::Type type() const { return Command::RESTORE; };

    const std::string & value() const { return m_value; };

  private:
    std::string m_value;
};

// -----------------------------------------------------------------

class LockCommand : public AbstractCommand
{
  public:
    virtual void exec( ZZTThing *thing );
    virtual Command::Type type() const { return Command::LOCK; };
};

// -----------------------------------------------------------------

class UnlockCommand : public AbstractCommand
{
  public:
    virtual void exec( ZZTThing *thing );
    virtual Command::Type type() const { return Command::UNLOCK; };
};

// -----------------------------------------------------------------

class SendCommand : public AbstractCommand
{
  public:
    SendCommand( const std::string &value )
      : m_value(value) { /* */ };

    virtual void exec( ZZTThing *thing );
    virtual Command::Type type() const { return Command::SEND; };

    const std::string & value() const { return m_value; };

  private:
    std::string m_value;
};

#endif // LABEL_COMMANDS_H

