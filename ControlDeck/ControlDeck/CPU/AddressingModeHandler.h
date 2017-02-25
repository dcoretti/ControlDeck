//#pragma once
//#include "AddressingMode.h"
//#include "SystemComponents.h"
//#include "MemoryMapper.h"
//
//namespace NES {
//    /**
//    *    Handler for instruction based parameter fetching and calculation of addresses.  Each instruction has an 
//    *    implied mode of memory access that determines the number of operands to fetch after the op code, and what
//    *    the final memory locations an instruction acts on.
//    *
//    *    Each method calculates the final absolute address in memory to be used by the instruction.  No 
//    *   read or write is done on that address except for Immediate and Relative addressing, which puts the value on the data bus.
//    *
//    *    References for addressing implementation: http://nesdev.com/NESDoc.pdf Appendix E
//    *   Cycle timing information sourced from http://users.telenet.be/kim1-6502/6502/hwman.html Appendix D
//    */
//    namespace AddressingModeHandler {
//
//    }
//
//}