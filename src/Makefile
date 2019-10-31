all: build

build:
	clear && gcc -o ../bin/clock main.c -O3 -g -Wall -Wextra -pedantic -Wshadow -Wno-unused-variable -Wno-unused-but-set-variable -std=gnu11

install:
	sudo ln -sf ../bin/clock /usr/bin/clock
clean:
	rm -f ../bin/clock
