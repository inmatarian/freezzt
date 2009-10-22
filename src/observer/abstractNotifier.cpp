
#include <list>
#include "abstractNotifier.h"
#include "abstractObserver.h"

void AbstractNotifier::addObserver( int signal, AbstractObserver *observer )
{
  Observation o;
  o.signal = signal;
  o.observer = observer;
  m_observerList.push_back( o );
}

void AbstractNotifier::removeObserver( int signal, AbstractObserver *observer )
{
  Observation o;
  o.signal = signal;
  o.observer = observer;
  m_observerList.remove( o );
}

void AbstractNotifier::notifyObserver( int signal )
{
  ObservationList::iterator it;
  for ( it = m_observerList.begin(); it != m_observerList.end(); it++ )
  {
    const Observation &o = *it;
    if ( o.signal != signal ) {
      continue;
    }
    
    // notify observer about signal
    o.observer->handleSignal( signal );
  }  
}

