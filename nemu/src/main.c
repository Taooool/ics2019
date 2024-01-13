//pa1: 生成表达式
#include<stdio.h>
#include<nemu.h>
uint32_t expr(char *e, bool *success);

int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  //pa1: 生成表达式
  FILE *fp = fopen("/home/hust/ics2019/nemu/tools/gen-expr/input", "r");
  char experssion[100];
  uint32_t result;
  bool flag = true;
  int nofExpr = 0;
  while(!feof(fp))
  {
    fscanf(fp, "%d", &result);
    fscanf(fp, "%s", experssion);
    bool success = true;
    uint32_t result2 = expr(experssion, &success);
    printf("%s\t%d\t%d\n", experssion, result, result2);
    if(result != result2)
    {
    	flag = false;
    	printf("expresion calculate wrongly!\n");
    	break;
    }
    nofExpr++;
  }
  if(flag)
    printf("expersion calculate correctly!\ntest times: %d\n", nofExpr);
  fclose(fp);
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
