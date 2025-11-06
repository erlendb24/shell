all:
	gcc -Wall -Wextra -pedantic shell.c -o shell
	gcc -Wall -Wextra -pedantic mv.c -o tmv
	gcc ls.c -o tls
