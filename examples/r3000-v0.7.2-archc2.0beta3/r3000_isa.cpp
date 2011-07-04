/**
 * @file      r3000_isa.cpp
 * @author    Marcio Juliato
 *            Alexandro Baldassin
 *
 *            The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br
 *
 * @version   1.0
 * @date      Mon, 19 Jun 2006 15:50:49 -0300
 * 
 * @brief     The ArchC R3000 cycle-accurate model.
 * 
 * @attention Copyright (C) 2002-2006 --- The ArchC Team
 * 
 * This program is free software; you can redistribute it and/or modify 
 * it under the terms of the GNU General Public License as published by 
 * the Free Software Foundation; either version 2 of the License, or 
 * (at your option) any later version. 
 * 
 * This program is distributed in the hope that it will be useful, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 * GNU General Public License for more details. 
 * 
 * You should have received a copy of the GNU General Public License 
 * along with this program; if not, write to the Free Software 
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "r3000_isa.H"
#include "r3000_isa_init.cpp"
#include "r3000_bhv_macros.H"
#include "r3000_G_IF.H"
#include "r3000_G_ID.H"
#include "r3000_G_EX.H"
#include "r3000_G_MEM.H"
#include "r3000_G_WB.H"
#include <stdio.h>

using namespace r3000_parms;

//!User defined macros to reference registers.
#define Ra 31
#define Hi 32
#define Lo 33

/************************************************************************/
//! Common pos execution function for all I Type instructions.
// Copy the pipeline registers contents from the EX_MEM to the MEM_WB
#define TYPE_I_MEM_WB_REG_COPY \
    MEM_WB.regwrite = EX_MEM.regwrite;\
    MEM_WB.rdest = EX_MEM.rdest;\
    MEM_WB.wbdata = EX_MEM.alures
/************************************************************************/

/************************************************************************/
void ac_behavior(begin)
{
 return;
}
/************************************************************************/

/************************************************************************/
void ac_behavior(end)
{
 return;
}
/************************************************************************/

/************************************************************************/
//! Generic instruction behavior method.
void ac_behavior(instruction)
{
 switch (stage)
 {
  case id_G_IF:
   IF_ID.npc = ac_pc + 4; 
   ac_pc += 4;
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   /* Execute write back when allowed */
   if (MEM_WB.regwrite == 1)
   {
    // Register 0 is never written
    if (MEM_WB.rdest != 0)
     RB.write(MEM_WB.rdest, MEM_WB.wbdata);
   }
   break;
  default:
   break;
 }
 return;
}
/************************************************************************/

