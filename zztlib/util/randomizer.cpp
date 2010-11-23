
#include <ctime>
#include "randomizer.h"

// Mimicking pascal's dumb randomizer.

static unsigned int value = 1;
static const unsigned int multiplier = 134775813;

static void step()
{
  value = value * multiplier + 1;
}

void Randomizer::seed( unsigned int s )
{
  value = s;
}

void Randomizer::timeSeed()
{
  seed( time(0) );
}

unsigned int Randomizer::randomInt()
{
  step();
  return value;
}

int Randomizer::randomSInt()
{
  step();
  return value;
}

int Randomizer::randomRange( int top )
{
  return randomSInt() % top;
}

int Randomizer::randomBound( int lower, int upper )
{
  if ( lower > upper ) {
    int t = upper;
    upper = lower;
    lower = t;
  }
  int range = upper - lower;
  return lower + randomRange(range);
}

