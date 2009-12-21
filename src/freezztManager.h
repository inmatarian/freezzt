// Insert copyright and license information here.

#ifndef FREEZZT_MANAGER_H
#define FREEZZT_MANAGER_H

#include "abstractManager.h"

class FreeZZTManagerPrivate;
class FreeZZTManager : public AbstractManager
{
  public:
    FreeZZTManager();
    virtual ~FreeZZTManager();

    void parseArgs( int argc, char ** argv );

    virtual void exec();

    virtual void setFrameRate( int hertz );
    virtual int frameRate() const;

  private:
    FreeZZTManagerPrivate *d;
};

#endif // FREEZZT_MANAGER_H

