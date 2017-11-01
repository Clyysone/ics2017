#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  NOTYPE = 256, EQU, NEQ, LAND, LOR, L, LE, G, GE, REG, INTEGER, HEX, VAR, NEG_SIGN, DEREFERENCE, UNKOWN

  /* TODO: Add more token types */

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", NOTYPE},	// spaces
  {"\\+", '+'}, 	// add
  {"-", '-'},		// sub or neg_sign
  {"\\*", '*'}, 	// multiple or dereference
  {"/", '/'},		// divide
  {"%", '%'},		// mod
  {"\\(",'('},      //left parenthese
  {"\\)",')'},      //right parenthese
  {"==", EQU},      // equal
  {"!=", NEQ},      // not equal
  {"!",'!'},        //not
  {"=",'='},        //assign
  {"&&", LAND},		// logic and
  {"\\|\\|", LOR},	// logic or
  {"<=",LE},		// less or equal than
  {"<", L},         // less than
  {">=",GE},		// greater or equal than
  {">", G},         // greater than
  {"0x[0-9a-fA-F]{1,32}",HEX},  //hex num
  {"[1-9]+[0-9]*|0",INTEGER},		//integer
  {"[a-zA-Z_]+[0-9a-zA-Z_]*", VAR}, // var
  {"\\$[a-zA-Z]+", REG},    //regitster
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

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
  char str[128];
} Token;

Token tokens[128];
int nr_token;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') 
  {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) 
	  {
  	  if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) 
  	  {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        //Log("match rules[%d] = \"%s\" at position %d with len %d: %s",
          //  i, rules[i].regex, position, substr_len, substr_start);
        position += substr_len;
        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) 
  	    {		
    			case NOTYPE:
                        break;
    			default:
    				    tokens[nr_token].type = rules[i].token_type;
    				    strncpy(tokens[nr_token].str,substr_start,substr_len);
    				    tokens[nr_token].str[substr_len]='\0'; 
                        //printf("get %s\n",tokens[nr_token].str);//delete
                        nr_token ++;
        }
        break;
      }
    }

    if (i == NR_REGEX) 
	  {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }
  //printf("true!\n");
  return true;
}


bool check_parentheses(int p,int q)
{
    if( tokens[p].type != '(' || tokens[q].type != ')' ) return false;
    int lp=0, rp=0, i;
    for(i=p+1;i<q;i++)
    {
        if(tokens[i].type == '(') lp++;
        if(tokens[i].type == ')') rp++;
        if(rp>lp) 
        {
            //printf("Bad expression!\n");    
            return false;
        }
    }
    //printf("left: %d right:%d\n",lp,rp);
    if(lp==rp) return true;
    else 
    {
        //printf("Bad expression!\n");
        return false;
    }
}

uint32_t dominant_operator(int p, int q) {
    int pos=p,i,match=0;
    //ordered by priority: 
    //high: * -
    //      * / %
    //      + -
    //      < <= > >=
    //      == !=
    //low:  && ||
    for(i=p;i<=q;i++)
    {
        if(tokens[i].type == '(') match++;
        if(tokens[i].type == ')') match--;
        if((tokens[i].type == LAND || tokens[i].type == LOR) && (i>pos) && (!match)) pos = i;
    }
    if(pos == p)
    {
        for(i=p;i<=q;i++)
        {
            if(tokens[i].type == '(') match++;
            if(tokens[i].type == ')') match--;
            if((tokens[i].type == EQU || tokens[i].type == NEQ) && (i>pos) && (!match)) pos = i;
        }
    }
    if(pos == p)
    {
        for(i=p;i<=q;i++)
        {
            if(tokens[i].type == '(') match++;
            if(tokens[i].type == ')') match--;
            if((tokens[i].type == L || tokens[i].type == LE || tokens[i].type == G || tokens[i].type == GE) && (i>pos) && (!match))
                pos = i;
        }
    }
    if(pos == p)
    {
        for(i=p;i<=q;i++)
        {
            if(tokens[i].type == '(') match++;
            if(tokens[i].type == ')') match--;
            if((tokens[i].type == '+' || tokens[i].type == '-') && (i>pos) && (!match)) pos = i;
        }
    }
    if(pos == p)
    {
        for(i=p;i<=q;i++)
        {
            if(tokens[i].type == '(') match++;
            if(tokens[i].type == ')') match--;
            if((tokens[i].type == '*' || tokens[i].type == '/' || tokens[i].type == '%') && (i>pos) && (!match)) pos = i;
        }
    }
    if(pos == p)
    {
        for(i=p;i<=q;i++)
        {
            if(tokens[i].type == '(') match++;
            if(tokens[i].type == ')') match--;
            if((tokens[i].type == NEG_SIGN || tokens[i].type == DEREFERENCE || tokens[i].type == '!') && (i>pos) && (!match)) pos = i;
        }
    }
    return pos;
}



