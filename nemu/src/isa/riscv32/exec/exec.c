#include "cpu/exec.h"
#include "all-instr.h"

//pa2
static OpcodeEntry load_table [8] = {
  EX(lb), EX(lh), EXW(ld, 4), EMPTY, EXW(ld,1), EXW(ld,2), EMPTY, EMPTY
};

static make_EHelper(load) {
  decinfo.width = load_table[decinfo.isa.instr.funct3].width;
  idex(pc, &load_table[decinfo.isa.instr.funct3]);  //lw's funct3 corresponds to load_table[]'s index
}

//pa2
static OpcodeEntry store_table [8] = {
  EXW(st,1), EXW(st,2), EXW(st, 4), EMPTY, EMPTY, EMPTY, EMPTY, EMPTY
};

static make_EHelper(store) {
  decinfo.width = store_table[decinfo.isa.instr.funct3].width;
  idex(pc, &store_table[decinfo.isa.instr.funct3]);
}

//pa2
//IDEX(ld, load): lw instr's decode function is 'make_DHelper(ld)' in decode.c; execute function is 'make_EHelper(load)' in exec.c
//make_EHelper(load) -> EXW(ld, 4)(is a new OpcodeEntry which decode function is NULL, execute function is also 'make_EHelper(ld)', width is 4) -> make_EHelper(ld) in Idst.c
//in short: lw -> IDEX(ld, load) -> make_EHelper(load) -> EXW(ld, 4) -> make_EHelper(ld)
static OpcodeEntry opcode_table [32] = {
  /* b00 */ IDEX(ld, load), EMPTY, EMPTY, EMPTY, IDEX(I, I_opcode_4), IDEX(U, auipc), EMPTY, EMPTY,
  /* b01 */ IDEX(st, store), EMPTY, EMPTY, EMPTY, IDEX(R, R_opcode_c), IDEX(U, lui), EMPTY, EMPTY,
  /* b10 */ EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY, EMPTY,
  /* b11 */ IDEX(B, B_opcode_18), IDEX(I, jalr), EX(nemu_trap), IDEX(J, jal), EMPTY, EMPTY, EMPTY, EMPTY,
};

void isa_exec(vaddr_t *pc) {
  decinfo.isa.instr.val = instr_fetch(pc, 4);
  assert(decinfo.isa.instr.opcode1_0 == 0x3);
  idex(pc, &opcode_table[decinfo.isa.instr.opcode6_2]);
}
