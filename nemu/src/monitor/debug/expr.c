#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
//pa1: 表达式求值
#include <stdlib.h> //to use atoi()
#include <stdio.h> //to use sscanf()
uint32_t isa_reg_str2val(const char *s, bool *success);

enum {
  TK_NOTYPE = 256, TK_EQ

  /* TODO: Add more token types */
  //pa1: 词法分析
  , TK_PLUS, TK_MINUS, TK_MULTIPLE, TK_DIVIDE, TK_LEFT_PARENTHESIS, TK_RIGHT_PARENTHESIS
  , TK_DECIMAL, TK_HEX, TK_REG, TK_DEREF, TK_NEQ, TK_AND
};

static struct rule {
  char *regex;
  int token_type;
  //pa1: 词法分析
  int priority; //the bigger num is, the higher priority is
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  //pa1: 词法分析
  {" +", TK_NOTYPE, 0},    // spaces
  {"\\+", TK_PLUS, 3},     // plus
  {"-", TK_MINUS, 3},      // minus
  {"\\*", TK_MULTIPLE, 4}, // multiple
  {"/", TK_DIVIDE, 4},     // divide
  {"\\(", TK_LEFT_PARENTHESIS, 6}, // left parenthesis
  {"\\)", TK_RIGHT_PARENTHESIS, 6}, // right parenthesis
  //TODO: deciaml can be more accurate
  {"0[xX][0-9a-fA-F]+", TK_HEX, 0}, // hex, ps: hex need to be declared before decimal
  {"[0-9]+", TK_DECIMAL, 0}, // decimal
  {"\\$[0-9a-zA-Z]+", TK_REG, 0}, // register
  {"==", TK_EQ, 2},        // equal
  {"!=", TK_NEQ, 2},       // not equal
  {"&&", TK_AND, 1},       // and
  //pointer need to be recognized specially in function expr()
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
  //pa1: 词法分析
  int priority; //corresponding to the array rules[]
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */
        //pa1: 词法分析
        if(substr_len > 31)
            panic("the length of the token is too long");
        switch(rules[i].token_type)
        {
          case TK_NOTYPE: break;
          case TK_DECIMAL:
          case TK_HEX:
          case TK_REG:
            //printf("token type: %d \t priority: %d\n", rules[i].token_type, rules[i].priority);
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].priority = rules[i].priority;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';
            nr_token++;
            break;
          case TK_PLUS:
          case TK_MINUS:
          case TK_MULTIPLE:
          case TK_DIVIDE:
          case TK_LEFT_PARENTHESIS:
          case TK_RIGHT_PARENTHESIS:
          case TK_EQ:
          case TK_NEQ:
          case TK_AND:
            tokens[nr_token].type = rules[i].token_type;
            tokens[nr_token].priority = rules[i].priority;
            nr_token++;
            break;
          default: TODO();
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  //printf("nr_token = %d\n", nr_token);
  return true;
}

//pa1: 递归求值
//function: check the expression is surrounded by parentheses or not
static bool check_parentheses(int p, int q)
{
  if(tokens[p].type != TK_LEFT_PARENTHESIS || tokens[q].type != TK_RIGHT_PARENTHESIS)
    return false;
  //dipose special case: (1+2) * (3+4) which is correct but not a BNF
  int leftParenthesesNum = 0;
  for(int i=p; i<=q; i++)
  {
    if(tokens[i].type == TK_LEFT_PARENTHESIS)
      leftParenthesesNum++;
    else if(tokens[i].type == TK_RIGHT_PARENTHESIS)
      leftParenthesesNum--;
    if(leftParenthesesNum == 0 && i != q)
      return false;
  }
  return true;
}

//pa1: 递归求值
//function: get the division operator's position to split the expression
static int get_op(int p, int q)
{
  int op = -1;
  int priority = 9999;
  int parenthesesNum = 0;
  for(int i=p; i<=q; i++)
  {
    if(tokens[i].type == TK_LEFT_PARENTHESIS)
      parenthesesNum++;
    else if(tokens[i].type == TK_RIGHT_PARENTHESIS)
      parenthesesNum--;
    else if(parenthesesNum == 0)
    {
      //use != 0 to ignore the spaces
      if(tokens[i].priority <= priority && tokens[i].priority != 0)
      {
        op = i;
        priority = tokens[i].priority;
      }
    }
  }
  return op;
}

//pa1: 递归求值
//argument: p,q refer to token position not character position in expr
static uint32_t eval(int p, int q, bool *success)
{
  if(p>q)
  {
    *success = false;
    return 0;
  }
  
  //dispose the spaces in the expression
  while(tokens[p].type == TK_NOTYPE && p <= q)
    p++;
  while(tokens[q].type == TK_NOTYPE && p <= q)
    q--;

  if(p==q)
  {
    int result = 0;
    //printf("%d %d\n", tokens[p].type, TK_DECIMAL);
    if(tokens[p].type == TK_DECIMAL)
    {
      //printf("it is a decimal\n");
      result = atoi(tokens[p].str);
    }
    else if(tokens[p].type == TK_HEX)
      sscanf(tokens[p].str, "%x", &result);
    //printf("token str: %s \t result: %d\n", tokens[p].str, result);

    //pa1: 扩展表达式求值的功能
    else if(tokens[p].type == TK_REG)
    {
      //printf("reg= %s\n", tokens[p].str);
      result = isa_reg_str2val(tokens[p].str, success);
      if(success == false)
        printf("can't find the register\n");
    }
    return result;
  }
  else if(check_parentheses(p, q) == true)
    return eval(p+1, q-1, success);
  else
  {
    int opIndex = get_op(p, q);
    //printf("opIndex = %d\n", opIndex);
    //can't dispose the pointer case in the following switch case
    if(opIndex == -1)
    {
      *success = false;
      printf("can't find the operator\n");
      return 0;
    }
    else if(tokens[opIndex].type == TK_DEREF)
    {
      paddr_t addr = eval(opIndex+1, q, success);
      printf("TK_DEREF\taddr= 0x%08x\n", addr);
      return paddr_read(addr, 4);
    }
    int val1 = eval(p, opIndex-1, success);
    int val2 = eval(opIndex+1, q, success);
    switch(tokens[opIndex].type)
    {
      case TK_PLUS: return val1 + val2;
      case TK_MINUS: return val1 - val2;
      case TK_MULTIPLE: return val1 * val2;
      case TK_DIVIDE: 
        if(val2 == 0)
        {
          *success = false;
          printf("divisor can't be zero\n");
          return 0;
        }
        else
          return val1 / val2;
      case TK_EQ: return val1 == val2;
      case TK_NEQ: return val1 != val2;
      case TK_AND: return val1 && val2;
      default: assert(0);
    }
  }
  return 0;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  //pa1: 词法分析
  for(int i=0; i<nr_token; i++)
  {
    int formerTokenType = (i==0) ? TK_NOTYPE : tokens[i-1].type;
    //e.g. an expression like ( *p) is BNF, and 4 + *p is not BNF, it should be 4 + (*p)
    if(tokens[i].type == '*' && (formerTokenType == 0 || formerTokenType == TK_NOTYPE || formerTokenType == TK_LEFT_PARENTHESIS))
    {
      tokens[i].type = TK_DEREF;
      tokens[i].priority = 5;
    }
  }

  //pa1: 递归求值
  return eval(0, nr_token-1, success);

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
