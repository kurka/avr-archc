#include "avr1_isa.H"
#include "avr1_isa_init.cpp"
#include "avr1_bhv_macros.H"

//#define DEBUG_MODEL
#include "ac_debug_model.H"

using namespace avr1_parms;

struct flags_t {
  bool I, T, H, S, V, N, Z, C;
} flags;
  
void ac_behavior (Type_RegDir_Rr_Rd) {

}

void ac_behavior(instruction) {
  ac_pc = npc;
  npc = ac_pc + 4;
}

void ac_behavior(begin) {
  flags.I = false;
  flags.T = false;
  flags.H = false;
  flags.S = false;
  flags.V = false;
  flags.N = false;
  flags.Z = false;
  flags.C = false;
  
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

  int r = RB[rd_2] + RB[rr];

  RB[rd_2] = RB[rd_2] + RB[rr];

  bool rd3 = RB[rd_2] & (1 << 3) == (1 << 3) ? true : false;
  bool rr3 = RB[rr] & (1 << 3) == (1 << 3) ? true : false;
  bool r3 = r & (1 << 3) == (1 << 3) ? true : false;
  bool rd7 = RB[rd_2] & (1 << 7) == (1 << 7) ? true : false;
  bool rr7 = RB[rr] & (1 << 7) == (1 << 7) ? true : false;
  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;

  flags.H = rd3 & rr3 | rr3 & ~r3 | ~r3 & rd3;
  flags.S = flags.N ^ flags.V;
  flags.V = rd7 & rr7 & ~r7 | ~rd7 & ~rr7 & r7;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;
  flags.C = rd7 & rr7 | rr7 & ~r7 | ~r7 & rd7;

  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(adc) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("adc r%d, r%d", rd_2, rr);

  int carry = 0;
  if (flags.C == true) {
    carry = 1;
  }
  int r = RB[rd_2] + RB[rr];

  RB[rd_2] = RB[rd_2] + RB[rr] + carry;

  bool rd3 = RB[rd_2] & (1 << 3) == (1 << 3) ? true : false;
  bool rr3 = RB[rr] & (1 << 3) == (1 << 3) ? true : false;
  bool r3 = r & (1 << 3) == (1 << 3) ? true : false;
  bool rd7 = RB[rd_2] & (1 << 7) == (1 << 7) ? true : false;
  bool rr7 = RB[rr] & (1 << 7) == (1 << 7) ? true : false;
  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;

  flags.H = rd3 & rr3 | rr3 & ~r3 | ~r3 & rd3;
  flags.S = flags.N ^ flags.V;
  flags.V = rd7 & rr7 & ~r7 | ~rd7 & ~rr7 & r7;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;
  flags.C = rd7 & rr7 | rr7 & ~r7 | ~r7 & rd7;

  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(logical_and) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("and r%d, r%d", rd_2, rr);

  int r = RB[rd_2] & RB[rr];

  RB[rd_2] = RB[rr] & RB[rd_2];

  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;

  flags.S = flags.N ^ flags.V;
  flags.V = false;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;

  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(cp) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("cp r%d, r%d", rd_2, rr);
  int r = RB[rd_2] - RB[rr];
  bool rd3 = RB[rd_2] & (1 << 3) == (1 << 3) ? true : false;
  bool rr3 = RB[rr] & (1 << 3) == (1 << 3) ? true : false;
  bool r3 = r & (1 << 3) == (1 << 3) ? true : false;
  bool rd7 = RB[rd_2] & (1 << 7) == (1 << 7) ? true : false;
  bool rr7 = RB[rr] & (1 << 7) == (1 << 7) ? true : false;
  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;

  flags.H = ~rd3 & rr3 | rr3 & r3 | r3 & ~rd3;
  flags.S = flags.N ^ flags.V;
  flags.V = rd7 & ~rr7 & r7 | ~rd7 & rr7 & r7;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;
  flags.C = false;
  if (std::abs(RB[rr] > std::abs(RB[rd_2])))
    flags.C = true;

  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(cpc) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("cpc r%d, r%d", rd_2, rr);
  int carry = 0;
  if (flags.C == true) {
    carry = 1;
  }
  int r = RB[rd_2] - RB[rr] - carry;
  bool rd3 = RB[rd_2] & (1 << 3) == (1 << 3) ? true : false;
  bool rr3 = RB[rr] & (1 << 3) == (1 << 3) ? true : false;
  bool r3 = r & (1 << 3) == (1 << 3) ? true : false;
  bool rd7 = RB[rd_2] & (1 << 7) == (1 << 7) ? true : false;
  bool rr7 = RB[rr] & (1 << 7) == (1 << 7) ? true : false;
  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;

  flags.H = ~rd3 & rr3 | rr3 & r3 | r3 & ~rd3;
  flags.S = flags.N ^ flags.V;
  flags.V = rd7 & ~rr7 & r7 | ~rd7 & rr7 & r7;
  flags.N = r7;
  flags.Z = r == 0 ? : false;
  flags.C = false;
  if (std::abs(RB[rr] > std::abs(RB[rd_2])))
    flags.C = true;
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(eor) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("eor r%d, r%d", rd_2, rr);
  int r = RB[rr] ^ RB[rd_2];
  RB[rd_2] = RB[rr] ^ RB[rd_2];
  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;
  flags.S = flags.N ^ flags.V;
  flags.V = false;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(logical_or) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("eor r%d, r%d", rd_2, rr);
  int r = RB[rr] | RB[rd_2];
  RB[rd_2] = RB[rr] | RB[rd_2];
  bool r7 = r & (1 << 7) == (1 << 7) ? true : false; 
  flags.S = flags.N ^ flags.V;
  flags.V = false;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;
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

  int carry = 0;
  if (flags.C == true) 
    carry = 1;
  int r = RB[rd_2] - RB[rr] - carry;

  RB[rd_2] = RB[rd_2] - RB[rr];

  bool rd3 = RB[rd_2] & (1 << 3) == (1 << 3) ? true : false;
  bool rr3 = RB[rr] & (1 << 3) == (1 << 3) ? true : false;
  bool r3 = r & (1 << 3) == (1 << 3) ? true : false;
  bool rd7 = RB[rd_2] & (1 << 7) == (1 << 7) ? true : false;
  bool rr7 = RB[rr] & (1 << 7) == (1 << 7) ? true : false;
  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;

  flags.H = ~rd3 & rr3 | rr3 & r3 | r3 & ~rd3;
  flags.S = flags.N ^ flags.V;
  flags.V = rd7 & ~rr7 & ~r7 | ~rd7 & rr7 & r7;
  flags.N = r7;
  flags.Z = r == 0 ? : false;
  flags.C = false;
  if (std::abs(RB[rr] > std::abs(RB[rd_2])))
    flags.C = true;

  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(sub) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("sub r%d, r%d", rd_2, rr);

  int r = RB[rd_2] - RB[rr];

  RB[rd_2] = RB[rd_2] - RB[rr];

  bool rd3 = RB[rd_2] & (1 << 3) == (1 << 3) ? true : false;
  bool rr3 = RB[rr] & (1 << 3) == (1 << 3) ? true : false;
  bool r3 = r & (1 << 3) == (1 << 3) ? true : false;
  bool rd7 = RB[rd_2] & (1 << 7) == (1 << 7) ? true : false;
  bool rr7 = RB[rr] & (1 << 7) == (1 << 7) ? true : false;
  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;

  flags.H = ~rd3 & rr3 | rr3 & r3 | r3 & ~rd3;
  flags.S = flags.N ^ flags.V;
  flags.V = rd7 & ~rr7 & ~r7 | ~rd7 & rr7 & r7;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;
  flags.C = false;
  if (std::abs(RB[rr] > std::abs(RB[rd_2])))
    flags.C = true;

  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(mul) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("add r%d, r%d", rd_2, rr);
  int r = RB[rr] * RB[rd_2];
  int r_0 = r & 0xFF;
  int r_1 = (r & 0xFF00) >> 8;
  RB[1] = r_1;
  RB[0] = r_0;

  flags.C = r & (1 << 15) == (1 << 15) ? true : false;
  flags.Z = r == 0 ? true : false;

  dbg_printf("Result = %#x\n", RB[rd_2]);
}



