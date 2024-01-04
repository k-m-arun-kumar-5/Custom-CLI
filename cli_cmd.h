#ifndef CLI_CMD_H
#define CLI_CMD_H

int RegisterCli();
int CLILoginCheck(int accessmode);
int CLIAccessPasswdChg(int accessmode);
int CLI_Input(char *input_str, unsigned char max_input_chars);
int CLIAccessPasswdChg(int accessmode);

/* must have CLI commands */
int logincheck(char **args);
int passwdcheck(char **args);
int modenames(char **args);
int exitmode(char **argv);
int SetSystemPrompt(char **argv);
int whichmode(char **args);
int ChangePrompt(char *newPrompt);

/* CLI*/
int num_addition(char **args);
int num_subtraction(char **args);
int num_multiplication(char **args);
int num_division(char **args);
int num_modulus(char **args);

#endif // CLI_CMD_H
