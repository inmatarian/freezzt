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
    BG_BLINK_BLACK = 0x80,
    BG_BLINK_BLUE = 0x90,
    BG_BLINK_GREEN = 0xa0,
    BG_BLINK_CYAN = 0xb0,
    BG_BLINK_RED = 0xc0,
    BG_BLINK_MAGENTA = 0xd0,
    BG_BLINK_YELLOW = 0xe0,
    BG_BLINK_GRAY = 0xf0,
  };


}; // namespace Defines

#endif // DEFINES_H
