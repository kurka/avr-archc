/**
 * @file      r3000_syscall.cpp
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

#include "r3000_syscall.H"

using namespace r3000_parms;

void r3000_syscall::get_buffer(int argn, unsigned char* buf, unsigned int size)
{
 unsigned int addr = RB.read(4 + argn);
 for (unsigned int i = 0; i < size; i++, addr++)
 {
  buf[i] = DM.read_byte(addr);
 }
 return;
}

void r3000_syscall::set_buffer(int argn, unsigned char* buf, unsigned int size)
{
 unsigned int addr = RB.read(4 + argn);
 for (unsigned int i = 0; i < size; i++, addr++)
 {
  DM.write_byte(addr, buf[i]);
 }
 return;
}

void r3000_syscall::set_buffer_noinvert(int argn, unsigned char* buf, unsigned int size)
{
 unsigned int addr = RB.read(4 + argn);
 //TODO: fixed wordsize
 for (unsigned int i = 0; i < size; i += 4, addr += 4)
 {
  DM.write(addr, *((unsigned int*) &buf[i]));
 }
 return;
}

int r3000_syscall::get_int(int argn)
{
 return RB.read(4 + argn);
}

void r3000_syscall::set_int(int argn, int val)
{
 RB.write((2 + argn), val);
 return;
}

void r3000_syscall::return_from_syscall()
{
 ac_pc.async_write(RB.read(31));
 return;
}

void r3000_syscall::set_prog_args(int argc, char** argv)
{
 int i, j, base;
 unsigned int ac_argv[30];
 char ac_argstr[512];

 base = AC_RAM_END - 512;
 for (i = 0, j = 0; i < argc; i++)
 {
  int len = strlen(argv[i]) + 1;
  ac_argv[i] = base + j;
  memcpy(&ac_argstr[j], argv[i], len);
  j += len;
 }
 // Adjust %sp and write argument string
 RB.write(29, (AC_RAM_END - 512));
 set_buffer(25, (unsigned char*) ac_argstr, 512);   //$25 = $29(sp) - 4 (set_buffer adds 4)
 // Adjust %sp and write string pointers
 RB.write(29, (AC_RAM_END - 512 - 120));
 set_buffer_noinvert(25, (unsigned char*) ac_argv, 120);
 // Set %o0 to the argument count
 RB.write(4, argc);
 //Set %o1 to the string pointers
 RB.write(5, (AC_RAM_END - 512 - 120));
 //Initialize register 0
 RB.write(0, 0);
 return;
}
