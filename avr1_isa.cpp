#include "avr1_isa.H"
#include "avr1_isa_init.cpp"
#include "avr1_bhv_macros.H"

#define DEBUG_MODEL
#include "ac_debug_model.H"

using namespace avr1_parms;

struct flags_t {
  bool I, T, H, S, V, N, Z, C;
} flags;
  
void ac_behavior (Type_RegDir_Rr_Rd) {}
void ac_behavior (Type_RegDir_Rd) {}
void ac_behavior (Type_RegDir_Rd_Imm) {}
void ac_behavior (Type_Branches) {}
void ac_behavior (Type_Relative_Program_Addressing) {}

void ac_behavior(instruction) {
  ac_pc = npc;
  npc = npc + 2;

  //  dbg_printf("----- PC=%#x ----- %lld\n", (int) ac_pc, ac_instr_counter);
  dbg_printf("----- size: %d ---  %d ----- PC=%#x NPC=%#x ----- %lld\n", get_size(), DM.read(ac_instr_counter), (int) ac_pc, (int)npc, ac_instr_counter);

  // ac_pc = 0x54;
  // npc = 0x54;
}

void ac_behavior(begin) {

  dbg_printf("begin!");

  flags.I = false;
  flags.T = false;
  flags.H = false;
  flags.S = false;
  flags.V = false;
  flags.N = false;
  flags.Z = false;
  flags.C = false;
  
  RB[0] = 0;
  // ac_pc = 0x54;
  npc = ac_pc + 2;
  
  for (int regNum = 0; regNum < 32; regNum++) {
    RB[regNum] = 0;
  }
}

void ac_behavior(end) {
  
}

void ac_behavior(add) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);


  dbg_printf("add r%d, r%d\n", rd_2, rr);
  dbg_printf("r1:%d, r2:%d\n", r1, r2);
  dbg_printf("rd:%d\n", rd_2);
  dbg_printf("op:%d\n", op_2);

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
  dbg_printf("adc r%d, r%d\n", rd_2, rr);

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
  dbg_printf("and r%d, r%d\n", rd_2, rr);

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
  dbg_printf("cp r%d, r%d\n", rd_2, rr);
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
  dbg_printf("cpc r%d, r%d\n", rd_2, rr);
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
  dbg_printf("eor r%d, r%d\n", rd_2, rr);
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
  dbg_printf("eor r%d, r%d\n", rd_2, rr);
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
  dbg_printf("mov r%d, r%d\n", rd_2, rr);
  RB[rd_2] = RB[rr];
  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(sbc) {
  unsigned rr = (r1 << 4) + (r2 & 0xF);
  dbg_printf("sbc r%d, r%d\n", rd_2, rr);

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
  dbg_printf("sub r%d, r%d\n", rd_2, rr);

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
  dbg_printf("mul r%d, r%d\n", rd_2, rr);

  dbg_printf("r1:%d, r2:%d\n", r1, r2);
  dbg_printf("rd:%d\n", rd_2);
  dbg_printf("op:%d\n", op_2);

  int r = RB[rr] * RB[rd_2];
  int r_0 = r & 0xFF;
  int r_1 = (r & 0xFF00) >> 8;
  RB[1] = r_1;
  RB[0] = r_0;

  flags.C = r & (1 << 15) == (1 << 15) ? true : false;
  flags.Z = r == 0 ? true : false;

  dbg_printf("Result = %#x\n", RB[rd_2]);
}

void ac_behavior(clr) {
  dbg_printf("clr r%d\n", rd_1);
  RB[rd_1] = RB[rd_1] ^ RB[rd_1];

  flags.S = false;
  flags.V = false;
  flags.N = false;
  flags.Z = true;

  dbg_printf("Result = %#x\n", RB[rd_1]);
}

void ac_behavior(tst) {
  dbg_printf("tst r%d\n", rd_1);

  int r = RB[rd_1] & RB[rd_1];

  RB[rd_1] = RB[rd_1] & RB[rd_1];

  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;
  flags.S = flags.N ^ flags.V;
  flags.V = false;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;

  dbg_printf("Result = %#x\n", RB[rd_1]);
}

void ac_behavior(lsl) {
  dbg_printf("lsl r%d\n", rd_1);

  int r = RB[rd_1];
  bool rd7 = r & (1 << 7) == (1 << 7) ? true : false;
  flags.C = rd7;
  bool rd3 = RB[rd_1] & (1 << 3) == (1 << 3) ? true : false;
  bool r7 = r & (1 << 6) == (1 << 6) ? true : false;

  RB[rd_1] = RB[rd_1] << 1;

  flags.H = rd3;
  flags.S = flags.N ^ flags.V;
  flags.V = flags.N ^ flags.C;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;

  dbg_printf("Result = %#x\n", RB[rd_1]);
}

