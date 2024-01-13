int init_monitor(int, char *[]);
void ui_mainloop(int);

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  int is_batch_mode = init_monitor(argc, argv);

  //pa1: 生成表达式
  FILE *fp = fopen("/home/hust/ics2019/nemu/tools/gen-expr/input", "r");
  char expersion[100];
  uint32_t result;
  while(!feof(fp))
  {
    fscanf(fp, "%d", &result);
    fscanf(fp, "%s", expersion);
    bool success = true;
    uint32_t result2 = expr(expersion, &success);
    printf("%s\t%d\t%d\t%d\n", expersion, result, result2, result == result2);
  }
  fclose(fp);
  /* Receive commands from user. */
  ui_mainloop(is_batch_mode);

  return 0;
}
