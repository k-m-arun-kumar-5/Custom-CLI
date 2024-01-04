#include "cli.h"
#include "cli_cmd.h"

#define NUM_MAX_CHARS   (10)

/***************************************************************************
** Function: logincheck
**           Checks for login.
** Inputs:   login name.
**
** Outputs:  Success or Failure.
**************************************************************************/
int logincheck(char **args)
{
    int i;
    char uid[25];

    sscanf(args[0],"%s",uid);
	for (i = 0; i < NMODES; ++i)
	{
		if (!strcmp(usermodes[i].userid, uid))
		{
			if(usermodes[i].mode == current_mode)
			{
				printf("You are already in %s mode\n", usermodes[i].userid);
				return CLI_SUCCESS;
			}
			break;
		}
	}
    if (i == NMODES)
    {
        printf("ERR: No such login mode exists\r\n");
        return CLI_FAILURE;
    }
    printf("Password:");
    {
		/* CLI terminal has triggered for login, take action only in CLI terminal */
       CLILoginCheck(i);
       return CLI_SUCCESS;
    }
    return CLI_SUCCESS;
}
/***************************************************************************
** Function: CLILoginCheck
**
** Inputs:
**
** Outputs:
**************************************************************************/
int CLILoginCheck(int accessmode)
{
    char string[MAX_PWD_LEN];

   	DISABLEECHO();
    CLI_Input(string, MAX_PWD_LEN);
    ENABLEECHO();

    if (!strcmp(string, usermodes[accessmode].passwd))
    {
        current_mode = usermodes[accessmode].mode;
        ChangePrompt(usermodes[accessmode].prompt);

       /* sprintf(string, "[INFO]: Changing to mode %s \r\n", usermodes[accessmode].name);
        printf(string); */
    }
    else
    {
        printf("INFO: Login Incorrect\r\n");
    }
    return CLI_SUCCESS;
}

/***************************************************************************
** Function: passwdcheck
**           Checks pasword.
** Inputs:   passwd string.
**
** Outputs: Success or Failure.
**************************************************************************/
int passwdcheck(char **args)
{
    int i = 0;
    char uid[25];
	int CLIAccessPasswdChg(int accessModeId);

    sscanf(args[0],"%s",uid );
    for (i = 0; i < NMODES; ++i)
        if (!strcmp(usermodes[i].userid, uid))
            break;
    if (i == NMODES)
    {
        printf("[ERR]: No such user exists\r\n");
        return CLI_FAILURE;
    }
	if(!strcmp(uid, ENGG_MODE_ID) && (current_mode == SUPERVISORY_MODE))
	{
		printf("ERR: Invalid Use.....\r\n");
		return CLI_FAILURE;
	}
    printf("Old Password:");
    CLIAccessPasswdChg(i);
    return CLI_SUCCESS;
}
/***************************************************************************
** Function: CLIAccessPasswdChg
**
** Inputs:
**
** Outputs:
**************************************************************************/
int CLIAccessPasswdChg(int accessmode)
{
    char ostring[MAX_PWD_LEN];

    DISABLEECHO();
    CLI_Input(ostring, MAX_PWD_LEN);
    ENABLEECHO();

    if (!strcmp(ostring, usermodes[accessmode].passwd))
    {
        char nstring[MAX_PWD_LEN], rstring[MAX_PWD_LEN];
		printf("New Password: ");
        DISABLEECHO();
		CLI_Input(nstring, MAX_PWD_LEN);
		ENABLEECHO();
		if(strlen(nstring) < MIN_PWD_LEN)
		{
			printf("ERR: Password < %u \r\n", MIN_PWD_LEN);
			return CLI_FAILURE;
		}
		printf("Retype New Password:");
		DISABLEECHO();
		CLI_Input(rstring, MAX_PWD_LEN);
		ENABLEECHO();
		if(strlen(rstring) < MIN_PWD_LEN)
		{
			printf("ERR: Password < %u \r\n", MIN_PWD_LEN);
			return CLI_FAILURE;
		}
		strcpy(usermodes[accessmode].passwd, nstring);
		printf("INFO: Pwd successfully changed \r\n");
    }
    else
    {
        printf("INFO: Login Incorrect\r\n");
		return CLI_FAILURE;
    }
    return CLI_SUCCESS;
}

/***************************************************************************
** Function: modenames
**
** Inputs:   cmd line argument.
**
** Outputs: Success.
**************************************************************************/
int modenames(char **args)
{
    char string[128];
    int i = 0;

    string[0] = '\0';
    for (; i < NMODES; ++i)
    {
        strcat(string, usermodes[i].userid);
        strcat(string, " ");
    }
    strcat(string, "\r\n");
    printf(string);
    return CLI_SUCCESS;
}

