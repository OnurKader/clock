#include <getopt.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define CLAMP(val, min, max) ((val < min) ? (min) : ((val > max) ? (max) : (val)))

const char* usage();
void cls();
void signal_handle(int);
void get_time(time_t*, struct tm*);
void print_time(const int, struct tm*);

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

	time_t raw;
	struct tm timeinfo;

	printf("\e[?25l");
	while(1)
	{
		cls();
		get_time(&raw, &timeinfo);
		print_time(color, &timeinfo);
		sleep(1U);
	}
	return 0;
}

const char* usage() { return "-c x, where x = {0..8}, pick a color"; }

void cls() { printf("\033[2J\033[H"); }

void signal_handle(int signum)
{
	cls();
	printf("\e[?25h");
	exit(0);
}

void get_time(time_t* raw, struct tm* timeinfo)
{
	time(raw);
	*timeinfo = *localtime(raw);
}

void print_time(const int color, struct tm* timeinfo)
{
	printf("\033[%dm%02d:%02d:%02d\033[m\n",
		   30 + color,
		   timeinfo->tm_hour,
		   timeinfo->tm_min,
		   timeinfo->tm_sec);
}

