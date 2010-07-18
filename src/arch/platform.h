/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef PLATFORM_H
#define PLATFORM_H


#ifdef CONFIG_SDL

// Interesting note:
// SDL.h needs to be included on MAC and WIN, so that
// the main(int, char**) function can be used.
#include <SDL.h>

#else // not CONFIG_SDL

#error *** NON-SDL platforms are not supported yet. ***

#endif // CONFIG_SDL


#endif // PLATFORM_H