/***************************************************************************
** Function: whichmode
**
** Inputs:   cmd line arg.
**
** Outputs: Success.
**************************************************************************/
int whichmode(char **args)
{
    int i = 0;
    char string[128];

    for (i = 0; i < NMODES; ++i)
       if (usermodes[i].mode == current_mode)
             break;
    sprintf(string, "You are now in %s mode\r\n", usermodes[i].userid);
    printf(string);

    return CLI_SUCCESS;
}

/***************************************************************************
** Function: exitmode
**
** Inputs:   cmd line arg.
**
** Outputs:  Success.
**************************************************************************/
int exitmode(char **argv)
{
    exit_flag = 1;
	printf("Exit from Application\n");
    return CLI_SUCCESS;
}
/***************************************************************************
** Function: SetSystemPrompt
**
** Inputs:
**
** Outputs:
**************************************************************************/
int SetSystemPrompt(char **argv)
{
    int i=0;

       for (i = 0; i < NMODES; ++i)
         if (usermodes[i].mode == current_mode )
            break;

       if (strlen(argv[0]) > (CLI_PROMPT_LEN -1))
       return CLI_FAILURE;

       strcpy(usermodes[i].prompt,argv[0]);

       ChangePrompt(usermodes[i].prompt);

   return CLI_SUCCESS;
}
/***************************************************************************
** Function: RegisterCli
**
** Inputs:
**
** Outputs:
**************************************************************************/
int RegisterCli()
{
	if(InsertCommand("<num1> + <num2>", START_MODE, num_addition) == CLI_FAILURE)
		return CLI_FAILURE;
	if(InsertCommand("<num1> - <num2>", START_MODE, num_subtraction) == CLI_FAILURE)
		return CLI_FAILURE;
	if(InsertCommand("<num1> * <num2>", START_MODE, num_multiplication) == CLI_FAILURE)
		return CLI_FAILURE;
	if(InsertCommand("<num1> / <num2>", START_MODE, num_division) == CLI_FAILURE)
		return CLI_FAILURE;
	if(InsertCommand("<num1> % <num2>", START_MODE, num_modulus) == CLI_FAILURE)
		return CLI_FAILURE;
	return CLI_SUCCESS;
}

/***************************************************************************
** Function: num_addition
**
** Inputs:
**
** Outputs:
**************************************************************************/
int num_addition(char **args)
{
    #define ADD_NUM_CHARS  (10)
	char num1_str[ADD_NUM_CHARS], num2_str[ADD_NUM_CHARS], result_str[ADD_NUM_CHARS + 1], *ptr;
	double num1, num2, result;

    if(strlen(args[0]) >= ADD_NUM_CHARS || strlen(args[1]) >= ADD_NUM_CHARS )
    {
        printf("ERR: Num chars exceeds %d\n", ADD_NUM_CHARS);
        return CLI_SUCCESS;
    }
	sscanf(args[0], "%s", num1_str);
	sscanf(args[1], "%s", num2_str);
	num1 = strtod(num1_str, &ptr);
	if(strcmp(ptr, ""))
	{
		printf("ERR: invalid num1: %s \n", num1_str);
		return CLI_SUCCESS;
	}
	num2 = strtod(num2_str, &ptr);
	if(strcmp(ptr, ""))
	{
		printf("ERR: invalid num2: %s \n", num2_str);
		return CLI_SUCCESS;
	}
	result = num1 + num2;
	sprintf(result_str, "%lf", result);
	printf("%s + %s = %s\n", num1_str, num2_str, result_str);
	return CLI_SUCCESS;
}
/***************************************************************************
** Function: num_subtraction
**
** Inputs:
**
** Outputs:
**************************************************************************/
int num_subtraction(char **args)
{
    #define SUB_NUM_CHARS  (10)
	char num1_str[SUB_NUM_CHARS], num2_str[SUB_NUM_CHARS], result_str[SUB_NUM_CHARS + 1], *ptr;
	double num1, num2, result;

    if(strlen(args[0]) >= SUB_NUM_CHARS || strlen(args[1]) >= SUB_NUM_CHARS )
    {
        printf("ERR: Num chars exceeds %d \n", SUB_NUM_CHARS);
        return CLI_SUCCESS;
    }
	sscanf(args[0], "%s", num1_str);
	sscanf(args[1], "%s", num2_str);
	num1 = strtod(num1_str, &ptr);
	if(strcmp(ptr, ""))
	{
		printf("ERR: invalid num1: %s \n", num1_str);
		return CLI_SUCCESS;
	}
	num2 = strtod(num2_str, &ptr);
	if(strcmp(ptr, ""))
	{
		printf("ERR: invalid num2: %s \n", num2_str);
		return CLI_SUCCESS;
	}
	result = num1 - num2;
	sprintf(result_str, "%lf", result);
	printf("%s - %s = %s\n", num1_str, num2_str, result_str);
	return CLI_SUCCESS;
}

