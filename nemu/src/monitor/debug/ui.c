#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
//pa1: 打印寄存器
void isa_reg_display();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_help(char *args);

//pa1: 单步执行
static int cmd_si(char *args);

//pa1: 打印寄存器
static int cmd_info(char *args); 

//pa1: 扫描内存
static int cmd_x(char *args);

//pa1: 表达式求值
static int cmd_p(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
	//pa1: 单步执行
	{ "si", "Single step execution", cmd_si },
	//pa1: 打印寄存器
	{ "info", "Display program status", cmd_info },
	//pa1: 扫描内存
	{ "x", "Read memory", cmd_x },
  //pa2: 表达式求值
  { "p", "Expression evaluation", cmd_p },

};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

//pa1: 单步执行
static int cmd_si(char *args)
{
	char *arg = strtok(NULL, " ");
	int step = 1;
	if(arg != NULL)
		step = atoi(arg);
	cpu_exec(step);
	return 0;
}

//pa1: 打印寄存器
static int cmd_info(char *args)
{
	char *arg = strtok(NULL, " ");
	if(arg == NULL)
	{
		printf("Please input the argument!\n");
		return 0;
	}	
	if(strcmp(arg, "r") == 0)
		isa_reg_display();
	else if(strcmp(arg, "w") == 0)
	{

	}
	else
		printf("Unknown command '%s'\n", arg);
	return 0;
}

//pa1: 扫描内存
static int cmd_x(char *args)
{
	char *arg = strtok(NULL, " ");
	if(arg == NULL)
	{
		printf("Please input two argument!\n");
		return 0;
	}
	int n = atoi(arg);
	arg = strtok(NULL, " ");
	if(arg == NULL)
	{
		printf("Please input two argument!\n");
		return 0;
	}
	paddr_t addr = strtol(arg, NULL, 16);
	for(int i=0; i<n; i++)
	{
		printf("0x%08x: ", addr);
		for(int j=0; j<4; j++)
		{
			printf("%02x ", paddr_read(addr, 1));
			addr++;
		}
		printf("\n");
	}
	return 0;
		
}

//pa2: 表达式求值
static int cmd_p(char *args)
{
  bool success = true;
  uint32_t result = expr(args, &success);
  if(success == false)
    printf("Expression evaluation failed!\n");
  else
    printf("0x%x\t%d\n", result, result);
  return 0;
}

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue(void);
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
