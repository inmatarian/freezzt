
#ifndef RANDOMIZER_H
#define RANDOMIZER_H

namespace Randomizer
{
  void seed( unsigned int s );
  void timeSeed();
  unsigned int randomInt();
  int randomSInt();
  int randomRange( int top );
  int randomBound( int lower, int upper );
};

#endif
