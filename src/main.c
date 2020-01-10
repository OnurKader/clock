#include "numbers.c"

#include <errno.h>
#include <getopt.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

#define CLAMP(val, min, max) (((val) < (min)) ? (min) : (((val) > (max)) ? (max) : (val)))

const char* usage();
void cls();
void signal_handle(int);
void get_time(time_t*, struct tm*);
void print_time(const int, struct tm*, struct winsize*);
void display_inputs(int);
void gotoRC(int row, int col);

int _rows, _cols;

int main(int argc, char** argv)
{
	signal(SIGINT, signal_handle);
	signal(SIGTERM, signal_handle);

	display_inputs(0);	  // Disable echo'ing chars while running

	struct winsize size;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
	_rows = size.ws_row;
	_cols = size.ws_col;

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

	cls();
	printf("\033[?25l");
	while(1)
	{
		/* cls(); */
		get_time(&raw, &timeinfo);
		print_time(color, &timeinfo, &size);
	}
	return 0;
}

const char* usage() { return "Usage:\n\t-c x, where x = {0..8}, pick a color"; }

void cls() { printf("\033[3J\033[2J\033[H"); }

void signal_handle(int signum)
{
	printf("%d\033[?25h", signum);
	cls();
	exit(0);
}

void get_time(time_t* raw, struct tm* timeinfo)
{
	time(raw);
	*timeinfo = *localtime(raw);
}

void print_time(const int color, struct tm* timeinfo, struct winsize* size)
{
	int num_indices[2];
	num_indices[0] = timeinfo->tm_hour / 10;
	num_indices[1] = timeinfo->tm_hour % 10;
	const int row = size->ws_row / 2 - 3;
	const int col = (size->ws_col - 72) / 2;
	gotoRC(row, col);
	for(int j = 0; j < 2; ++j)
		for(int i = 0; i < 7; ++i)
		{
			gotoRC(row + i, col + 9 * j);
			printf("\033[%dm%s\033[m", (color + 29), numbers[num_indices[j]][i]);
		}
	for(int i = 0; i < 7; ++i)
	{
		gotoRC(row + i, col + 18);
		printf("\033[%dm%s\033[m", (color + 29), numbers[10][i]);
	}

	/////////////////////////////////////////////////
	num_indices[0] = timeinfo->tm_min / 10;
	num_indices[1] = timeinfo->tm_min % 10;
	for(int i = 0; i < 7; ++i)
	{
		gotoRC(row + i, col + 27);
		printf("\033[%dm%s\033[m", (color + 29), numbers[num_indices[0]][i]);
	}
	for(int i = 0; i < 7; ++i)
	{
		gotoRC(row + i, col + 36);
		printf("\033[%dm%s\033[m", (color + 29), numbers[num_indices[1]][i]);
	}
	for(int i = 0; i < 7; ++i)
	{
		gotoRC(row + i, col + 45);
		printf("\033[%dm%s\033[m", (color + 29), numbers[10][i]);
	}

	/////////////////////////////////////////////////
	num_indices[0] = timeinfo->tm_sec / 10;
	num_indices[1] = timeinfo->tm_sec % 10;
	for(int i = 0; i < 7; ++i)
	{
		gotoRC(row + i, col + 54);
		printf("\033[%dm%s\033[m", (color + 29), numbers[num_indices[0]][i]);
	}
	for(int i = 0; i < 7; ++i)
	{
		gotoRC(row + i, col + 63);
		printf("\033[%dm%s\033[m", (color + 29), numbers[num_indices[1]][i]);
	}
	// Quarter second delay 0.42s
	usleep(420000U);
	// Check the size, if it has changed update accordingly
	if(size->ws_row != _rows || size->ws_col != _cols)
	{
		cls();
		_rows = size->ws_row;
		_cols = size->ws_col;
	}
	else
	{
		ioctl(STDOUT_FILENO, TIOCGWINSZ, size);
	}
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
		t.c_lflag |= ECHO;
	else
		t.c_lflag &= ~((tcflag_t)ECHO);

	errno = 0;
	if(tcsetattr(STDIN_FILENO, TCSANOW, &t))
	{
		fprintf(stderr, "tcsetattr failed (%d)\n", errno);
		return;
	}
}

void gotoRC(int row, int col) { printf("\033[%d;%dH", row, col); }

