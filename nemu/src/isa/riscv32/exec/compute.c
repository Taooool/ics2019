#include "cpu/exec.h"

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

//pa2
make_EHelper(auipc) {
  rtl_add(&id_dest->val, &cpu.pc, &id_src->val);
  rtl_sr(id_dest->reg, &id_dest->val, 4);

  print_asm_template2(auipc);
}

make_EHelper(I_opcode_4){
  switch (decinfo.isa.instr.funct3){
  case 0:{  // addi
    rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);

    print_asm_template2(addi);

    break;
  }
  case 1:{  // slli
    rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);

    print_asm_template2(slli);

    break;
  }
  case 2:{  // slti
    id_dest->val = (signed)id_src->val < (signed)id_src2->val;
    rtl_sr(id_dest->reg, &id_dest->val, 4);

    print_asm_template2(slti);

    break;
  }
  case 3:{  // sltiu
    id_dest->val = (unsigned)id_src->val < (unsigned)id_src2->val;
    rtl_sr(id_dest->reg, &id_dest->val, 4);

    print_asm_template2(sltiu);

    break;
  }
  case 4:{  // xori
    rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);

    print_asm_template2(xori);

    break;
  }
  case 5:{  // srli || srai
    if(decinfo.isa.instr.funct7 == 0x0){ // srli
      rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template2(srli);
    }
    else{ // srai
      rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template2(srai);
    }
    break;
  }
  case 6:{  // ori
    rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);

    print_asm_template2(ori);

    break;
  }
  case 7:{  // andi
    rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
    rtl_sr(id_dest->reg, &id_dest->val, 4);

    print_asm_template2(andi);

    break;
  }
  default: break;
  }
}

make_EHelper(R_opcode_c){
  switch (decinfo.isa.instr.funct3){
  case 0: {
    if(decinfo.isa.instr.funct7 == 0x00){ // add
      rtl_add(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(add);
      
    }
    else if(decinfo.isa.instr.funct7 == 0x20){ // sub
      rtl_sub(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(sub);
    }
    else{ // mul
      rtl_imul_lo(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(mul);
    }
    break;
  }
  case 1: {
    if(decinfo.isa.instr.funct7 == 0x00){  // sll
      rtl_shl(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(sll);

    }
    else{  // mulh
      rtl_imul_hi(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(mulh);
    }
    break;
  }
  case 2: {
    if(decinfo.isa.instr.funct7 == 0x00){  // slt
      id_dest->val = (signed)id_src->val < (signed)id_src2->val;
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(slt);
    }
    else{ // mulhsu
      TODO();
    }
    break;
  }
  case 3: {
    if(decinfo.isa.instr.funct7 == 0x00){  // sltu
      id_dest->val = (unsigned)id_src->val < (unsigned)id_src2->val;
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(sltu);
    }
    else{ // mulhu
      TODO();
    }
    break;
  }
  case 4: {
    if(decinfo.isa.instr.funct7 == 0x00){ // xor
      rtl_xor(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(xor);
    }
    else{ // div
      rtl_idiv_q(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(div);
    }
    break;
  }
  case 5: {
    if(decinfo.isa.instr.funct7 == 0x00){  // srl
      rtl_shr(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(srl);
    }
    else if(decinfo.isa.instr.funct7 == 0x20){ // sra
      rtl_sar(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(sra);
    }
    else{ // divu
      rtl_div_q(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(divu);
    }
    break;
  }
  case 6: {
    if(decinfo.isa.instr.funct7 == 0x00){  // or
      rtl_or(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(or);
    }
    else{ // rem
      rtl_idiv_r(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(rem);
    }
    break;
  }
  case 7: { // and
    if(decinfo.isa.instr.funct7 == 0x00){
      rtl_and(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(and);
    }
    else{ // remu
      rtl_div_r(&id_dest->val, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &id_dest->val, 4);

      print_asm_template3(remu);
    }
    break;
  }
  default: break;
  }
}
