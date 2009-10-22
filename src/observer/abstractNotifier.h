// Insert copyright and license information here.

#ifndef ABSTRACT_NOTIFIER_H
#define ABSTRACT_NOTIFIER_H

#include <list>

class AbstractObserver;

struct Observation
{
  int signal;
  AbstractObserver *observer;

  bool operator==(const Observation &other) const {
    return (other.signal == signal) && (other.observer == observer);
  }
};

typedef std::list<Observation > ObservationList;

class AbstractNotifier
{
  public:
    void addObserver( int signal, AbstractObserver *observer );
    void removeObserver( int signal, AbstractObserver *observer );
    void notifyObserver( int signal );
  private:
    ObservationList m_observerList;
};

#endif // ABSTRACT_NOTIFIER_H