uint32_t eval(int p, int q)
{
    if(p>q)
    {
        return 0;
    }
    if(p==q)
    {
        int value=0, i;
        if(tokens[p].type == HEX)//hex str to int
        {
            for(i = 2; i< strlen(tokens[p].str); i++)
            {
                if(tokens[p].str[i]>= '0' && tokens[p].str[i]<= '9')
                    value = value * 16 + tokens[p].str[i] - '0';
                if(tokens[p].str[i]>= 'A' && tokens[p].str[i]<= 'F')
                    value = value * 16 + tokens[p].str[i] - 'A' + 10;
                if(tokens[p].str[i]>= 'a' && tokens[p].str[i]<= 'f')
                    value = value * 16 + tokens[p].str[i] - 'a' + 10;
            }
        return value;
        }
        else if(tokens[p].type == INTEGER) //dec str to int
        {
            for(i = 0; i< strlen(tokens[p].str); i++)
            {
                value = value * 10 + tokens[p].str[i] - '0';
            }
            return value;
        }
        else if(tokens[p].type == REG)// 32bit register only
        {
            if (!strcmp(tokens[p].str,"$eax")) return cpu.eax;
            else if (!strcmp(tokens[p].str,"$ecx")) return cpu.ecx;
            else if (!strcmp(tokens[p].str,"$edx")) return cpu.edx;
            else if (!strcmp(tokens[p].str,"$ebx")) return cpu.ebx;
            else if (!strcmp(tokens[p].str,"$esp")) return cpu.esp;
            else if (!strcmp(tokens[p].str,"$ebp")) return cpu.ebp;
            else if (!strcmp(tokens[p].str,"$esi")) return cpu.esi; 
            else if (!strcmp(tokens[p].str,"$edi")) return cpu.edi;
            else if (!strcmp(tokens[p].str,"$eip")) return cpu.eip;
            // TODO: 16bit and 8bit reg
        }
        else if(tokens[p].type == VAR)// todo
        {
            //TODO: variable value
            printf("\nvariable is not implemented!\n");
            return 0;
        }
    }
    

    else if (check_parentheses(p, q) == true) 
    {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
        //printf("the parenthese should be throw away! %d %d\n",p,q);
        return eval(p + 1, q - 1);
    }

    
    else
    {
        int op = dominant_operator(p,q);
        //printf("op here %d %s %d to %d!\n",op,tokens[op].str,p,q);
        int val1=0,val2=0;
        if(op == p || tokens[op].type == DEREFERENCE || tokens[op].type == NEG_SIGN || tokens[op].type == '!')
        {
            uint32_t val = eval(p+1,q);
            switch(tokens[op].type)
            {
                case DEREFERENCE: return vaddr_read(val,4);
                case NEG_SIGN: return -val;
                case '!': return !val;
                default: Assert(1,"single operator meet some error\n");
            }
        }
        //printf("val1 here %d %s %d to %d!\n",op,tokens[op].str,p,q);
        val1 = eval(p,op-1);
        //printf("val2 here %d %s %d to %d!\n",op,tokens[op].str,p,q);
        val2 = eval(op+1,q);
        switch(tokens[op].type)
        {
            case '+': return val1+val2;
            case '-': return val1-val2;
            case '*': return val1*val2;
            case '/': return val1/val2;
            case '%': return val1%val2;
            case EQU: return val1==val2;
            case NEQ: return val1!=val2;
            case LAND: return val1&&val2;
            case LOR: return val1||val2;
            case L: return val1<val2;
            case LE: return val1<=val2;
            case G: return val1>val2;
            case GE: return val1>=val2;
            default: Assert(1,"unexpected!");
        }
    }
    printf("Bad expression, return wrong value!\n");
    return 0;
    
}

uint32_t expr(char *e, bool *success) {
    if (!make_token(e)) {
        *success = false;
        return 0;
    }
    int i;
    for(i = 0; i < nr_token; i ++) 
    {
        if(tokens[i].type=='*' && (i==0 || tokens[i-1].type=='+' || tokens[i-1].type == '-' || tokens[i-1].type ==NEG_SIGN
            || tokens[i-1].type=='*' || tokens[i-1].type=='/' || tokens[i-1].type=='/'|| tokens[i-1].type=='(')) 
        {
                tokens[i].type=DEREFERENCE;
                //printf("token %d change to dereference\n",i);
        }
        if(tokens[i].type=='-' && (i==0 || tokens[i-1].type=='+' || tokens[i-1].type =='-' || tokens[i-1].type ==NEG_SIGN
            || tokens[i-1].type=='*' || tokens[i-1].type=='/' || tokens[i-1].type=='/' || tokens[i-1].type=='(')) 
        {
            tokens[i].type=NEG_SIGN;
            //printf("token %d change to negative\n",i);
        }
    }


    /* TODO: Insert codes to evaluate the expression. */
    //TODO();
    //printf("%d\n",nr_token-1);
    *success = true;
    return eval(0,nr_token-1);
}
