/**
 * @file shell.h
 * @brief Header file for the shell module.
 *
 * This file contains the declarations for the shell module, which provides a command-line interface.
 */

#ifndef SHELL_H_
#define SHELL_H_

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "main.h"
#include "stm32g4xx_hal.h"

#define SHELL_ARGC_MAX 8                       /**< Maximum number of arguments for a shell command. */
#define SHELL_PRINT_BUFFER_SIZE 64             /**< Size of the print buffer used for output. */
#define SHELL_CMD_BUFFER_SIZE 64               /**< Size of the command buffer used for input. */
#define SHELL_FUNC_LIST_MAX_SIZE 64            /**< Maximum number of registered shell functions. */
#define PROMPT "MSC@SAC-TP:/"

struct h_shell_struct;

/**
 * @brief Function pointer type for shell commands.
 *
 * This type represents a function pointer to a shell command implementation.
 *
 * @param h_shell The pointer to the shell instance.
 * @param argc The number of command arguments.
 * @param argv The array of command arguments.
 * @return The return value of the command execution.
 */
typedef int (*shell_func_pointer_t)(struct h_shell_struct* h_shell, int argc, char** argv);

/**
 * @brief Function type for transmitting data.
 *
 * This type represents a function pointer to a driver function for transmitting data.
 *
 * @param pData The pointer to the data to be transmitted.
 * @param size The size of the data to be transmitted.
 * @return The number of bytes transmitted.
 */
typedef uint8_t (*drv_shell_transmit_t)(const char* pData, uint16_t size);

/**
 * @brief Function type for receiving data.
 *
 * This type represents a function pointer to a driver function for receiving data.
 *
 * @param pData The pointer to the buffer to store the received data.
 * @param size The maximum size of the buffer.
 * @return The number of bytes received.
 */
typedef uint8_t (*drv_shell_receive_t)(char* pData, uint16_t size);

/**
 * @brief Structure representing a shell command.
 */
typedef struct shell_func_struct
{
    char* string_func_code;                             /**< The character trigger for the command. */
    shell_func_pointer_t func;          /**< Pointer to the function implementing the command. */
    char* description;                  /**< The description of the command. */
} shell_func_t;

/**
 * @brief Structure representing a shell driver.
 */
typedef struct drv_shell_struct
{
    drv_shell_transmit_t transmit;      /**< Function pointer for transmitting data. */
    drv_shell_receive_t receive;        /**< Function pointer for receiving data. */
} drv_shell_t;

/**
 * @brief Structure representing a shell instance.
 */
typedef struct h_shell_struct
{
    int func_list_size;                     				/**< The current size of the function list. */
    shell_func_t func_list[SHELL_FUNC_LIST_MAX_SIZE];     	/**< Array of registered shell functions. */
    char print_buffer[SHELL_PRINT_BUFFER_SIZE];           	/**< Buffer used for output. */
    char cmd_buffer[SHELL_CMD_BUFFER_SIZE];               	/**< Buffer used for input. */
    drv_shell_t drv;                        				/**< The shell driver instance. */
} h_shell_t;

extern h_shell_t hshell1;

/**
 * @brief Initializes the shell instance.
 *
 * This function initializes the shell instance by setting up the internal data structures and registering the help command.
 *
 * @param h_shell The pointer to the shell instance.
 */
void shell_init(h_shell_t* h_shell);

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
int shell_add(h_shell_t* h_shell, char* string_func_code, shell_func_pointer_t pfunc, char* description);

/**
 * @brief Runs the shell.
 *
 * This function runs the shell, processing user commands.
 *
 * @param h_shell The pointer to the shell instance.
 * @return Never returns, it's an infinite loop.
 */
int shell_run(h_shell_t* h_shell);

#endif /* SHELL_H_ */