/************************************************************************/
//! Common behaviour for all R Type instructions
void ac_behavior(Type_R)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Assuming that all R type instructions execute write back. When it is not necessary,
      the instruction BY ITSELF must clear the regwrite bit on its ID stage */
   ID_EX.regwrite = 1;
   ID_EX.memread = 0;
   ID_EX.memwrite = 0;
   ID_EX.npc = IF_ID.npc;
   ID_EX.rs = rs;
   ID_EX.rt = rt;
   ID_EX.rd = rd;
   /* Checking forwarding for the rs register */
   if ((EX_MEM.regwrite == 1) &&
       (EX_MEM.rdest != 0) &&
       (EX_MEM.rdest == rs)) // There was a ID_EX before the rs here, but it's gone now.
    id_value1 = EX_MEM.alures.read();
   else if ((MEM_WB.regwrite == 1) &&
            (MEM_WB.rdest != 0) &&
            (MEM_WB.rdest == rs)) // There was a ID_EX before the rs here, but it's gone now.
    id_value1 = MEM_WB.wbdata.read();
   else
    id_value1 = RB.read(rs);
   /* Checking forwarding for the rt register */
   if ((EX_MEM.regwrite == 1) &&
       (EX_MEM.rdest != 0) &&
       (EX_MEM.rdest == rt)) // There was a ID_EX before the rt here, but it's gone now.
    id_value2 = EX_MEM.alures.read();
   else if ((MEM_WB.regwrite == 1) &&
            (MEM_WB.rdest != 0) &&
            (MEM_WB.rdest == rt)) // There was a ID_EX before the rt here, but it's gone now.
    id_value2 = MEM_WB.wbdata.read();
   else
    id_value2 = RB.read(rt);
   ID_EX.data1 = id_value1;
   ID_EX.data2 = id_value2;
   break;
  case id_G_EX:
   /* Checking forwarding for the rs register */
   if ((EX_MEM.regwrite == 1) &&
       (EX_MEM.rdest != 0) &&
       (EX_MEM.rdest == ID_EX.rs))
    ex_value1 = EX_MEM.alures;
   else if ((MEM_WB.regwrite == 1) &&
            (MEM_WB.rdest != 0) &&
            (MEM_WB.rdest == ID_EX.rs))
    ex_value1 = MEM_WB.wbdata;
   else
    ex_value1 = ID_EX.data1;
   /* Checking forwarding for the rt register */
   if ((EX_MEM.regwrite == 1) &&
       (EX_MEM.rdest != 0) &&
       (EX_MEM.rdest == ID_EX.rt))
    ex_value2 = EX_MEM.alures;
   else if ((MEM_WB.regwrite == 1) &&
            (MEM_WB.rdest != 0) &&
            (MEM_WB.rdest == ID_EX.rt))
    ex_value2 = MEM_WB.wbdata;
   else
    ex_value2 = ID_EX.data2;
   break;
  case id_G_MEM:
   /* Just copies the values from EX_MEM to MEM_WB */
   MEM_WB.regwrite = EX_MEM.regwrite;
   MEM_WB.wbdata = EX_MEM.alures;
   MEM_WB.rdest = EX_MEM.rdest;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}
/************************************************************************/

/************************************************************************/
//! Common behaviour for all I Type instructions
void ac_behavior(Type_I)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Assuming that all instructions execute write back. When it is not necessary,
      the instruction must clear the regwrite bit on its EX stage */
   ID_EX.regwrite = 1;
   ID_EX.memread = 0;
   ID_EX.memwrite = 0;
   ID_EX.npc = IF_ID.npc;
   ID_EX.rs = rs;
   ID_EX.rt = rt;
   ID_EX.imm = imm;
   /* Checking forwarding for the rs register */
   if ((EX_MEM.regwrite == 1) &&
       (EX_MEM.rdest != 0) &&
       (EX_MEM.rdest == rs)) // There was a ID_EX before the rs here, but it's gone now.
    id_value1 = EX_MEM.alures;
   else if ((MEM_WB.regwrite == 1) &&
            (MEM_WB.rdest != 0) &&   // Register 0 is never written
            (MEM_WB.rdest == rs)) // There was a ID_EX before the rs here, but it's gone now.
    id_value1 = MEM_WB.wbdata;
   else
    id_value1 = RB.read(rs);
   /* Checking forwarding for the rt register (branch instructions) */
   if ((EX_MEM.regwrite == 1) &&
       (EX_MEM.rdest != 0) &&
       (EX_MEM.rdest == rt)) // There was a ID_EX before the rt here, but it's gone now.
    id_value2 = EX_MEM.alures;
   else if ((MEM_WB.regwrite == 1) &&
            (MEM_WB.rdest != 0) &&
            (MEM_WB.rdest == rt)) // There was a ID_EX before the rt here, but it's gone now.
    id_value2 = MEM_WB.wbdata;
   else
    id_value2 = RB.read(rt);
   ID_EX.data1 = id_value1;
   ID_EX.data2 = id_value2;
   break;
  case id_G_EX:
   /* Checking forwarding for the rs register */
   if ((EX_MEM.regwrite == 1) &&
       (EX_MEM.rdest != 0) &&       // Register 0 is never written
       (EX_MEM.rdest == ID_EX.rs))
    ex_value1 = EX_MEM.alures;
   else if ((MEM_WB.regwrite == 1) &&
            (MEM_WB.rdest != 0) &&  // Register 0 is never written
            (MEM_WB.rdest == ID_EX.rs))
    ex_value1 = MEM_WB.wbdata;
   else
    ex_value1 = ID_EX.data1;
   /* Checking forwarding for the rt register (branch instructions) */
   if ((EX_MEM.regwrite == 1) &&
       (EX_MEM.rdest != 0) &&
       (EX_MEM.rdest == ID_EX.rt))
    ex_value2 = EX_MEM.alures;
   else if ((MEM_WB.regwrite == 1) &&
            (MEM_WB.rdest != 0) &&
            (MEM_WB.rdest == ID_EX.rt))
    ex_value2 = MEM_WB.wbdata;
   else
    ex_value2 = ID_EX.data2;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}
