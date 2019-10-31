#include "numbers.c"

#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define CLAMP(val, min, max) ((val < min) ? (min) : ((val > max) ? (max) : (val)))

const char* usage();
void cls();
void signal_handle(int);
void get_time(time_t*, struct tm*);
void print_time(const int, struct tm*);
void display_inputs(int);

int main(int argc, char** argv)
{
	signal(SIGINT, signal_handle);
	signal(SIGTERM, signal_handle);

	display_inputs(0);	  // Disable echo'ing chars while running
	char* time_buffer = calloc(1, 2);

	int optc = 0;
	uint8_t color = 7U;
	while((optc = getopt(argc, argv, "c:")) != -1)
	{
		switch(optc)
		{
			case 'c': color = CLAMP(atoi(optarg), 0, 9); break;
			default: fprintf(stderr, "%s\n", usage()); return 1;
		}
	}

	time_t raw;
	struct tm timeinfo;

	printf("\033[?25l");
	while(1)
	{
		cls();
		get_time(&raw, &timeinfo);
		print_time(color, &timeinfo);
		sleep(1U);
	}
	return 0;
}

const char* usage() { return "-c x, where x = {0..10}, pick a color"; }

void cls() { printf("\033[2J\033[H"); }

void signal_handle(int signum)
{
	printf("%d\033[?25h", signum);
	cls();
	printf("%s\n\n", (numbers[0]));
	printf("%s\n\n", (numbers[1]));
	printf("%s\n\n", (numbers[2]));
	printf("%s\n\n", (numbers[3]));
	printf("%s\n\n", (numbers[4]));
	printf("%s\n\n", (numbers[5]));
	printf("%s\n\n", (numbers[6]));
	printf("%s\n\n", (numbers[7]));
	printf("%s\n\n", (numbers[8]));
	printf("%s\n\n", (numbers[9]));
	printf("%s\n", (numbers[10]));
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
		   29 + color,
		   timeinfo->tm_hour,
		   timeinfo->tm_min,
		   timeinfo->tm_sec);
}

void display_inputs(
	int display)	// Credit to
					// "https://www.reddit.com/r/C_Programming/comments/64524q/turning_off_echo_in_terminal_using_c/dfzso9b?utm_source=share&utm_medium=web2x"
{
	struct termios t;

	/* Get console mode */
	errno = 0;
	if(tcgetattr(STDIN_FILENO, &t))
	{
		fprintf(stderr, "tcgetattr failed (%d)\n", errno);
		return;
	}

	/* Set console mode to echo or no echo */
	if(display)
	{
		t.c_lflag |= ECHO;
	}
	else
	{
		t.c_lflag &= ~((tcflag_t)ECHO);
	}
	errno = 0;
	if(tcsetattr(STDIN_FILENO, TCSANOW, &t))
	{
		fprintf(stderr, "tcsetattr failed (%d)\n", errno);
		return;
	}
}

