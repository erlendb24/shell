#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <stdbool.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>

#define MAX_PATH 1024

int check_amt_digits(int number) {
    if (number < 10) {
        return 1;
    }

    return 1 + check_amt_digits(number/10);
}

void bitwise_print(struct stat *stat, int constant, char char_to_print) {
    if (stat->st_mode & constant) {
        printf("%c", char_to_print);
    } else {
        printf("%c", '-');
    }
}

void ls_l(struct dirent *dir) {
    struct stat stat = { 0 };
    char buf[1024] = { 0 };
    lstat(dir->d_name, &stat);
    struct tm time = { 0 };
    localtime_r(&stat.st_mtimespec.tv_sec, &time);

    if (S_ISDIR(stat.st_mode)) {
        printf("%c", 'd');
    } else {
        printf("%c", '-');
    }

    bitwise_print(&stat, S_IRUSR, 'r');
    bitwise_print(&stat, S_IWUSR, 'w');
    bitwise_print(&stat, S_IXUSR, 'x');

    for (int i = 0; i < 2; i++) {
        bitwise_print(&stat, S_IROTH, 'r');
        bitwise_print(&stat, S_IWOTH, 'w');
        bitwise_print(&stat, S_IXOTH, 'x');
    }
    int size_digits = check_amt_digits(stat.st_nlink);
    for(int j = size_digits; j < 4; j++) {
        printf(" ");
    }
    printf("%c%d", ' ', stat.st_nlink);

    struct passwd *pw = getpwuid(stat.st_uid);
    struct group *gr = getgrgid(stat.st_gid);

    printf("%c%s", ' ', pw->pw_name);
    printf("%c%s", ' ', gr->gr_name);

    size_digits = check_amt_digits(stat.st_size);
    for(int m = size_digits; m < 9; m++) {
        printf(" ");
    }
    printf("%c%lld", ' ', stat.st_size);
    strftime(buf, sizeof(buf), "%b %d %H:%M", &time);
    printf("%c%s", ' ', buf);
    printf("%c%s", ' ', dir->d_name);
    printf("\n");
}

void dirwalk(char *dir, void (*fcn)(char*), char *argv[]) {
    char name[MAX_PATH];
    struct dirent *dir_p;
    DIR *dir_fd;

    bool all = false;
    bool list = false;

    if (argv[1] != NULL) {
        if (argv[1][0] == '-') {
            int i = 1;
            while (argv[1][i] != '\0') {
                switch (argv[1][i]) {
                    case 'a':
                        all = true;
                        break;
                    case 'l':
                        list = true;
                    default:
                        break;
                }
                i++;
            }
        }
    }
    
    if ((dir_fd = opendir(dir)) == NULL) {
        fprintf(stderr, "%s\n", "Directory is empty!");
    }

    while ((dir_p = readdir(dir_fd)) != NULL) {
        if (all == false) {
            if (strcmp(dir_p->d_name, ".") == 0 || strcmp(dir_p->d_name, "..") == 0) {
                continue;
            }
        }
        if (strlen(dir) + strlen(dir_p->d_name) + 2 > sizeof(name)) {
            fprintf(stderr, "%s\n", "File name too is long!");
            /* handle error, name too long */
        } else if (list == true) {
            ls_l(dir_p);
        } else {
            (*fcn)(dir_p->d_name);
            printf("\t");
        }
    }

    printf("\n");
    closedir(dir_fd);
}

int main(int argc, char *argv[]) {
    char dir[MAX_PATH] = ".";
    dirwalk(dir, (void *)printf, argv);
}