/************************************************************************/

/************************************************************************/
//! Common behaviour for all J Type instructions
void ac_behavior(Type_J)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* None of the J Type instructions execute write back */
   ID_EX.regwrite = 0;
   /* Neither access memory */
   ID_EX.memread = 0;
   ID_EX.memwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   MEM_WB.wbdata = EX_MEM.alures;
   MEM_WB.regwrite = EX_MEM.regwrite;
   MEM_WB.rdest = EX_MEM.rdest;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}
/************************************************************************/

/************************************************************************/
//! Instruction lb behavior method.
void ac_behavior(lb)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   ID_EX.memread = 1;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   char byte;
   /* Copy the pipeline registers */
   TYPE_I_MEM_WB_REG_COPY;
   byte = DM.read_byte(EX_MEM.alures);
   MEM_WB.wbdata = (ac_Sword) byte;
   break;
  case id_G_WB:
   /* It was implemented for all instructions on ac_behavior(instruction) */
   break;
  default:
   break;
 }
 return;
}

//! Instruction lbu behavior method.
void ac_behavior(lbu)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   ID_EX.memread = 1;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   unsigned char byte;
   TYPE_I_MEM_WB_REG_COPY;
   byte = DM.read_byte(EX_MEM.alures);
   MEM_WB.wbdata = byte;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction lh behavior method.
void ac_behavior(lh)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   ID_EX.memread = 1;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   short int half;
   TYPE_I_MEM_WB_REG_COPY;
   half = DM.read_half(EX_MEM.alures);
   MEM_WB.wbdata = (ac_Sword) half;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction lhu behavior method.
void ac_behavior(lhu)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   ID_EX.memread = 1;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   unsigned short int half;
   TYPE_I_MEM_WB_REG_COPY;
   half = DM.read_half(EX_MEM.alures);
   MEM_WB.wbdata = half;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction lw behavior method.
void ac_behavior(lw)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Reads from memory */
   ID_EX.memread = 1;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   MEM_WB.wbdata = DM.read(EX_MEM.alures);
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction lwl behavior method.
void ac_behavior(lwl)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   ID_EX.memread = 1;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   // Bypassing the contents of the register rt within the processor through the wdata register
   EX_MEM.wdata = ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   unsigned int addr, offset;
   ac_Uword data;
   ac_Uword rdest_value;
   // Data fowarding to the rt register while in Memory stage
   // This is necessary to get the right value when using consecutive (lwl and lwr) intructions.
   if ((MEM_WB.regwrite == 1) &&
       (MEM_WB.rdest != 0) &&
       (MEM_WB.rdest == EX_MEM.rdest))
    // Gets the most actual rt value -- NEM: most -actual-???
    rdest_value = MEM_WB.wbdata;
   else
    // Use the value (previously) read from the register bank
    rdest_value = EX_MEM.wdata;
   TYPE_I_MEM_WB_REG_COPY;
   // address
   addr = EX_MEM.alures;
   // offset
   offset = (addr & 0x3) * 8;
   // read the whole 32-bit data starting at the word alignment
   data = DM.read(addr & 0xfffffffc);
   // shift it offset bits to the left
   data <<= offset;
   // mask the necessary bits 
   data |= (rdest_value & ((1 << offset) - 1));
   MEM_WB.wbdata = data;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction lwr behavior method.
