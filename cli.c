#include "cli.h"
#include "cli_cmd.h"

#define STRCMP      strmycasecmp
#define TAKE_CARE_OF_ARGS()     {\
        printf(args);\
        printf("\n");\
}
static char **inputArr;         /* Argument list for callbacks */
char clrLine[SPACE_SIZE];         /*clrLine contains spaces*/
CliTreeNode *cli_root;
CliTreeNode *nextRoot;
char prompt[CLI_PROMPT_LEN];                        /* current prompt, extern linkage */
struct usermodes usermodes[NMODES];
char str[256];
static char args[MAX_LEN_ARGS];
int current_mode;               /* current mode, extern linkage */
int echo_char = 1;
int indexer = -1;
static int found = 0;
unsigned int nomem = 0;
int cmdtabCnt=0;
char exit_flag = 0;


/* Prototypes for Local functions */
static CliTreeNode **InsertToken(char *var, int mode,
                          CliTreeNode **rootPtr, funcPtr callBackFn);
static void InOrder(CliTreeNode *,int tabCnt);
int SearchInTree(CliTreeNode *, char *);
static int strmycasecmp(const char *, const char *);
void PrintErr(int x);
int CliInitialize();
/***************************************************************************
** Function    :
**
** Inputs      :
**
** Outputs     :
**
** Description : Get user modes and related info

**************************************************************************/
int init_cli()
{
       usermodes[0].mode = USER_MODE;
       strcpy(usermodes[0].userid, USER_MODE_ID);
       strcpy(usermodes[0].passwd,"user");
       strcpy(usermodes[0].prompt,"user>");

       usermodes[2].mode = ENGG_MODE;
       strcpy(usermodes[2].userid, ENGG_MODE_ID);
       strcpy(usermodes[2].passwd,"engg");
       strcpy(usermodes[2].prompt,"engg$" );

       usermodes[1].mode = SUPERVISORY_MODE;
       strcpy(usermodes[1].userid, ROOT_MODE_ID);
       strcpy(usermodes[1].passwd,"root");
       strcpy(usermodes[1].prompt,"root#");

	   if(CliInitialize() != CLI_SUCCESS)
          return CLI_FAILURE;
	   if(RegisterCli() != CLI_SUCCESS)
		   return CLI_FAILURE;
       return CLI_SUCCESS;
}

/***************************************************************************
** Function: ChangePrompt
**
** Inputs:   prompt
**
** Outputs:  Success.
**************************************************************************/
int ChangePrompt(char *newPrompt)
{
    strcpy(prompt, newPrompt);
    return CLI_SUCCESS;
}

/***************************************************************************
** Function: set_startmode
**
** Inputs:   mode
**
** Outputs:  Success or Failure.
**************************************************************************/
static int set_startmode(int mode)
{
    int i;

    for (i = 0; i < NMODES; ++i)
        if (usermodes[i].mode == mode)
            break;
    if (i == NMODES)
        return CLI_FAILURE;
    current_mode = mode;
    ChangePrompt(usermodes[i].prompt);

    return CLI_SUCCESS;
}

/***************************************************************************
** Function: CliInitialize
**           Initialising necessary cmds in CLI tree and modes.
** Inputs:   modes.
**
** Outputs: success or failure.
**************************************************************************/
int CliInitialize()
{
    int i;

    cli_root = NULL;            /* Root of the CLI tree */

    inputArr = (char **) malloc(MAX_PARAMETERS * sizeof(char *));
    if(inputArr == NULL)
    {
       printf("CLI INIT: error in allocation\r\n");
       return CLI_FAILURE;
    }

    for(i = 0;i < MAX_PARAMETERS; i++)
      inputArr[i] = NULL;

    if (set_startmode(START_MODE) < 0)
        return CLI_FAILURE;

    if (InsertCommand("login <uname>", START_MODE, logincheck) == CLI_FAILURE)
        return CLI_FAILURE;
    if (InsertCommand("set passwd <uname>", SUPERVISORY_MODE, passwdcheck) == CLI_FAILURE)
        return CLI_FAILURE;
    if (InsertCommand("show mode list", ENGG_MODE, modenames) == CLI_FAILURE)
        return CLI_FAILURE;
    if (InsertCommand("exit application", START_MODE, exitmode) == CLI_FAILURE)
        return CLI_FAILURE;
	if (InsertCommand("set system prompt <prompt>", START_MODE, SetSystemPrompt) == CLI_FAILURE)
        return CLI_FAILURE;
	return (InsertCommand("who am i", START_MODE, whichmode));
}

