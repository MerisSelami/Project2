#ifndef LAB_H
#define LAB_H

#include <stdlib.h>
#include <stdbool.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

#define lab_VERSION_MAJOR 1
#define lab_VERSION_MINOR 0
#define UNUSED(x) (void)(x);

#ifdef __cplusplus
extern "C" {
#endif

struct shell {
    int shell_is_interactive;
    pid_t shell_pgid;
    struct termios shell_tmodes;
    int shell_terminal;
    char *prompt;
};

/**
 * @brief Returns the shell prompt. If the environment variable (env) is set,
 * its value is used; otherwise a default prompt "shell>" is returned.
 * The returned string is allocated on the heap and must be freed by the caller.
 *
 * @param env The name of the environment variable to check.
 * @return char* The prompt string.
 */
char *get_prompt(const char *env);

/**
 * @brief Changes the current working directory.
 * If no argument is provided (i.e. the command is just "cd"), the user's home directory is used.
 *
 * @param dir The argument vector containing the command and its arguments.
 * @return int Returns 0 on success or -1 on error.
 */
int change_dir(char **dir);

/**
 * @brief Parses a command line string into an array of tokens.
 * Tokens are separated by whitespace. The resulting array is terminated with a NULL pointer.
 * Memory for both the array and tokens is allocated on the heap.
 *
 * @param line The input command line string.
 * @return char** Array of token strings.
 */
char **cmd_parse(char const *line);

/**
 * @brief Frees the memory allocated by cmd_parse.
 *
 * @param line The array of token strings to free.
 */
void cmd_free(char ** line);

/**
 * @brief Trims leading and trailing whitespace from a string.
 * This function modifies the string in place.
 *
 * @param line The string to trim.
 * @return char* The trimmed string.
 */
char *trim_white(char *line);

/**
 * @brief Checks if the first token of the command is a built-in command (exit, cd, history).
 * If so, it handles the command and returns true. Otherwise, returns false.
 *
 * @param sh Pointer to the shell structure.
 * @param argv Array of token strings representing the command.
 * @return bool True if a built-in command was executed.
 */
bool do_builtin(struct shell *sh, char **argv);

/**
 * @brief Initializes the shell for interactive use.
 * This includes setting terminal attributes, process group control, and loading the prompt.
 *
 * @param sh Pointer to the shell structure.
 */
void sh_init(struct shell *sh);

/**
 * @brief Cleans up shell resources and restores terminal settings.
 *
 * @param sh Pointer to the shell structure.
 */
void sh_destroy(struct shell *sh);

/**
 * @brief Parses command line arguments provided at shell launch.
 * If the -v flag is found, the version is printed and the program exits.
 *
 * @param argc The number of command line arguments.
 * @param argv The argument vector.
 */
void parse_args(int argc, char **argv);

#ifdef __cplusplus
}
#endif

#endif