void ac_behavior(lwr)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   ID_EX.memread = 1;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   // Bypassing the contents of the register rt within the processor through the wdata register
   EX_MEM.wdata = ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   unsigned int addr, offset;
   ac_Uword data;
   ac_Uword rdest_value;
   // Data fowarding to the rt register while in Memory stage
   // This is necessary to get the right value when using consecutive (lwl and lwr) intructions.
   if ((MEM_WB.regwrite == 1) &&
       (MEM_WB.rdest != 0) &&
       (MEM_WB.rdest == EX_MEM.rdest))
    // Gets the most actual rt value
    rdest_value = MEM_WB.wbdata;
   else
    // Use the value (previously) read from the register bank
    rdest_value = EX_MEM.wdata;
   TYPE_I_MEM_WB_REG_COPY;
   // address
   addr = EX_MEM.alures;
   // offset
   offset = (3 - (addr & 0x3)) * 8;
   // read the whole 32-bit data starting at the word alignment
   data = DM.read(addr & 0xfffffffc);
    // shift it offset bits to the right
   data >>= offset;
   // mask the necessary bits
   data |= rdest_value & (0xffffffff << (32 - offset));
   MEM_WB.wbdata = data;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sb behavior method.
void ac_behavior(sb)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   ID_EX.memwrite = 1;
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.wdata = ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   unsigned char byte;
   byte = EX_MEM.wdata & 0xff;
   DM.write_byte(EX_MEM.alures, byte);
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sh behavior method.
void ac_behavior(sh)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   ID_EX.memwrite = 1;
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.wdata = ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;    
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   unsigned short int half;
   half = EX_MEM.wdata & 0xffff;
   DM.write_half(EX_MEM.alures, half);
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sw behavior method.
void ac_behavior(sw)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Writes to memory */
   ID_EX.memwrite = 1;
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.wdata = ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   DM.write(EX_MEM.alures, EX_MEM.wdata);
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction swl behavior method.
void ac_behavior(swl)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Writes to memory */
   ID_EX.memwrite = 1;
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.wdata = ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   unsigned int addr, offset;
   ac_Uword data;
   // address
   addr = EX_MEM.alures;
   // offset
   offset = (addr & 0x3) * 8;
   data = EX_MEM.wdata;
   data >>= offset;
   data |= (DM.read(addr & 0xfffffffc) & (0xffffffff << (32 - offset)));
   DM.write((addr & 0xfffffffc), data);
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction swr behavior method.
void ac_behavior(swr)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Writes to memory */
   ID_EX.memwrite = 1;
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.wdata = ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   unsigned int addr, offset;
   ac_Uword data;
    // address
   addr = EX_MEM.alures;
   // offset
   offset = (3 - (addr & 0x3)) * 8;
   data = EX_MEM.wdata;
   data <<= offset;
   data |= (DM.read(addr & 0xfffffffc) & ((1 << offset) - 1));
   DM.write((addr & 0xfffffffc), data);
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction addi behavior method.
void ac_behavior(addi)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // remember that ID_EX.imm is already a 32-bit sign extended register
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   // overflow checking
   if (((ex_value1 & 0x80000000) == (ID_EX.imm & 0x80000000)) &&
       ((ID_EX.imm & 0x80000000) != (ex_value2 & 0x80000000)))
   {
    fprintf(stderr, "EXCEPTION(addi): integer overflow.\n");
    exit(EXIT_FAILURE);
   }
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction addiu behavior method.
void ac_behavior(addiu)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // remember that ID_EX.imm is already a 32-bit sign extended register
   EX_MEM.alures = ex_value1 + ID_EX.imm;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
 default:
   break;
 }
 return;
}

//! Instruction slti behavior method.
void ac_behavior(slti)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // Set the RD if RS < IMM
   if ((ac_Sword) ex_value1 < (ac_Sword) ID_EX.imm)
    EX_MEM.alures = 1;
   // Else reset RD
   else
    EX_MEM.alures = 0;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sltiu behavior method.
void ac_behavior(sltiu)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // Set the RD if RS < IMM
   if ((ac_Uword) ex_value1 < (ac_Uword) ID_EX.imm)
    EX_MEM.alures = 1;
   // Else reset RD
   else
    EX_MEM.alures = 0;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction andi behavior method.
