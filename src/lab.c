#include "lab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <pwd.h>
#include <signal.h>
#include <termios.h>
#include <getopt.h>
#include <readline/readline.h>
#include <readline/history.h>

/* Returns the prompt string based on an environment variable or a default value. */
char *get_prompt(const char *env) {
    const char *default_prompt = "shell>";
    const char *env_val = getenv(env);
    if (!env_val)
        env_val = default_prompt;
    char *prompt = malloc(strlen(env_val) + 1);
    if (!prompt) {
        fprintf(stderr, "Error allocating memory for prompt\n");
        exit(EXIT_FAILURE);
    }
    strcpy(prompt, env_val);
    return prompt;
}

/* Changes the current working directory. */
int change_dir(char **dir) {
    char *target_dir = NULL;
    if (dir[1] == NULL) {
        target_dir = getenv("HOME");
        if (!target_dir) {
            struct passwd *pw = getpwuid(getuid());
            if (pw)
                target_dir = pw->pw_dir;
            else {
                fprintf(stderr, "Cannot determine home directory.\n");
                return -1;
            }
        }
    } else {
        target_dir = dir[1];
    }
    if (chdir(target_dir) != 0) {
        perror("chdir failed");
        return -1;
    }
    return 0;
}

char **cmd_parse(const char *line) {
    if (!line)
        return NULL;
    int token_count = 0;
    int in_token = 0;
    for (const char *ptr = line; *ptr; ptr++) {
        if (!isspace((unsigned char)*ptr)) {
            if (!in_token) {
                token_count++;
                in_token = 1;
            }
        } else {
            in_token = 0;
        }
    }

    char **tokens = malloc((token_count + 1) * sizeof(char *));
    if (!tokens)
        return NULL;

    char *copy = strdup(line);
    if (!copy) {
        free(tokens);
        return NULL;
    }
    int idx = 0;
    char *token = strtok(copy, " ");
    while (token) {
        tokens[idx] = strdup(token);
        if (!tokens[idx]) {
            for (int i = 0; i < idx; i++)
                free(tokens[i]);
            free(tokens);
            free(copy);
            return NULL;
        }
        idx++;
        token = strtok(NULL, " ");
    }
    tokens[idx] = NULL;
    free(copy);
    return tokens;
}

/* Frees the memory allocated by cmd_parse. */
void cmd_free(char **line) {
    if (!line)
        return;
    for (int i = 0; line[i] != NULL; i++) {
        free(line[i]);
    }
    free(line);
}

/* Trims leading and trailing whitespace from the string.
   The trimming is done in place so the returned pointer remains valid for freeing. */
char *trim_white(char *line) {
    if (!line)
        return NULL;
    int start = 0;
    while (line[start] && isspace((unsigned char)line[start]))
        start++;
    if (start > 0) {
        int i = 0;
        while (line[start + i]) {
            line[i] = line[start + i];
            i++;
        }
        line[i] = '\0';
    }
    int len = strlen(line);
    while (len > 0 && isspace((unsigned char)line[len - 1]))
        len--;
    line[len] = '\0';
    return line;
}

/* Executes built-in commands (exit, cd, history). */
bool do_builtin(struct shell *sh, char **argv) {
    if (!argv || !argv[0])
        return false;
    if (strcmp(argv[0], "exit") == 0) {
        sh_destroy(sh);
        exit(0);
    } else if (strcmp(argv[0], "cd") == 0) {
        change_dir(argv);
        return true;
    } else if (strcmp(argv[0], "history") == 0) {
        HIST_ENTRY **hist = history_list();
        if (hist) {
            for (int i = 0; hist[i] != NULL; i++) {
                printf("%s\n", hist[i]->line);
            }
        }
        return true;
    }
    return false;
}

/* Initializes the shell structure, sets terminal control, and loads the prompt. */
void sh_init(struct shell *sh) {
    sh->shell_terminal = STDIN_FILENO;
    sh->shell_is_interactive = isatty(sh->shell_terminal);
    if (sh->shell_is_interactive) {
        while (tcgetpgrp(sh->shell_terminal) != (sh->shell_pgid = getpgrp()))
            kill(-sh->shell_pgid, SIGTTIN);
        signal(SIGINT, SIG_IGN);
        signal(SIGQUIT, SIG_IGN);
        signal(SIGTSTP, SIG_IGN);
        signal(SIGTTIN, SIG_IGN);
        signal(SIGTTOU, SIG_IGN);
        tcgetattr(sh->shell_terminal, &sh->shell_tmodes);
    }
    sh->prompt = get_prompt("MY_PROMPT");
}

/* Cleans up shell resources and restores terminal settings. */
void sh_destroy(struct shell *sh) {
    if (sh->prompt) {
        free(sh->prompt);
        sh->prompt = NULL;
    }
    tcsetattr(sh->shell_terminal, TCSANOW, &sh->shell_tmodes);
}

/* Parses command line arguments. If -v is passed, prints version and exits. */
void parse_args(int argc, char **argv) {
    int opt;
    while ((opt = getopt(argc, argv, "v")) != -1) {
        switch (opt) {
            case 'v':
                printf("lab version %d.%d\n", lab_VERSION_MAJOR, lab_VERSION_MINOR);
                exit(0);
            default:
                fprintf(stderr, "Usage: %s [-v]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }
}
