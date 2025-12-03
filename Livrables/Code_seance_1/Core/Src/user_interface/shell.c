/**
 * @file shell.c
 * @brief Implementation file for the shell module.
 *
 * This file contains the implementation of the shell module, which provides a command-line interface.
 */

#include "user_interface/shell.h"

h_shell_t hshell1;

/**
 * @brief Transmit function for shell driver using UART2
 */



/**
 * @brief Checks if a character is valid for a shell command.
 *
 * This function checks if a character is valid for a shell command. Valid characters include alphanumeric characters and spaces.
 *
 * @param c The character to check.
 * @return 1 if the character is valid, 0 otherwise.
 */
static int is_character_valid(char c)
{
	return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || (c == ' ') || (c == '=');
}

static int is_string_valid(char* str)
{
	int reading_head = 0;
	while(str[reading_head] != '\0'){
		//		char c = str[reading_head];
		if(!is_character_valid(str[reading_head])){
			if(reading_head == 0){
				return 0;
			}
			else{
				str[reading_head] = '\0';
				return 1;
			}
		}
		reading_head++;
	}
	return 1;
}

/**
 * @brief Help command implementation.
 *
 * This function is the implementation of the help command. It displays the list of available shell commands and their descriptions.
 *
 * @param h_shell The pointer to the shell instance.
 * @param argc The number of command arguments.
 * @param argv The array of command arguments.
 * @return 0 on success.
 */
static int sh_help(h_shell_t* h_shell, int argc, char** argv)
{
	int i, size;
	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Code \t | Description \r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);
	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "----------------------\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);

	for (i = 0; i < h_shell->func_list_size; i++){
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "%s \t | %s\r\n", h_shell->func_list[i].string_func_code, h_shell->func_list[i].description);
		h_shell->drv.transmit(h_shell->print_buffer, size);
	}
	return 0;
}

static int sh_test_list(h_shell_t* h_shell, int argc, char** argv)
{
	int size;
	for(int arg=0; arg<argc; arg++){
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "Arg %d \t %s\r\n", arg, argv[arg]);
		h_shell->drv.transmit(h_shell->print_buffer, size);
	}
	return 0;
}




/**
 * @brief Initializes the shell instance.
 *
 * This function initializes the shell instance by setting up the internal data structures and registering the help command.
 *
 * @param h_shell The pointer to the shell instance.
 */
void shell_init(h_shell_t* h_shell)
{
	int size = 0;

	h_shell->func_list_size = 0;

	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "\r\n=> Monsieur Shell v0.2.2 without FreeRTOS <=\r\n");
	h_shell->drv.transmit(h_shell->print_buffer, size);
	h_shell->drv.transmit(PROMPT, sizeof(PROMPT));

	shell_add(h_shell, "help", sh_help, "Help");
	shell_add(h_shell, "test", sh_test_list, "Test list");
}

/**
 * @brief Adds a shell command to the instance.
 *
 * This function adds a shell command to the shell instance.
 *
 * @param h_shell The pointer to the shell instance.
 * @param c The character trigger for the command.
 * @param pfunc Pointer to the function implementing the command.
 * @param description The description of the command.
 * @return 0 on success, or a negative error code on failure.
 */
int shell_add(h_shell_t* h_shell, char* string_func_code, shell_func_pointer_t pfunc, char* description)
{
	if(is_string_valid(string_func_code))
	{
		if (h_shell->func_list_size < SHELL_FUNC_LIST_MAX_SIZE)
		{
			h_shell->func_list[h_shell->func_list_size].string_func_code = string_func_code;
			h_shell->func_list[h_shell->func_list_size].func = pfunc;
			h_shell->func_list[h_shell->func_list_size].description = description;
			h_shell->func_list_size++;
			return 0;
		}
	}
	return -1;
}

/**
 * @brief Executes a shell command.
 *
 * This function executes a shell command based on the input buffer.
 *
 * @param h_shell The pointer to the shell instance.
 * @param buf The input buffer containing the command.
 * @return 0 on success, or a negative error code on failure.
 */
static int shell_exec(h_shell_t* h_shell, char* buf)
{
	int i, argc;
	char* argv[SHELL_ARGC_MAX];
	char* p;

	// Create argc, argv**
	argc = 1;
	argv[0] = buf;
	for (p = buf; *p != '\0' && argc < SHELL_ARGC_MAX; p++)
	{
		if (*p == ' ')
		{
			*p = '\0';
			argv[argc++] = p + 1;
		}
	}

	for (i = 0; i < h_shell->func_list_size; i++)
	{
		if(strcmp(h_shell->func_list[i].string_func_code, argv[0])==0)
		{

			return h_shell->func_list[i].func(h_shell, argc, argv);
		}
	}

	int size;
	size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "%s : no such command\r\n", argv[0]);
	h_shell->drv.transmit(h_shell->print_buffer, size);
	return -1;
}

/**
 * @brief Runs the shell.
 *
 * This function runs the shell, processing user commands.
 *
 * @param h_shell The pointer to the shell instance.
 * @return Never returns, it's an infinite loop.
 */
int shell_run(h_shell_t* h_shell)
{
	static int cmd_buffer_index;
	char c;
	int size;

	h_shell->drv.receive(&c, 1);

	switch(c)
	{
	case '\r': // Process RETURN key
		//case '\n':
		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, "\r\n");
		h_shell->drv.transmit(h_shell->print_buffer, size);
		h_shell->cmd_buffer[cmd_buffer_index++] = 0; // Add '\0' char at the end of the string
		//		size = snprintf(h_shell->print_buffer, SHELL_PRINT_BUFFER_SIZE, ":%s\r\n", h_shell->cmd_buffer);
		//		h_shell->drv.transmit(h_shell->print_buffer, size);
		cmd_buffer_index = 0; // Reset buffer
		shell_exec(h_shell, h_shell->cmd_buffer);
		h_shell->drv.transmit(PROMPT, sizeof(PROMPT));
		break;

	case '\b': // Backspace
		if (cmd_buffer_index > 0) // Is there a character to delete?
		{
			h_shell->cmd_buffer[cmd_buffer_index] = '\0'; // Removes character from the buffer, '\0' character is required for shell_exec to work
			cmd_buffer_index--;
			h_shell->drv.transmit("\b \b", 3); // "Deletes" the character on the terminal
		}
		break;

	default: // Other characters
		// Only store characters if the buffer has space
		if (cmd_buffer_index < SHELL_CMD_BUFFER_SIZE)
		{
			if (is_character_valid(c))
			{
				h_shell->drv.transmit(&c, 1); // echo
				h_shell->cmd_buffer[cmd_buffer_index++] = c; // Store
			}
		}
	}
	return 0;
}
