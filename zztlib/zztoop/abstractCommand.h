// Insert copyright and license information here.

#ifndef ABSTRACT_COMMAND_H
#define ABSTRACT_COMMAND_H

class ZZTThing;

namespace Command {
  enum Type
  {
    NONE = 0,
    // object maintanence
    NAME,
    CYCLE,
    BIND,
    IDLE,
    CHAR,
    RESTART,
    DIE,
    END,
    // label related commands
    LABEL,
    SEND,
    ZAP,
    RESTORE,
    LOCK,
    UNLOCK,
    // printing commands
    PRINT,
    CENTER_PRINT,
    HYPERLINK,
    // flag commands
    IF,
    SET,
    CLEAR,
    // movement commands
    GO,
    WALK,
    TRY,
    // violent commands
    SHOOT,
    THROWSTAR,
    // item commands    
    BECOME,
    PUT,
    CHANGE,
    GIVE,
    TAKE,
    // that other stuff
    PLAY,
    ENDGAME
  };
};

class AbstractCommand
{
  public:
    virtual void exec( ZZTThing *thing ) = 0;
    virtual Command::Type type() const = 0;
};

#endif // ABSTRACT_COMMAND_H

