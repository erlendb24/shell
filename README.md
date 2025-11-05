# Shell

This is a very simple POSIX-style shell with only a handful of commands
currently supported. 

## Commands
Commands use UNIX names with a 't' at the start to differenciate between the
global commands in PATH and my own built-in or external program(ls).

To make "tls" work properly in different directories, move it to /usr/local/bin
or another path that is global. On Linux, you might need to type "./tls" to
make tls work at all, the flags will work the same.

Use "shell" to start the program after using make.

Supported commands in the shell:

```shell
   tls 
   tls -a
   tls -l
   tls -al
   tls -la
   tcd <dir>
   tpwd
   techo <string>
   texit
```
