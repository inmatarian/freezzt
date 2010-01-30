/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef ABSTRACT_EVENT_LOOP_H
#define ABSTRACT_EVENT_LOOP_H

class FreeZZTManager;

class AbstractEventLoop
{
  public:
    AbstractEventLoop()
      : pManager(0)
    {/* */};

    virtual void exec() = 0;
    virtual void sleep( int milliseconds ) = 0;
    virtual int clock() const = 0;
    virtual void stop() = 0;

    void setManager( FreeZZTManager *manager ) { pManager = manager; };
    FreeZZTManager *manager() const { return pManager; };

  private:
    FreeZZTManager *pManager;
};

#endif // ABSTRACT_EVENT_LOOP_H