/***************************************************************************
** Function: InsertToken
**           Inserting the tokens of cmd to CLI tree.Follows AVL algorithm.
** Inputs:   Token, mode, Cliroot and function.
**
** Outputs:  Returns nextptr of the inserted token.
**************************************************************************/
static CliTreeNode **InsertToken(char *var, int mode, CliTreeNode **rootPtr, funcPtr callBackFn)
{
    CliTreeNode *fNode, *pNode, *qNode, *new_ptr, *aNode, *bNode, *cNode;
    int d;

    if (!strncmp(var, "<", 1))
    {
        mode |= INPUT_MODE;
    }

    if (!(*rootPtr))
    {
        new_ptr = (CliTreeNode *) malloc(sizeof(CliTreeNode));
        if (!new_ptr)
        {
            printf("ERR: Memory alloc failed \r\n");
            return NULL;
        }
        (*rootPtr) = new_ptr;
        strcpy((*rootPtr)->data, var);
        (*rootPtr)->balFactor = 0;
        (*rootPtr)->callBackFn = callBackFn;
        (*rootPtr)->status = mode;
        (*rootPtr)->nextTree = NULL;
        (*rootPtr)->lchild = 0;
        (*rootPtr)->rchild = 0;
        return (&((*rootPtr)->nextTree));
    }

    /* Phase 1: */
    fNode = 0;
    aNode = (*rootPtr);
    pNode = (*rootPtr);
    qNode = 0;

    while (pNode)
    {
        if (pNode->balFactor)
        {
            aNode = pNode;
            fNode = qNode;
        }
        if (!STRCMP(var, pNode->data))
        {

            pNode->status |= mode;
            return (&(pNode->nextTree));
        }
        if (STRCMP(var, pNode->data) < 0)
        {
            qNode = pNode;
            pNode = pNode->lchild;
        }
        else if (STRCMP(var, pNode->data) > 0)
        {
            qNode = pNode;
            pNode = pNode->rchild;
        }
        else
        {
            new_ptr = pNode;
        }
    }                           /* End of while loop */
    /* Phase 2 */
    new_ptr = (CliTreeNode *) malloc(sizeof(CliTreeNode));
    if (!new_ptr)
    {
          printf("ERR: CLI INSERT TOKEN :allocation failure \r\n");
          return NULL;
    }
    strcpy(new_ptr->data, var);

    new_ptr->status = mode;
    new_ptr->callBackFn = callBackFn;
    new_ptr->nextTree = NULL;

    new_ptr->lchild = 0;
    new_ptr->rchild = 0;
    new_ptr->balFactor = 0;

    if (STRCMP(var, qNode->data) < 0)
    {
        qNode->lchild = new_ptr;
    }
    else
    {
        qNode->rchild = new_ptr;
    }
    /* Adjust bal Factors. */
    if (STRCMP(var, aNode->data) > 0)
    {
        pNode = aNode->rchild;
        bNode = pNode;
        d = -1;
    }
    else
    {
        pNode = aNode->lchild;
        bNode = pNode;
        d = +1;
    }
    while (pNode != new_ptr)
    {
        if (STRCMP(var, pNode->data) > 0)
        {
            pNode->balFactor = -1;
            pNode = pNode->rchild;
        }
        else
        {
            pNode->balFactor = +1;
            pNode = pNode->lchild;
        }
    }                           /* while loop */
    /* Is tree Unbalanced. */

    if (!aNode->balFactor)
    {
        aNode->balFactor = d;
        return (&(new_ptr->nextTree));
    }
    if (!(aNode->balFactor + d))
    {
        aNode->balFactor = 0;
        return (&(new_ptr->nextTree));
    }

    if (d == 1)
    {
        if (bNode->balFactor == 1)
        {
            aNode->lchild = bNode->rchild;
            bNode->rchild = aNode;
            aNode->balFactor = bNode->balFactor = 0;
        }
        else
        {
            cNode = bNode->rchild;
            bNode->rchild = cNode->lchild;
            aNode->lchild = cNode->rchild;
            cNode->lchild = bNode;
            cNode->rchild = aNode;

            if (cNode->balFactor == 1)
            {
                aNode->balFactor = -1;
                bNode->balFactor = 0;
            }
            else if (cNode->balFactor < 0)
            {
                aNode->balFactor = 0;
                bNode->balFactor = +1;
            }
            else if (!cNode->balFactor)
            {
                bNode->balFactor = 0;
                aNode->balFactor = 0;
            }
            cNode->balFactor = 0;
            bNode = cNode;
        }                       /* else */
    }                           /*if d */
    else
        /*if d */
    {
        if (bNode->balFactor == -1)
        {
            aNode->rchild = bNode->lchild;
            bNode->lchild = aNode;
            aNode->balFactor = bNode->balFactor = 0;
        }
        else
        {
            cNode = bNode->lchild;
            bNode->lchild = cNode->rchild;
            aNode->rchild = cNode->lchild;
            cNode->rchild = bNode;
            cNode->lchild = aNode;

            if (cNode->balFactor == 1)
            {
                aNode->balFactor = 0;
                bNode->balFactor = -1;
            }
            else if (cNode->balFactor < 0)
            {
                aNode->balFactor = 1;
                bNode->balFactor = 0;
            }
            else if (!cNode->balFactor)
            {
                bNode->balFactor = 0;
                aNode->balFactor = 0;
            }
            cNode->balFactor = 0;
            bNode = cNode;
        }                       /* else */
    }

    if (!fNode)
    {
        (*rootPtr) = bNode;
    }
    else if (aNode == fNode->lchild)
    {
        fNode->lchild = bNode;
    }
    else if (aNode == fNode->rchild)
    {
        fNode->rchild = bNode;
    }
    return (&(new_ptr->nextTree));
}

