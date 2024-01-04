#ifndef CLI_H
#define CLI_H

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "ctype.h"

#define MAX_PARAMETERS  15      /* Max no. of prameters in a command */
#define MAX_LEN_TOKEN   84      /* Max length of a token */
#define MIN_PWD_LEN     4
#define SPACE_SIZE      128
#define CLI_PROMPT_LEN   9
#define CMD_SIZE        120
#define MAX_LEN_ARGS    512     /* Max length of all args in a given tree */
#define MAX_USER_LEN      9
#define MAX_PWD_LEN       9

#define USER_MODE_ID    "user"
#define ENGG_MODE_ID    "engg"
#define ROOT_MODE_ID    "root"
      
extern int echo_char;
#define ENABLEECHO()            {echo_char = 1;}
#define DISABLEECHO()           {echo_char = 0;}

typedef enum  { CLI_FAILURE, CLI_SUCCESS, ERR_MULTI_TOKENS, EMPTY_STRING, BAD_MODE, EMPTY_TREE } cli_status_t;

typedef enum  { MORE_ARGUMENTS = 100, FEWER_ARGUMENTS, MULTIPLE_TOKENS, UNKNOWN_TOKEN, NO_PERMISSION,
                SUICIDAL, NOTHING, MEM_FAILURE } cli_error_t;

typedef enum { SUCC_CALLBACK = 200, SUCC_NEXTTREE, ERR_IMPLEMENTATION, ERR_PRIVILEGE } cli_avl_tree_t;

int setpasswd(char *, char *);

#define NMODES                  3
#define USER_MODE               0x100
#define SUPERVISORY_MODE        0x200
#define ENGG_MODE               0x400
#define INPUT_MODE              0x2

#define START_MODE              USER_MODE
#define SET_PASSWORD            setpasswd

typedef int (*funcPtr)(char *str[]);

typedef struct CliTreeNode
{
  char    data[MAX_LEN_TOKEN];
  int     balFactor;
  int     status;
/*  unsigned cmArrIdx; */
  int     (*callBackFn)(char **str);
  struct  CliTreeNode *lchild;
  struct  CliTreeNode *rchild;
  struct  CliTreeNode *nextTree;
} CliTreeNode;

/* Support for different privilage levels */
struct usermodes
{
        int mode;
        char userid[MAX_USER_LEN];
        char passwd[MAX_PWD_LEN];
        char name[20];
        char prompt[CLI_PROMPT_LEN];
};

extern CliTreeNode *cli_root;
extern CliTreeNode *nextRoot;
extern char clrLine[SPACE_SIZE];         /*clrLine contains spaces*/
extern struct usermodes usermodes[NMODES];
extern char prompt[CLI_PROMPT_LEN], exit_flag;
extern int current_mode;

int init_cli();
void CliCleanup(void);
int InsertCommand(const char *cmd,int mode, funcPtr callBackFn);
int ParseACommand(const char *);

#endif // CLI_H
