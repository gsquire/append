CC=gcc
OPTS=-Wall -Werror -Wextra -O2 -std=c99

all : 
	$(CC) $(OPTS) -o app append.c

clean :
	\rm app