void ac_behavior(andi)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // the immediate is zero extended
   EX_MEM.alures = ex_value1 & (ID_EX.imm & 0xffff);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction ori behavior method.
void ac_behavior(ori)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // the immediate is zero extended
   EX_MEM.alures = ex_value1 | (ID_EX.imm & 0xffff);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction xori behavior method.
void ac_behavior(xori)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // the immediate is zero extended
   EX_MEM.alures = ex_value1 ^ (ID_EX.imm & 0xffff);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction lui behavior method.
void ac_behavior(lui)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // Load a constant in the upper 16 bits of a register
   // To achieve the desired behaviour, the constant was shifted 16 bits left
   // and moved to the target register (rt)
   EX_MEM.alures = ID_EX.imm << 16;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rt;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction add behavior method.
void ac_behavior(add)
{
 switch (stage)
 {
  case id_G_IF:
   /* It was implemented for all instructions on ac_behaviour(instruction) */
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ex_value2;
   // Overflow test
   if (((ex_value1 & 0x80000000) == (EX_MEM.alures & 0x80000000)) &&
       ((EX_MEM.alures & 0x80000000) != (ex_value2 & 0x80000000)))
   {
    fprintf(stderr, "EXCEPTION(add): integer overflow.\n");
    exit(EXIT_FAILURE);
   }
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction addu behavior method.
void ac_behavior(addu)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 + ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sub behavior method.
void ac_behavior(sub)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 - ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction subu behavior method.
void ac_behavior(subu)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 - ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction slt behavior method.
void ac_behavior(slt)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // Set the RD if RS < RT
   if ((ac_Sword) ex_value1 < (ac_Sword) ex_value2)
    EX_MEM.alures = 1;
   // Else reset RD
   else
     EX_MEM.alures = 0;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sltu behavior method.
void ac_behavior(sltu)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   // Set the RD if RS < RT
   if (ex_value1 < ex_value2)
     EX_MEM.alures = 1;
   // Else reset RD
   else
    EX_MEM.alures = 0;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction instr_and behavior method.
void ac_behavior(instr_and)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 & ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction instr_or behavior method.
void ac_behavior(instr_or)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 | ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction instr_xor behavior method.
void ac_behavior(instr_xor)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value1 ^ ex_value2;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction nor behavior method.
void ac_behavior(instr_nor)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ~(ex_value1 | ex_value2);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sll behavior method.
void ac_behavior(sll)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value2 << shamt;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction srl behavior method.
void ac_behavior(srl)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value2 >> shamt;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sra behavior method.
void ac_behavior(sra)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = (ac_Sword) ex_value2 >> shamt;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sllv behavior method.
void ac_behavior(sllv)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value2 << (ex_value1 & 0x1f);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction srlv behavior method.
void ac_behavior(srlv)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = ex_value2 >> (ex_value1 & 0x1f);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction srav behavior method.
void ac_behavior(srav)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = (ac_Sword) ex_value2 >> (ex_value1 & 0x1f);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction mult behavior method.
void ac_behavior(mult)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   long long result;
   int half_result;
   result = (ac_Sword) ex_value1;
   result *= (ac_Sword) ex_value2;
   half_result = (result & 0xffffffff);
   // Register LO receives 32 less significant bits
   RB.write(Lo, half_result);
   half_result = ((result >> 32) & 0xffffffff);
   // Register HI receives 32 most significant bits
   RB.write(Hi, half_result);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction multu behavior method.
void ac_behavior(multu)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   unsigned long long result;
   unsigned int half_result;
   result = ex_value1;
   result *= ex_value2;
   half_result = (result & 0xffffffff);
   // Register LO receives 32 less significant bits
   RB.write(Lo, half_result);
   half_result = ((result >> 32) & 0xffffffff);
   // Register HI receives 32 most significant bits
   RB.write(Hi, half_result);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction div behavior method.
void ac_behavior(div)
{
  switch (stage)
  {
  case id_G_IF:
   break;
  case id_G_ID:
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   // Register LO receives 32 less significant bits
   RB.write(Lo, ((ac_Sword) ex_value1 / (ac_Sword) ex_value2));
   // Register HI receives 32 most significant bits
   RB.write(Hi, ((ac_Sword) ex_value1 % (ac_Sword) ex_value2));
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction divu behavior method.
void ac_behavior(divu)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   // Register LO receives 32 less significant bits
   RB.write(Lo, (ex_value1 / ex_value2));
   // Register HI receives 32 most significant bits
   RB.write(Hi, (ex_value1 % ex_value2));
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction mfhi behavior method.
void ac_behavior(mfhi)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = RB.read(Hi);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction mthi behavior method.
void ac_behavior(mthi)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   RB.write(Hi, ex_value1);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction mflo behavior method.
void ac_behavior(mflo)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   EX_MEM.alures = RB.read(Lo);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction mtlo behavior method.
void ac_behavior(mtlo)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   RB.write(Lo, ex_value1);
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction j behavior method.
void ac_behavior(j)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   addr = addr << 2;
   ac_pc = (IF_ID.npc & 0xf0000000) | addr;
   break;
  case id_G_EX:
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction jal behavior method.
void ac_behavior(jal)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   addr = addr << 2;
   ac_pc = (IF_ID.npc & 0xf0000000) | addr;
   // Enable to write the value in RA
   ID_EX.regwrite = 1;
   ID_EX.npc = IF_ID.npc;
   break;
  case id_G_EX:
   // Calculate the value of PC + MEMORY_INC (return address) to be stored in $ra ($31)
   EX_MEM.alures = ID_EX.npc + 4;
   // Indicate that Register RA will receive the return address
   EX_MEM.rdest = Ra;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction jr behavior method.
void ac_behavior(jr)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rt == rs))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && (EX_MEM.rdest == rs)))
   {
    G_ID.stall();
    IF_ID.suspend();
   }
   else
    // Jump to the address stored on the register reg[RS]
    ac_pc = id_value1;
   // This instruction doesn't execute write back
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction jalr behavior method.
void ac_behavior(jalr)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rt == rs))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && (EX_MEM.rdest == rs)))
   {
    G_ID.stall();
    IF_ID.suspend();
    ID_EX.regwrite = 0;
   }
   else
   {
    // Jumps to the address contained in general register $rs
    ac_pc = id_value1;
    // If rd is not defined, use default
    if (rd == 0)
     rd = Ra;
    // Pass the number of the target value to the next stage
    ID_EX.rd = rd;
    // Enable write in the target register
    ID_EX.regwrite = 1;
    ID_EX.npc = IF_ID.npc;
   }
   break;
  case id_G_EX:
   // Calculate the value of PC + MEMORY_INC (return address) to be stored in the target register
   EX_MEM.alures = ID_EX.npc + 4;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   EX_MEM.rdest = ID_EX.rd;
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction beq behavior method.
void ac_behavior(beq)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rd == rt) || (ID_EX.rt == rs) || (ID_EX.rt == rt))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && ((EX_MEM.rdest == rs) || (EX_MEM.rdest == rt))))
   {
    G_ID.stall();
    IF_ID.suspend();
   }
   else if (id_value1 == id_value2)
    ac_pc = IF_ID.npc + (imm << 2);
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction bne behavior method.
void ac_behavior(bne)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rd == rt) || (ID_EX.rt == rs) || (ID_EX.rt == rt))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && ((EX_MEM.rdest == rs) || (EX_MEM.rdest == rt))))
   {
    G_ID.stall();
    IF_ID.suspend();
   }
   else if (id_value1 != id_value2)
    ac_pc = IF_ID.npc + (imm << 2);
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction blez behavior method.
void ac_behavior(blez)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rt == rs))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && (EX_MEM.rdest == rs)))
   {
    G_ID.stall();
    IF_ID.suspend();
   }
   else if ((id_value1 == 0) || (id_value1 & 0x80000000))
    ac_pc = IF_ID.npc + (imm << 2);
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction bgtz behavior method.
void ac_behavior(bgtz)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rt == rs))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && (EX_MEM.rdest == rs)))
   {
    G_ID.stall();
    IF_ID.suspend();
   }
   else if (!(id_value1 & 0x80000000) && (id_value1 != 0))
    ac_pc = IF_ID.npc + (imm << 2);
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction bltz behavior method.
void ac_behavior(bltz)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rt == rs))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && (EX_MEM.rdest == rs)))
   {
    G_ID.stall();
    IF_ID.suspend();
   }
   else if (id_value1 & 0x80000000)
    ac_pc = IF_ID.npc + (imm << 2);
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
  }
 return;
}

