#include "avr1_isa.H"
#include "avr1_isa_init.cpp"
#include "avr1_bhv_macros.H"

//#define DEBUG_MODEL
#include "ac_debug_model.H"

using namespace avr1_parms;

void ac_behavior(add) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("add r%d, r%d", rd, rr);
  RB[rd] = RB[rr] + RB[rd];
  dbg_printf("Result = %#x\n", RB[rd]);
}

void ac_behavior(adc) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("adc r%d, r%d", rd, rr);
  RB[rd] = RB[rr] + RB[rd];
  dbg_printf("Result = %#x\n", RB[rd]);
}

void ac_behavior(logical_and) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("and r%d, r%d", rd, rr);
  RB[rd] = RB[rr] & RB[rd];
  dbg_printf("Result = %#x\n", RB[rd]);
}

// void ac_behavior(cp) {
//   unsigned rr = (r1 << 4) + (r2 & 0xF);
//   dbg_printf("adc r%d, r%d", rd, rr);
//   RB[rd] = RB[rr] + RB[rd];
//   dbg_printf("Result = %#x\n", RB[rd]);
// }

// void ac_behavior(cpc) {
//   unsigned rr = (r1 << 4) + (r2 & 0xF);
//   dbg_printf("adc r%d, r%d", rd, rr);
//   RB[rd] = RB[rr] + RB[rd];
//   dbg_printf("Result = %#x\n", RB[rd]);
// }

void ac_behavior(eor) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("eor r%d, r%d", rd, rr);
  RB[rd] = RB[rr] ^ RB[rd];
  dbg_printf("Result = %#x\n", RB[rd]);
}

void ac_behavior(mov) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("mov r%d, r%d", rd, rr);
  RB[rd] = RB[rr];
  dbg_printf("Result = %#x\n", RB[rd]);
}

void ac_behavior(sbc) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("sbc r%d, r%d", rd, rr);
  RB[rd] = RB[rd] - RB[rr];
  dbg_printf("Result = %#x\n", RB[rd]);
}

void ac_behavior(sub) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("sub r%d, r%d", rd, rr);
  RB[rd] = RB[rd] - RB[rr];
  dbg_printf("Result = %#x\n", RB[rd]);
}

