#include <getopt.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define CLAMP(val, min, max) ((val < min) ? (min) : ((val > max) ? (max) : (val)))

const char* usage();
void cls();
void signal_handle(int);
void get_time(time_t*, struct tm*);

int main(int argc, char** argv)
{
	signal(SIGINT, signal_handle);
	signal(SIGTERM, signal_handle);

	int optc = 0;
	uint8_t color = 6U;
	while((optc = getopt(argc, argv, "c:")) != -1)
	{
		switch(optc)
		{
			case 'c': color = CLAMP(atoi(optarg), 0, 7); break;
			default: fprintf(stderr, "%s\n", usage()); return 1;
		}
	}
	printf("\033[%dmColor Test!\033[m\n", 30 + color);
	while(1) {}

	return 0;
}

const char* usage() { return "-c x, where x = {0..8}, pick a color"; }

void cls() { printf("\033[2J\033[H"); }

void signal_handle(int signum)
{
	cls();
	exit(0);
}

void get_time(time_t* raw, struct tm* timeinfo)
{
	time(raw);
	timeinfo = localtime(raw);
}

