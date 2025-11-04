#include <stdio.h>
#include <unistd.h>

char *cwd() {
    char buf[1000];
    getcwd(buf, sizeof(buf));
    return buf;
}

int main(int argc, char *argv[]) {
    char *buf = cwd();
    printf("%s", buf);
}
