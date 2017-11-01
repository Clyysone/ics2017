#include "cpu/rtl.h"

/* Condition Code */

void rtl_setcc(rtlreg_t* dest, uint8_t subcode) {
  bool invert = subcode & 0x1;
  enum {
    CC_O, CC_NO, CC_B,  CC_NB,
    CC_E, CC_NE, CC_BE, CC_NBE,
    CC_S, CC_NS, CC_P,  CC_NP,
    CC_L, CC_NL, CC_LE, CC_NLE
  };

  // TODO: Query EFLAGS to determine whether the condition code is satisfied.
  // dest <- ( cc is satisfied ? 1 : 0)
  switch (subcode & 0xe) {

    case CC_O:*dest = (uint8_t)cpu.OF;break;//0

    case CC_NO:*dest = !cpu.OF;break;//1

    case CC_B:*dest = cpu.CF;break;//2

    case CC_NB:*dest = !cpu.CF;break;//3

    case CC_E:*dest = cpu.ZF;break;//4

    case CC_NE:*dest = !cpu.ZF;break;//5

    case CC_BE:*dest = (cpu.CF||cpu.ZF);break;//6

    case CC_NBE:*dest = ((!cpu.CF) && (!cpu.ZF));break;//7

    case CC_S:*dest = cpu.SF;break;//8

    case CC_NS:*dest = !cpu.SF;break;//9

    case CC_L:*dest = (cpu.SF!=cpu.OF);break;//C

    case CC_NL:*dest = (cpu.SF==cpu.OF);break;//D

    case CC_LE:*dest = ((cpu.ZF)||(cpu.SF!=cpu.OF));break;//E

    case CC_NLE:*dest = ((!cpu.ZF)&&(cpu.SF==cpu.OF));break;//F

    default: panic("should not reach here");

    case CC_P: panic("n86 does not have PF");
  }

  if (invert) {
    rtl_xori(dest, dest, 0x1);
  }
}
