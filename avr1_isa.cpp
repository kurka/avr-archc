#include "avr1_isa.H"
#include "avr1_isa_init.cpp"
#include "avr1_bhv_macros.H"

//#define DEBUG_MODEL
#include "ac_debug_model.H"

using namespace avr1_parms;

void ac_behavior (Type_RegDir_Rr_Rd) {

}

void ac_behavior(instruction) {
  ac_pc = npc;
  npc = ac_pc + 4;
}

void ac_behavior(begin) {
  RB[0] = 0;
  npc = ac_pc + 4;
  
  for (int regNum = 0; regNum < 32; regNum++) {
    RB[regNum] = 0;
  }
  hi = 0; lo = 0;
}

void ac_behavior(end) {
  
}

void ac_behavior(add) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("add r%d, r%d", rd_2, rr);
  RB[rd_2] = RB[rr] + RB[rd_2];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(adc) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("adc r%d, r%d", rd_2, rr);
  RB[rd_2] = RB[rr] + RB[rd_2];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(logical_and) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("and r%d, r%d", rd_2, rr);
  RB[rd_2] = RB[rr] & RB[rd_2];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

// void ac_behavior(cp) {
//   unsigned rr = (r1 << 4) + (r2 & 0xF);
//   dbg_printf("adc r%d, r%d", rd_2, rr);
//   RB[rd_2] = RB[rr] + RB[rd_2];
//   dbg_printf("Result = %#x\n", RB[rd_2]);
// }

// void ac_behavior(cpc) {
//   unsigned rr = (r1 << 4) + (r2 & 0xF);
//   dbg_printf("adc r%d, r%d", rd_2, rr);
//   RB[rd_2] = RB[rr] + RB[rd_2];
//   dbg_printf("Result = %#x\n", RB[rd_2]);
// }

void ac_behavior(eor) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("eor r%d, r%d", rd_2, rr);
  RB[rd_2] = RB[rr] ^ RB[rd_2];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(or) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("eor r%d, r%d", rd_2, rr);
  RB[rd_2] = RB[rr] | RB[rd_2];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(mov) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("mov r%d, r%d", rd_2, rr);
  RB[rd_2] = RB[rr];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(sbc) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("sbc r%d, r%d", rd_2, rr);
  RB[rd_2] = RB[rd_2] - RB[rr];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(sub) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("sub r%d, r%d", rd_2, rr);
  RB[rd_2] = RB[rd_2] - RB[rr];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}


// arrumar isso!!!
void ac_behavior(mul) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("add r%d, r%d", rd_2, rr);
  RB[rd_2] = RB[rr] * RB[rd_2];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}