/***************************************************************************
** Function: num_multiplication
**
** Inputs:
**
** Outputs:
**************************************************************************/
int num_multiplication(char **args)
{
    #define MUL_NUM_CHARS  (8)
	char num1_str[MUL_NUM_CHARS], num2_str[MUL_NUM_CHARS], result_str[MUL_NUM_CHARS + 4], *ptr;
	double num1, num2, result;

    if(strlen(args[0]) >= MUL_NUM_CHARS || strlen(args[1]) >= MUL_NUM_CHARS )
    {
        printf("ERR: Num chars exceeds %d \n", MUL_NUM_CHARS);
        return CLI_SUCCESS;
    }
	sscanf(args[0], "%s", num1_str);
	sscanf(args[1], "%s", num2_str);
	num1 = strtod(num1_str, &ptr);
	if(strcmp(ptr, ""))
	{
		printf("ERR: Invalid num1: %s \n", num1_str);
		return CLI_SUCCESS;
	}
	num2 = strtod(num2_str, &ptr);
	if(strcmp(ptr, ""))
	{
		printf("ERR: Invalid num2: %s \n", num2_str);
		return CLI_SUCCESS;
	}
	result = num1 * num2;
	sprintf(result_str, "%lf", result);
	printf("%s * %s = %s\n", num1_str, num2_str, result_str);
	return CLI_SUCCESS;
}
/***************************************************************************
** Function: num_division
**
** Inputs:
**
** Outputs:
**************************************************************************/
int num_division(char **args)
{
    #define DIV_NUM_CHARS  (8)
	char num1_str[DIV_NUM_CHARS], num2_str[DIV_NUM_CHARS], result_str[DIV_NUM_CHARS + 4], *ptr;
	double num1, num2, result;

    if(strlen(args[0]) >= DIV_NUM_CHARS || strlen(args[1]) >= DIV_NUM_CHARS )
    {
        printf("ERR: Num chars exceeds %d \n", DIV_NUM_CHARS);
        return CLI_SUCCESS;
    }
	sscanf(args[0], "%s", num1_str);
	sscanf(args[1], "%s", num2_str);
	num1 = strtod(num1_str, &ptr);
	if(strcmp(ptr, ""))
	{
		printf("ERR: Invalid num1: %s \n", num1_str);
		return CLI_SUCCESS;
	}
	num2 = strtod(num2_str, &ptr);
	if(strcmp(ptr, ""))
	{
		printf("ERR: Invalid num2: %s \n", num2_str);
		return CLI_SUCCESS;
	}
	result = num1 / num2;
	sprintf(result_str, "%lf", result);
	printf("%s / %s = %s\n", num1_str, num2_str, result_str);
	return CLI_SUCCESS;
}

/***************************************************************************
** Function: num_modulus
**
** Inputs:
**
** Outputs:
**************************************************************************/
int num_modulus(char **args)
{
    #define MOD_NUM_CHARS  (8)
	char num1_str[MOD_NUM_CHARS], num2_str[MOD_NUM_CHARS], result_str[MOD_NUM_CHARS], *ptr;
	long int num1, num2, result;

    if(strlen(args[0]) >= MOD_NUM_CHARS || strlen(args[1]) >= MOD_NUM_CHARS )
    {
        printf("ERR: Num chars exceeds %d \n", MOD_NUM_CHARS);
        return CLI_SUCCESS;
    }
	sscanf(args[0], "%s", num1_str);
	sscanf(args[1], "%s", num2_str);
	num1 = strtol(num1_str, &ptr, 10);
	if(strcmp(ptr, ""))
	{
		printf("ERR: Invalid num1: %s \n", num1_str);
		return CLI_SUCCESS;
	}
	num2 = strtol(num2_str, &ptr, 10);
	if(strcmp(ptr, ""))
	{
		printf("ERR: Invalid num2: %s \n", num2_str);
		return CLI_SUCCESS;
	}
	result = num1 % num2;
	sprintf(result_str, "%ld", result);
	printf("%s %% %s = %s\n", num1_str, num2_str, result_str);
	return CLI_SUCCESS;
}
