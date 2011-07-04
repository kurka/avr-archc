/**
 * @file      armv5e_gdb_funcs.cpp
 * @author    Danilo Marcolin Caravana
 *
 *            The ArchC Team
 *            http://www.archc.org/
 *
 *            Computer Systems Laboratory (LSC)
 *            IC-UNICAMP
 *            http://www.lsc.ic.unicamp.br/
 *
 * @version   1.0
 * @date      Mon, 19 Jun 2006 15:33:28 -0300
 * 
 * @brief     The ArchC ARMv5e functional model.
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

#include "armv5e.H"

using namespace armv5e_parms;

int armv5e::nRegs(void) {
   return 16;
}

ac_word armv5e::reg_read( int reg ) {
  
  /* general purpose registers */
  if ( ( reg >= 0 ) && ( reg < 15 ) )
    return RB.read( reg );
  else if ( reg == 15 )
    return ac_pc;
  //    else /* cpsr */
  //      return CPSR.read();
  // CPSR.read did not work;
  return 0;
}

void armv5e::reg_write( int reg, ac_word value ) {
  /* general purpose registers */
  printf("Register is: %d, value is %x\n",reg,value);
  if ( ( reg >= 0 ) && ( reg < 15 ) )
    RB.write( reg, value );
  else if ( reg == 15 )
    /* pc */
    ac_pc = value;
  //  else /* CPSR */
  //    CPSR.write ();
  // CPSR.write did not work either
}

unsigned char armv5e::mem_read( unsigned int address ) {
  return IM->read_byte(address);
}

void armv5e::mem_write( unsigned int address, unsigned char byte ) {
  IM->write_byte( address, byte );
}

