#include "nemu.h"

const char *regsl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

void isa_reg_display() {
	//pa1: display registers like GDB
	for(int i=0; i<32; i++)
		printf("%s\t0x%08x\t%d\n", regsl[i], reg_l(i), reg_l(i));
	printf("pc\t0x%08x\t%d\n", cpu.pc, cpu.pc);
	
}

uint32_t isa_reg_str2val(const char *s, bool *success) {
  //pa1: 扩展表达式求值的功能
  *success = true;
  if(strcmp("pc", s+1))
  {
    printf("1");
    return cpu.pc;
  }
  else if(s[1] == '0')
  {
    printf("2");
    return reg_l(0);
  }
  else
  {
    for(int i=1; i<32; i++)
    {
      if(strcmp(regsl[i], s+1) == 0)
      {
        uint32_t result = reg_l(i);
        printf("i= %d\treg= %s\tval= 0x%08x\t%d\n", i, regsl[i], result, result);
        return result;
      }
    }
  }
  *success = false;
  return 0;
}
