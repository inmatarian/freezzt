/**
 * @file
 * @author  Inmatarian <inmatarian@gmail.com>
 * @section LICENSE
 * Insert copyright and license information here.
 */

#ifndef DEFINES_H
#define DEFINES_H

namespace Defines {

  /// ZZT virtual keyboard
  enum KeyCode
  {
    Z_None = 0,
    Z_Unicode,
    Z_Up,
    Z_Down,
    Z_Left,
    Z_Right,
    Z_ShootUp,
    Z_ShootDown,
    Z_ShootLeft,
    Z_ShootRight,
    Z_Escape,
    Z_Enter,
    Z_Backspace,
    Z_Tab,
    Z_Home,
    Z_End,
    Z_PageUp,
    Z_PageDown,
    Z_Insert,
    Z_Delete,
    Z_F1,
    Z_F2,
    Z_F3,
    Z_F4,
    Z_F5,
    Z_F6,
    Z_F7,
    Z_F8,
    Z_F9,
    Z_F10,
    Z_F11,
    Z_F12,
    Z_MaxKeyCode
  };

  /// list of colors
  enum ColorChart {
    BLACK = 0,
    DARK_BLUE = 1,
    DARK_GREEN = 2,
    DARK_CYAN = 3,
    DARK_RED = 4,
    DARK_PURPLE = 5,
    DARK_BROWN = 6,
    GRAY = 7,
    DARK_GRAY = 8,
    BLUE = 9,
    GREEN = 0xa,
    CYAN = 0xb,
    RED = 0xc,
    MAGENTA = 0xd,
    YELLOW = 0xe,
    WHITE = 0xf,
    BG_BLUE = 0x10,
    BG_GREEN = 0x20,
    BG_CYAN = 0x30,
    BG_RED = 0x40,
    BG_PURPLE = 0x50,
    BG_BROWN = 0x60,
    BG_GRAY = 0x70,
    BLINK = 0x80,
  };

  /// list of pipes
  enum PipeChart {
    PIPE_NS = 0xb3,
    PIPE_NSW = 0xb4,
    PIPE_NSW_DH = 0xb5,
    PIPE_NSW_DV = 0xb6,
    PIPE_SW_DV= 0xb7,
    PIPE_SW_DH = 0xb8,
    PIPE_NSW_DH_DV = 0xb9,
    PIPE_NS_DV = 0xba,
    PIPE_SW_DH_DV = 0xbb,
    PIPE_NW_DH_DV = 0xbc,
    PIPE_NW_DV = 0xbd,
    PIPE_NW_DH = 0xbe,
    PIPE_SW = 0xbf,
    PIPE_NE = 0xc0,
    PIPE_NWE = 0xc1,
    PIPE_SWE = 0xc2,
    PIPE_NSE = 0xc3,
    PIPE_WE = 0xc4,
    PIPE_NSWE = 0xc5,
    PIPE_NSE_DH = 0xc6,
    PIPE_NSE_DV = 0xc7,
    PIPE_NE_DH_DV = 0xc8,
    PIPE_SE_DH_DV = 0xc9,
    PIPE_NWE_DH_DV = 0xca,
    PIPE_SWE_DH_DV = 0xcb,
    PIPE_NSE_DH_DV = 0xcc,
    PIPE_WE_DH = 0xcd,
    PIPE_NSWE_DH_DV = 0xce,
    PIPE_NWE_DH = 0xcf,
    PIPE_NWE_DV = 0xd0,
    PIPE_SWE_DH = 0xd1,
    PIPE_SWE_DV = 0xd2,
    PIPE_NE_DV = 0xd3,
    PIPE_NE_DH = 0xd4,
    PIPE_SE_DH = 0xd5,
    PIPE_SE_DV = 0xd6,
    PIPE_NSWE_DV = 0xd7,
    PIPE_NSWE_DH = 0xd8,
    PIPE_NW = 0xd9,
    PIPE_SE = 0xda
  };

}; // namespace Defines

inline int boundInt( const int left, const int value, const int right )
{
  return ( value < left ) ? left
       : ( value > right) ? right
                          : value;
}

#endif // DEFINES_H
