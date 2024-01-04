#include "cli.h"

#define MENU_STR_MAX_CHARS  1

typedef enum
{
	TRUE = 0, SUCCESS = 0, FAILURE, FALSE, NO_MEMORY, ERR_NULL_PTR, ERR_INVALID_DATA
} system_status_t;

int main()
{
     char input_cli[CMD_SIZE];
      
	 printf("Welcome to my CLI shell\n\n");
     if(init_cli() != CLI_SUCCESS)
	 {
	    printf("ERR: CLI Initial failed\n");
		CliCleanup();
		return 1;
	 }
	 do
	 {
         printf("%s", prompt);
		 memset(input_cli, '\0', CMD_SIZE);
		 fgets(input_cli, CMD_SIZE, stdin);
		 input_cli[strlen(input_cli) - 1] = '\0';
		//  printf("cmd : %s", input_cli);
		 ParseACommand(input_cli);
		 fflush(stdin);
	 } while (!exit_flag);
	 //printf("INFO: CLI cleanup\n");
	 CliCleanup();
	 return 0;
}