//!Instruction bgez behavior method.
void ac_behavior(bgez)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rt == rs))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && (EX_MEM.rdest == rs)))
   {
    G_ID.stall();
    IF_ID.suspend();
   }
   else if (!(id_value1 & 0x80000000))
    ac_pc = IF_ID.npc + (imm << 2);
   /* This instruction doesn't execute write back */
   ID_EX.regwrite = 0;
   break;
  case id_G_EX:
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}


//! Instruction bltzal behavior method.
void ac_behavior(bltzal)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rt == rs))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && (EX_MEM.rdest == rs)))
   {
    G_ID.stall();
    IF_ID.suspend();
    ID_EX.regwrite = 0;
   }
   else if (id_value1 & 0x80000000)
   {
    ac_pc = IF_ID.npc + (imm << 2);
    ID_EX.regwrite = 1;
    ID_EX.npc = IF_ID.npc;
   }
   else
   {
    ID_EX.regwrite = 0;
    ID_EX.npc = IF_ID.npc;
   }
   break;
  case id_G_EX:
   // Calculate the value of PC + MEMORY_INC (return address) to be stored in $ra ($31)
   EX_MEM.alures = ID_EX.npc + 4;
   // Indicate that Register RA will receive the return address
   EX_MEM.rdest = Ra;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction bgezal behavior method.
