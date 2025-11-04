#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <string.h>

struct state {
    char *curr_dir;
    char *user;
};

char *cwd(void) {
    long size;
    char *buf;
    char *ptr;

    size = pathconf(".", _PC_PATH_MAX);
    buf = (char *)malloc((size_t)size);
    ptr = getcwd(buf, (size_t)size);
    free(buf);
    return ptr;
}

char *get_relative_path(char *path) {
    char *token; 
    char *dirs[1000] = { 0 };
    int i = 0;

    while ((token = strsep(&path, "/")) != NULL) {
        if (*token) {
            dirs[i++] = token;
        }
    }

    while (dirs[i] != NULL) {
        i++;
    }

    return dirs[--i];
}

void cd(char *new_dir, struct state *state) {
    chdir(new_dir);
    state->curr_dir = get_relative_path(cwd());
}

void echo(char *args[]) {
    for (int i = 1; args[i] != NULL; i++) {
        printf("%s", args[i]);
        printf(" ");
    }
    printf("\n");
}

void exit_shell(void) {
    printf("exit");
    exit(EXIT_SUCCESS);
}

void pwd(void) {
    char pwd[1024];
    char *ptr = getcwd(pwd, sizeof(pwd));
    puts(ptr);
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
    args[i] = NULL;
    if (args[0] == NULL) {
        return;
    }

    if (strcmp(args[0], "tcd") == 0) {
        cd(args[1], state);
        return;
    } else if (strcmp(args[0], "texit") == 0) {
        exit_shell();
        return;
    } else if (strcmp(args[0], "tpwd") == 0) {
        pwd();
        return;
    } else if (strcmp(args[0], "techo") == 0) {
        echo(args);
        return;
    } else {
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
    char *PATH;
    PATH = getenv("PATH");
    setenv("tls", PATH, 1);
    state.curr_dir = get_relative_path(cwd());
    lstat(state.curr_dir, &info);
    struct passwd *pw = getpwuid(info.st_uid);
    state.user = pw->pw_name;

    while (1) {
        printf("%s %s  $  ", state.user, state.curr_dir);
        fgets(buf, sizeof(buf), stdin);
        argument_parser(buf, &state);
    }
}

