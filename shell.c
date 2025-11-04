#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <string.h>

/* Using write(int file_descriptor, const void buf[count], size_t count)
 * and read(int file_descriptor, void buf[count], size_t count)
 *
 * Remember ERRNO FOR ALL SYSCALLS
 *
 * Basic structure:
 *
 * Main loop that takes input, saves it for later and executes
 *
 * Tokenize user input from a string to tokens that can be parsed
 *
 * Implement built-in functions without forking: cd, echo and exit first
 *
 * Implement external programs for commands: ls and its varieties first
 *
 * ls (almost done)
 *
 * pwd (done!)
 *
 * mv (not started)
 *
 * rm (not started)
 *
 * mkdir/rmdir (not started)
 *
 * chmod/chown (not started)
 *
 * find/grep (maybe not)
 *
 * cat (we will see)
 *
 * Data structures:
 *
 * Array for inputs - prompt 
 *
 * 
 */

struct state {
    char *curr_dir;
    char *user;
};

char *cwd(void) {
    char *buf = malloc(1000);
    getcwd(buf, sizeof(buf));
    return *buf;
}

void cd(char *new_dir, struct state *state) {
    chdir(new_dir);
    state->curr_dir = cwd();
}

void echo(char *args[]) {
    for (int i = 0; args[i] != NULL; i++) {
        printf("%s", args[i]);
    }
}

void exit_shell(void) {
    printf("exit");
    exit(EXIT_SUCCESS);
}

void pwd(struct state *state) {
    state->curr_dir = cwd();
    printf("%s", state->curr_dir);
}

void argument_parser(char *arguments, struct state *state) {
    char *args[1000] = { 0 };
    int i = 0;

    arguments[strcspn(arguments, "\n")] = '\0';

    char *token; 
    while ((token = strsep(&arguments, " ")) != NULL) {
        if (*token) {
            args[i++] = token;
        }
    }
    args[++i] = NULL;
    if (strcmp(args[0], "\n") == 0) {
        return;
    } else if (strcmp(args[0], "tcd") == 0) {
        cd(args[1], state);
        return;
    } else if (strcmp(args[0], "texit") == 0) {
        exit_shell();
        return;
    } else if (strcmp(args[0], "tpwd") == 0){
        pwd(state);
        return;
    } else {
        /*printf("%s\n", args[0]);*/
        /*printf("%s\n", args[1]);*/
        /*printf("%s\n", args[2]);*/
        if (fork() == 0) {
            execvp(args[0], args);
            exit(EXIT_FAILURE);
        } else {
            wait(NULL);
        }
    }
}

int main(void) {
    char buf[1024];
    struct stat info = { 0 };
    struct state state = { 0 };
    state.curr_dir = "/Users/erlendbleivik/devel/projects/shell";
    lstat(state.curr_dir, &info);
    struct passwd *pw = getpwuid(info.st_uid);
    state.user = pw->pw_name;

    while (1) {
        printf("%s %s  $  ", state.user, state.curr_dir);
        fgets(buf, sizeof(buf), stdin);
        argument_parser(buf, &state);
    }
}