/***************************************************************************
** Function: InsertCommand
**           Inserting commands in the CLI tree.
** Inputs:   Command,its mode and its function.
**
** Outputs:  Success or Failure.
**************************************************************************/
int InsertCommand(const char *cmd,int mode, funcPtr callBackFn)
{
    char *var1, *varNext;
    CliTreeNode **tmproot;

    if(mode == ENGG_MODE) 
    {
      ;
    }
    else if (mode == SUPERVISORY_MODE) 
    {
      mode |= ENGG_MODE;
    }
    else if (mode == USER_MODE) 
    {
      mode |= ENGG_MODE;
      mode |= SUPERVISORY_MODE;
    }

     strcpy(str,cmd);

    if (!(var1 = strtok(str, " \t")))
    {
        return EMPTY_STRING;
    }
    tmproot = &cli_root;
    while (var1)
    {
        varNext = strtok(NULL, " \t");
        if ((tmproot = InsertToken(var1,mode, tmproot,varNext ? 0: callBackFn)) == NULL)
            return CLI_FAILURE;
        var1 = varNext;
    }
    return CLI_SUCCESS;
}
/***************************************************************************
** Function: cleanup_clitree
**
** Inputs:   Cli root ptr.
**
** Outputs: None.
**************************************************************************/
static void cleanup_clitree(CliTreeNode * root)
{
    if (!root)
    {
        if (root->lchild)
            cleanup_clitree(root->lchild);
        if (root->rchild)
            cleanup_clitree(root->rchild);
        if (root->nextTree)
            cleanup_clitree(root->nextTree);
        free(root);
    }
}
/***************************************************************************
** Function: CliCleanup
**
** Inputs:   None.
**
** Outputs:  None.
**************************************************************************/

void CliCleanup(void)
{
	free(inputArr);
    cleanup_clitree(cli_root);
}

/***************************************************************************
** Function: ParseACommand
**
** Inputs:   String i.e. Cmd.
**
** Outputs: Success or retvalue.
**************************************************************************/

