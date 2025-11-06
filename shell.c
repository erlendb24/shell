#include <unistd.h>
#include <stdio.h>
#include <dirent.h>
#include <stdlib.h>
#include <pwd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <string.h>

struct command_node {
    char *command;
    struct command_node *next;
    struct command_node *previous;
};

struct state {
    char *curr_dir;
    char *user;
    struct command_node *head;
    struct command_node *tail;
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
    if (chdir(new_dir) != -1) {
        state->curr_dir = get_relative_path(cwd());
    }
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
    char *ptr;
    if ((ptr = getcwd(pwd, sizeof(pwd))) != NULL) {
        puts(ptr);
    }
    return;
}

void print_command_history(struct state *state) {
    struct command_node *temp = state->head;
    printf("\n\n%s\n", "print_command_history activated");
    while (temp != NULL) {
        printf("%s\n", temp->command);
        temp = temp->next;
    }
}

void add_command(char *command, struct state *state) {
    struct command_node *new_node = malloc(sizeof(struct command_node));
    if (new_node == NULL) {
        perror("Failed to allocate memory for new command node");
        free(new_node);
        return;
    }

    new_node->command = malloc(strlen(command) + 1);
    if (new_node->command == NULL) {
        perror("Failed to allocate memory for command");
        free(new_node);
        return;
    }
    strcpy(new_node->command, command);
    new_node->next = NULL;
    new_node->previous = NULL;

    if (state->head == NULL) {
        state->head = new_node;
        return;
    }
    if (state->tail == NULL) {
        state->head->next = new_node;
        new_node->previous = state->head;
        state->tail = new_node;
        return;
    }

    new_node->previous = state->tail;
    state->tail->next = new_node;
    state->tail = new_node;
}


void argument_parser(char *arguments, struct state *state) {
    char *args[1000] = { 0 };
    int i = 0;
    add_command(arguments, state);
    /*char *arguments_copy = malloc(strlen(arguments));*/
    /*strlcpy(arguments_copy, arguments, strlen(arguments_copy) + 1);*/

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
    } else if (strcmp(args[0], "tprint") == 0) {
        print_command_history(state);
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