void ac_behavior(bgezal)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   /* Stalls the pipeline if there is any chance that the register being tested is not ready. */
   if (((ID_EX.regwrite == 1) && ((ID_EX.rd == rs) || (ID_EX.rt == rs))) ||
       ((EX_MEM.regwrite == 1) && (EX_MEM.memread == 1) && (EX_MEM.rdest == rs)))
   {
    G_ID.stall();
    IF_ID.suspend();
    ID_EX.regwrite = 0;
   }
   else if (!(id_value1 & 0x80000000))
   {
    ac_pc = IF_ID.npc + (imm << 2);
    ID_EX.regwrite = 1;
    ID_EX.npc = IF_ID.npc;
   }
   else
   {
    ID_EX.regwrite = 0;
    ID_EX.npc = IF_ID.npc;
   }
   break;
  case id_G_EX:
   // Calculate the value of PC + MEMORY_INC (return address) to be stored in $ra ($31)
   EX_MEM.alures = ID_EX.npc + 4;
   // Indicate that Register RA will receive the return address
   EX_MEM.rdest = Ra;
   EX_MEM.regwrite = ID_EX.regwrite;
   EX_MEM.memread = ID_EX.memread;
   EX_MEM.memwrite = ID_EX.memwrite;
   break;
  case id_G_MEM:
   TYPE_I_MEM_WB_REG_COPY;
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction sys_call behavior method.
void ac_behavior(sys_call)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   fprintf(stderr, "syscall behavior not implemented.\n"); exit(EXIT_FAILURE);
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}

//! Instruction instr_break behavior method.
void ac_behavior(instr_break)
{
 switch (stage)
 {
  case id_G_IF:
   break;
  case id_G_ID:
   break;
  case id_G_EX:
   fprintf(stderr, "instr_break behavior not implemented.\n"); exit(EXIT_FAILURE);
   break;
  case id_G_MEM:
   break;
  case id_G_WB:
   break;
  default:
   break;
 }
 return;
}
