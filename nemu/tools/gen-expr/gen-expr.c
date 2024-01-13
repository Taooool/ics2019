#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

// this should be enough
static char buf[65536];

//pa1: 生成表达式
uint32_t choose(uint32_t n) {
  return rand()%n;
}

//arguments: p represents the left bound of the expression, q represents the right bound of the expression, both are array index
static inline void gen_rand_expr(int p, int q) {
  // buf[0] = '\0';
  //pa1: 生成表达式
  //buf[q+1] = '\0';  //ps: can't implement here cause the function will iterate
  if(p > q)
    return ;	//ps:don't foget
  else if(p == q) //1 token expersion is a number
    buf[p] = choose(9) + '1'; //choose(9) 'scope is 0~8, so add '1' to get the char. not gen 0 to prevent /0 case
  else if(p + 1 == q) //2 token expersion is also a number
  {
    buf[p] = choose(9) + '1'; //the first number can't be 0
    buf[q] = choose(10) + '0';
  }
  else
  {
    int opIndex = choose(q-p-1) + p + 1; //choose an operator, opIndex's scope is [p+1, q-1]
    switch(choose(5))
    {
      case 0:
        gen_rand_expr(p, opIndex-1);
        buf[opIndex] = '+';
        gen_rand_expr(opIndex+1, q);
        break;
      case 1:
        gen_rand_expr(p, opIndex-1);
        buf[opIndex] = '-';
        gen_rand_expr(opIndex+1, q);
        break;
      case 2:
        gen_rand_expr(p, opIndex-1);
        buf[opIndex] = '*';
        gen_rand_expr(opIndex+1, q);
        break;
      case 3:
        gen_rand_expr(p, opIndex-1);
        buf[opIndex] = '/';
        gen_rand_expr(opIndex+1, q);
        break;
      case 4:
        buf[p] = '(';
        buf[q] = ')';
        gen_rand_expr(p+1, q-1);
        break;
      default: assert(0);
    }
  }

}

static char code_buf[65536];
static char *code_format =
"#include <stdio.h>\n"
"int main() { "
"  unsigned result = %s; "
"  printf(\"%%u\", result); "
"  return 0; "
"}";

int main(int argc, char *argv[]) {
  int seed = time(0);
  srand(seed);
  int loop = 1;
  if (argc > 1) {
    sscanf(argv[1], "%d", &loop);
  }
  int i;
  for (i = 0; i < loop; i ++) {
    gen_rand_expr(0, 10);
    //pa1: 生成表达式
    buf[11]='\0';
    //for(int i=0; i<=10; i++)
    	//printf("%c", buf[i]);
    //printf("\n");

    sprintf(code_buf, code_format, buf);

    FILE *fp = fopen("/tmp/.code.c", "w");
    assert(fp != NULL);
    fputs(code_buf, fp);
    fclose(fp);

    int ret = system("gcc /tmp/.code.c -o /tmp/.expr");
    if (ret != 0) continue;

    fp = popen("/tmp/.expr", "r");
    assert(fp != NULL);

    int result;
    int tmp = fscanf(fp, "%d", &result);
    pclose(fp);
    printf("%u %s\n", result, buf);
  }
  return 0;
}
