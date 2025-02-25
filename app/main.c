#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "../src/lab.h"

/* Helper function to explain the wait status of a child process */
static void explain_waitpid(int status)
{
    if (!WIFEXITED(status))
        fprintf(stderr, "Child exited with status %d\n", WEXITSTATUS(status));
    if (WIFSIGNALED(status))
        fprintf(stderr, "Child terminated by signal %d\n", WTERMSIG(status));
    if (WIFSTOPPED(status))
        fprintf(stderr, "Child stopped by signal %d\n", WSTOPSIG(status));
    if (WIFCONTINUED(status))
        fprintf(stderr, "Child continued after SIGCONT\n");
}

int main(int argc, char *argv[])
{
    parse_args(argc, argv);
    struct shell sh;
    sh_init(&sh);
    char *line = NULL;
    while ((line = readline(sh.prompt))) {
        /* Trim the input; if blank, free and continue */
        line = trim_white(line);
        if (!*line) {
            free(line);
            continue;
        }
        add_history(line);
        char **cmd = cmd_parse(line);
        if (!do_builtin(&sh, cmd)) {
            pid_t pid = fork();
            if (pid == 0) {
                /* Child process: set up process group and restore default signals */
                pid_t child = getpid();
                setpgid(child, child);
                tcsetpgrp(sh.shell_terminal, child);
                signal(SIGINT, SIG_DFL);
                signal(SIGQUIT, SIG_DFL);
                signal(SIGTSTP, SIG_DFL);
                signal(SIGTTIN, SIG_DFL);
                signal(SIGTTOU, SIG_DFL);
                execvp(cmd[0], cmd);
                perror("execvp failed");
                exit(EXIT_FAILURE);
            }
            else if (pid < 0) {
                perror("fork failed");
                abort();
            }
            /* Parent process: put child in its own group and wait for it */
            setpgid(pid, pid);
            tcsetpgrp(sh.shell_terminal, pid);
            int status;
            if (waitpid(pid, &status, 0) == -1) {
                perror("waitpid failed");
                explain_waitpid(status);
            }
            cmd_free(cmd);
            /* Return terminal control to shell */
            tcsetpgrp(sh.shell_terminal, sh.shell_pgid);
        }
        free(line);
    }
    sh_destroy(&sh);
    return 0;
}
