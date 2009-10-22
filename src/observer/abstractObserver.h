// Insert copyright and license information here.

#ifndef ABSTRACT_OBSERVER_H
#define ABSTRACT_OBSERVER_H

class AbstractObserver
{
  public:
    virtual void handleSignal( int signal ) = 0;
};

#endif // ABSTRACT_OBSERVER_H

