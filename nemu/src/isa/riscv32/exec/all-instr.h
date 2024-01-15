#include "cpu/exec.h"

make_EHelper(lui);

make_EHelper(ld);
make_EHelper(st);

make_EHelper(inv);
make_EHelper(nemu_trap);

//pa2
// compute.c
make_EHelper(auipc);
make_EHelper(I_opcode_4);
make_EHelper(R_opcode_c);

// control.c
make_EHelper(jal);
make_EHelper(jalr);
make_EHelper(B_opcode_18);

// ldst.c
make_EHelper(lh);
make_EHelper(lb);