void ac_behavior(rol) {
  dbg_printf("rol r%d\n", rd_1);
  
  int old_carry = flags.C == true ? 1 : 0;
  int r = RB[rd_1];
  bool rd7 = r & (1 << 7) == (1 << 7) ? true : false;
  flags.C = rd7;
  bool rd3 = RB[rd_1] & (1 << 3) == (1 << 3) ? true : false;
  bool r7 = r & (1 << 6) == (1 << 6) ? true : false;

  RB[rd_1] = RB[rd_1] << 1;
  RB[rd_1] = RB[rd_1] & old_carry;

  flags.H = rd3;
  flags.S = flags.N ^ flags.V;
  flags.V = flags.N ^ flags.C;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;

  dbg_printf("Result = %#x\n", RB[rd_1]);
}

void ac_behavior(rjmp) {
  
  // int k1 = (k_7 & 0xF00) >> 8;
  // int k2 = (k_7 & 0x0FF) << 4;

  // int j = (k1 + k2) << 1;
  int j = k_7;

  dbg_printf("rjmp %x\n", j);
  dbg_printf("op: %x\n", op_7);

  if ( (j & (1<<11) ) == (1<<11) ) {
    j = ( (~j) + 1) & 0xFFF;
    j = -j;
  }

  ac_pc = ac_pc + j;
  npc = ac_pc + 2;

  dbg_printf("Result = %#x %d\n", (int)ac_pc, j);
}

void ac_behavior(breq) {
  dbg_printf("breq %d\n", k_8);
  
  if (flags.Z) {
    ac_pc = npc;
    npc = ac_pc + k_8;
  }

  dbg_printf("Result = %#x\n", k_8);
}

void ac_behavior(brge) {
  dbg_printf("brge %d\n", k_8);
  
  if (!(flags.N ^ flags.V)) {
    ac_pc = npc;
    npc = ac_pc + k_8;
  }

  dbg_printf("Result = %#x\n", k_8);
}

void ac_behavior(brlo) {
  dbg_printf("brlo %d\n", k_8);
  
  if (flags.C) {
    ac_pc = npc;
    npc = ac_pc + k_8;
  }

  dbg_printf("Result = %#x\n", k_8);
}

void ac_behavior(brlt) {
  dbg_printf("brlt %d\n", k_8);
  
  if (flags.N ^ flags.V) {
    ac_pc = npc;
    npc = ac_pc + k_8;
  }

  dbg_printf("Result = %#x\n", k_8);
}

void ac_behavior(brsh) {
  dbg_printf("brsh %d\n", k_8);
  
  if (!flags.C) {
    ac_pc = npc;
    npc = ac_pc + k_8;
  }

  dbg_printf("Result = %#x\n", k_8);
}

void ac_behavior(brne) {
  dbg_printf("brne %d\n", k_8);
  
  if (!flags.Z) {
    ac_pc = npc;
    npc = ac_pc + k_8;
  }

  dbg_printf("Result = %#x\n", k_8);
}

void ac_behavior(andi) {
  unsigned k = (k1 << 4) + (k2 & 0xF);
  dbg_printf("andi r%d, %d\n", rd_3, k);
  
  int r = RB[rd_3] & k;
  RB[rd_3] =  RB[rd_3] & k;

  bool r7 = r & (1 << 7) == (1 << 7) ? true : false;

  flags.S = flags.N ^ flags.V;
  flags.V = false;
  flags.N = r7;
  flags.Z = r == 0 ? true : false;

  dbg_printf("Result = %#x\n", RB[rd_3]);
}

void ac_behavior(ldi) {
  unsigned k = (k1 << 4) + (k2 & 0xF);
  dbg_printf("ldi r%d, %d\n", rd_3, k);
  
  RB[rd_3] = k;

  dbg_printf("Result = %#x\n", RB[rd_3]);
}

// void ac_behavior(andi) {
//   unsigned k = (k1 << 4) + (k2 & 0xF);
//   dbg_printf("andi r%d, %d\n", rd_3, k);
  
//   int r = RB[rd_3] & k;
//   RB[rd_3] =  RB[rd_3] & k;

//   bool r7 = r & (1 << 7) == (1 << 7) ? true : false;

//   flags.S = flags.N ^ flags.V;
//   flags.V = false;
//   flags.N = r7;
//   flags.Z = r == 0 ? true : false;

//   dbg_printf("Result = %#x\n", RB[rd_3]);
// }
