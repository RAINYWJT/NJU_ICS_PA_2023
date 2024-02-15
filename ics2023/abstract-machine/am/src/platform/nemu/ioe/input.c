#include <am.h>
#include <nemu.h>

#define KEYDOWN_MASK 0x8000

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  //kbd->keydown = 0;
  //kbd->keycode = AM_KEY_NONE;

  int key=inl(KBD_ADDR);
  kbd->keydown = (key & KEYDOWN_MASK) ? 1 : 0;
  kbd->keycode = key & ~KEYDOWN_MASK;

}