int ParseACommand(const char *str)
{
	char *tmp;
    char *var1;
    CliTreeNode *tmpRoot;
    char cmd[CMD_SIZE+1];
    int retval, i;

    args[0] = 0;
    tmpRoot = cli_root;
    indexer = -1;

    if(inputArr == NULL)
    {
        return CLI_SUCCESS;
    }

	for(i = 0;i< MAX_PARAMETERS;i++)
      inputArr[i] = NULL;
    strcpy(cmd,str);

    if (!cli_root)
    {
        return EMPTY_TREE;
    }
    if (!(var1 = strtok(cmd, " \t")))
    {
        return EMPTY_STRING;
    }
    if (!strcmp(var1, "?"))
    {
        tmp = strtok(NULL, " \t");
        if (!tmp)
        {
            PrintErr(NOTHING);
            InOrder(cli_root,0);
            printf("\n\n");
            return CLI_SUCCESS;
        }
        PrintErr(UNKNOWN_TOKEN);
        return CLI_FAILURE;
    }
    while (var1)
    {
        found = 0;
        retval = SearchInTree(tmpRoot, var1);
        tmp = var1;
        var1 = strtok(NULL, " \t");
        if (retval == SUCC_CALLBACK)
        {
            if (var1)
                PrintErr(MORE_ARGUMENTS);
            else
               {
                  retval = (nextRoot->callBackFn)(inputArr);
                  if(retval != CLI_SUCCESS)
                    printf("Invalid Parameter(s)\n");
               }
            break;
        }
        else if (retval == SUCC_NEXTTREE)
        {
            if (!var1)
            {
                PrintErr(FEWER_ARGUMENTS);
                InOrder(nextRoot, 0);
                printf("\n");
                retval = FEWER_ARGUMENTS;
                break;
            }
            tmpRoot = nextRoot;
        }
        else if (retval == ERR_MULTI_TOKENS)
        {
            PrintErr(MULTIPLE_TOKENS);
            InOrder(tmpRoot, 0);
            printf("\n");
            retval = MULTIPLE_TOKENS;
            break;
        }
        else if (retval == CLI_FAILURE)
        {
            if (nomem)
                break;
            if(!strcmp(tmp, "?"))
            {
                if(!var1)
                   PrintErr(NOTHING);
                else
                   PrintErr(UNKNOWN_TOKEN);
            }
            else
               PrintErr(UNKNOWN_TOKEN);
            InOrder(tmpRoot, 0);
            printf("\n");
            break;
        }
        else if (retval == ERR_IMPLEMENTATION)
        {
            PrintErr(SUICIDAL);
            break;
        }
        else if (retval == ERR_PRIVILEGE)
        {
            PrintErr(NO_PERMISSION);
            break;
        }
        else if (retval == MEM_FAILURE)
        {
            break;
        }
    }
    for (i = 0; i <= indexer; ++i)
        free(inputArr[i]);
    if(retval != SUCC_CALLBACK)
        printf("\n");
    return retval == SUCC_CALLBACK ? CLI_SUCCESS : retval;
}
/***************************************************************************
** Function: PrintErr
**
** Inputs:
**
** Outputs:
**************************************************************************/
void PrintErr(int x)
{
    char string[64];

  switch(x)
    {
       case MORE_ARGUMENTS:
             printf("ERR: Too Long a command\n");
             break;
       case FEWER_ARGUMENTS:
             printf("ERR: More Arguments Expected\n");
             TAKE_CARE_OF_ARGS();
             break;
       case MULTIPLE_TOKENS:
             printf("ERR: More than one token matches input\n");
             TAKE_CARE_OF_ARGS();
             break;
       case UNKNOWN_TOKEN:
             sprintf(string, "Unknown token\n");
             printf(string);
             TAKE_CARE_OF_ARGS();
             break;
       case NO_PERMISSION:
             printf("ERR: Not enough permission to execute this command\n");
             break;
       case NOTHING:
             TAKE_CARE_OF_ARGS();
              break;
       case SUICIDAL:
             printf("ERR: Something wrong with CLI\n");
             break;
	   default:
	      ;
     }
}
/***************************************************************************
** Function: SearchInTree
**           search for valid token in the cli tree.
** Inputs: Cli rootptr and token.
**
** Outputs: Success or return value.

**************************************************************************/
int SearchInTree(CliTreeNode * rootPtr, char *var)
{
    int retval;
    static CliTreeNode *tmpPtr;

#define RETURN_APPROPRIATE      {\
        if ( ((rootPtr->callBackFn)!=0))  {\
                if (!(current_mode & rootPtr->status)){\
                        return ERR_PRIVILEGE;}\
                nextRoot = rootPtr; {\
               return SUCC_CALLBACK;}\
        }\
        else if ((nextRoot = rootPtr->nextTree))        \
                return SUCC_NEXTTREE;\
        return ERR_IMPLEMENTATION;\
       }

    nomem = 0;
    if (rootPtr)
    {
        if ((rootPtr->status & INPUT_MODE) == INPUT_MODE)
        {
            indexer++;
            inputArr[indexer] = (char *) malloc(strlen(var) + 1);
            if(inputArr[indexer] == NULL)
            {
             return MEM_FAILURE;
            }
            strcpy(inputArr[indexer], var);

      	   RETURN_APPROPRIATE
        }
        retval = strncmp(var, rootPtr->data, strlen(var));

       if (nomem)
            return MEM_FAILURE;
        if (retval < 0)
        {
            retval = SearchInTree(rootPtr->lchild, var);
            if (retval == ERR_MULTI_TOKENS || retval == MEM_FAILURE)
              return retval;
        }
        else if (retval > 0)
        {
            retval = SearchInTree(rootPtr->rchild, var);
            if (retval == ERR_MULTI_TOKENS || retval == MEM_FAILURE)
              return retval;
        }
        else
        {
            if (strlen(var) == strlen(rootPtr->data))
			{
			   RETURN_APPROPRIATE
		    }
            else
            {
                int retval1;

                if (found)
                    return ERR_MULTI_TOKENS;
                found = 1;
                tmpPtr = rootPtr;
                retval1 = SearchInTree(rootPtr->lchild, var);
                if (retval1 == ERR_MULTI_TOKENS || retval1 == MEM_FAILURE)
                        return retval1;
                retval1 = SearchInTree(rootPtr->rchild, var);
                if (retval1 == ERR_MULTI_TOKENS || retval1 == MEM_FAILURE)
                    return retval1;
			   RETURN_APPROPRIATE
            }
        }
    }
    else
        return CLI_FAILURE;
}

/***************************************************************************
** Function: InOrder
**           Inorder traversal of Cli tree.
** Inputs:   Cli root.
**
** Outputs:  None.
**************************************************************************/
static void InOrder(CliTreeNode *root, int tabCnt)
{
	int accessmode;
	cmdtabCnt = tabCnt;

    if (root)
    {
        InOrder(root->lchild, cmdtabCnt);
		accessmode = current_mode;
        if(accessmode & root->status)
        {
            if (strlen(args) + strlen(root->data) + strlen(" ") >= MAX_LEN_ARGS)
                return;
    		printf("%-20s",root->data);
    		cmdtabCnt++;
    		if(cmdtabCnt >= 3)
    		{
				cmdtabCnt=0;
				printf("\n");
			}

        }
        InOrder(root->rchild, cmdtabCnt);
    }
}
/***************************************************************************
** Function: CLI_Input
**
** Inputs:
**
** Outputs:
**************************************************************************/
int CLI_Input(char *input_str, unsigned char max_num_chars)
{
    int i = 0;
	char get_input = 1, get_char;

	do
	{
		if(echo_char == 1)
	        input_str[i] = getchar();
	    else
	    {
			get_char = _getch();
			if(i > 0)
            {
                if(get_char == 8)
			    {
			       --i;
                   input_str[i] = '\0';
                   putchar(8);
				   putchar(' ');
				   putchar(8);
			    }
            }
		    if (get_char != 8 && get_char != 13)
            {
                input_str[i] = get_char;
                putchar('*');
				++i;
            }
	    }
		if(i + 1 >= max_num_chars || get_char == 13)
	    {
            get_input = 0;
	        input_str[i] = '\0';
		    putchar('\n');
	    }
	} while(get_input == 1);
	//printf("\n PWD : %s \n", input_str);
	return CLI_SUCCESS;
}
/***************************************************************************
** Function: mystrdup
**
** Inputs:   a string.
**
** Outputs:  a copy of input string.
**************************************************************************/
char *mystrdup(const char *str)
{
    char *dup;
    dup = (char *) malloc(strlen(str) + 1);
    if (!dup)
        {
         printf("CLI MYSTRDUP: allocation failure\r\n");
         return NULL;
        }
    strcpy(dup,(char *)str);
    return dup;
}

/***************************************************************************
** Function: strmycasecmp
**
** Inputs:   strings and length.
**
** Outputs:  as same as strcmp output.
**************************************************************************/
int strmycasecmp(const char *str1, const char *str2)
{
    char *tmp1, *tmp2;
    int i;

    tmp1 = mystrdup(str1);
    tmp2 = mystrdup(str2);
    if (!tmp1 || !tmp2)
    {
        nomem = 1;
        return 0;
    }
    for (i = 0; i < strlen(str1); ++i)
        *(tmp1 + i) = tolower(str1[i]);
    for (i = 0; i < strlen(str2); ++i)
        *(tmp2 + i) = tolower(str2[i]);
    i = strcmp(tmp1, tmp2);
    free(tmp1);
    free(tmp2);
    return i;
}
