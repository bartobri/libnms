// Copyright (c) 2016 Brian Barto
//
// This program is free software; you can redistribute it and/or modify it
// under the terms of the MIT License. See LICENSE for more details.

#include "libnms.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>
#include <locale.h>

#define __USE_XOPEN
#include <wchar.h>

#define NCURSES_WIDECHAR 1
#include <ncurses.h>

#define TYPE_EFFECT_SPEED    4     // miliseconds per char
#define JUMBLE_SECONDS       2     // number of seconds for jumble effect
#define JUMBLE_LOOP_SPEED    35    // miliseconds between each jumble
#define REVEAL_LOOP_SPEED    100   // miliseconds (must be evenly divisible by 5)

#define SHOW_CURSOR          1     // show cursor during the 'decryption'

#define SPACE                32
#define NEWLINE              10
#define TAB                  9
#define TAB_SIZE             4

#define MASK_CHAR_COUNT      218   // Total characters in maskCharTable[] array.

// Character table representing the character set know as CP437 used by
// the original IBM PC - https://en.wikipedia.org/wiki/Code_page_437
static char *maskCharTable[] = {
	"!", "\"", "#", "$", "%", "&", "'", "(", ")", "*", "+", ",", "-", "~",
	".", "/", ":", ";", "<", "=", ">", "?", "[", "\\", "]", "_", "{", "}",
	"A", "B", "C", "D", "E", "F", "G", "H", "I", "J", "K", "L", "M",
	"N", "O", "P", "Q", "R", "S", "T", "U", "V", "W", "X", "Y", "Z",
	"a", "b", "c", "d", "e", "f", "g", "h", "i", "j", "k", "l", "m",
	"n", "o", "p", "q", "r", "s", "t", "u", "v", "w", "x", "y", "z",
	"0", "1", "2", "3", "4", "5", "6", "7", "8", "9",
	"\xc3\x87", "\xc3\xbc", "\xc3\xa9", "\xc3\xa2", "\xc3\xa4", "\xc3\xa0",
	"\xc3\xa5", "\xc3\xa7", "\xc3\xaa", "\xc3\xab", "\xc3\xa8", "\xc3\xaf",
	"\xc3\xae", "\xc3\xac", "\xc3\x84", "\xc3\x85", "\xc3\x89", "\xc3\xa6",
	"\xc3\x86", "\xc3\xb4", "\xc3\xb6", "\xc3\xb2", "\xc3\xbb", "\xc3\xb9",
	"\xc3\xbf", "\xc3\x96", "\xc3\x9c", "\xc2\xa2", "\xc2\xa3", "\xc2\xa5",
	"\xc6\x92", "\xc3\xa1", "\xc3\xad", "\xc3\xb3", "\xc3\xba", "\xc3\xb1",
	"\xc3\x91", "\xc2\xaa", "\xc2\xba", "\xc2\xbf", "\xc2\xac", "\xc2\xbd",
	"\xc2\xbc", "\xc2\xa1", "\xc2\xab", "\xc2\xbb", "\xce\xb1", "\xc3\x9f",
	"\xce\x93", "\xcf\x80", "\xce\xa3", "\xcf\x83", "\xc2\xb5", "\xcf\x84",
	"\xce\xa6", "\xce\x98", "\xce\xa9", "\xce\xb4", "\xcf\x86", "\xce\xb5",
	"\xc2\xb1", "\xc3\xb7", "\xc2\xb0", "\xc2\xb7", "\xc2\xb2", "\xc2\xb6",
	"\xe2\x8c\x90", "\xe2\x82\xa7", "\xe2\x96\x91", "\xe2\x96\x92",
	"\xe2\x96\x93", "\xe2\x94\x82", "\xe2\x94\xa4", "\xe2\x95\xa1",
	"\xe2\x95\xa2", "\xe2\x95\x96", "\xe2\x95\x95", "\xe2\x95\xa3",
	"\xe2\x95\x91", "\xe2\x95\x97", "\xe2\x95\x9d", "\xe2\x95\x9c",
	"\xe2\x95\x9b", "\xe2\x94\x90", "\xe2\x94\x94", "\xe2\x94\xb4",
	"\xe2\x94\xac", "\xe2\x94\x9c", "\xe2\x94\x80", "\xe2\x94\xbc",
	"\xe2\x95\x9e", "\xe2\x95\x9f", "\xe2\x95\x9a", "\xe2\x95\x94",
	"\xe2\x95\xa9", "\xe2\x95\xa6", "\xe2\x95\xa0", "\xe2\x95\x90",
	"\xe2\x95\xac", "\xe2\x95\xa7", "\xe2\x95\xa8", "\xe2\x95\xa4",
	"\xe2\x95\xa7", "\xe2\x95\x99", "\xe2\x95\x98", "\xe2\x95\x92",
	"\xe2\x95\x93", "\xe2\x95\xab", "\xe2\x95\xaa", "\xe2\x94\x98",
	"\xe2\x94\x8c", "\xe2\x96\x88", "\xe2\x96\x84", "\xe2\x96\x8c",
	"\xe2\x96\x90", "\xe2\x96\x80", "\xe2\x88\x9e", "\xe2\x88\xa9",
	"\xe2\x89\xa1", "\xe2\x89\xa5", "\xe2\x89\xa4", "\xe2\x8c\xa0",
	"\xe2\x8c\xa1", "\xe2\x89\x88", "\xe2\x88\x99", "\xe2\x88\x9a",
	"\xe2\x81\xbf", "\xe2\x96\xa0"
};

// Static variable settings
static int foregroundColor = COLOR_BLUE;   // Foreground color setting
static char *returnOpts    = NULL;         // Return option setting
static int autoDecrypt     = 0;            // Auto-decrypt flag
static int inputPositionX  = -1;           // X coordinate for input position
static int inputPositionY  = -1;           // Y coordinate for input position

// Window position structure, linked list. Keeps track of every
// character's position on the terminal, as well as other attributes.
struct winpos {
	char *source;
	char *mask;
	int width;
	int is_space;
	int s1_time;
	int s2_time;
	struct winpos *next;
};

// Function prototypes
void nms_sleep(int);

/*
 * void nms_exec(NmsArgs *)
 *
 * DESCR:
 * Performs "decryption" effect, Returns the character pressed at the last pause, or
 * '\0' (null character) if there are any problems.
 *
 * ARGS:
 * char *string - character string
 */
char nms_exec(char *string) {
	struct winpos *list_pointer = NULL;
	struct winpos *start;
	struct winpos *temp;
	int i;
	int r_time, r_time_l, r_time_s;
	int maxRows;
	char ret = 0;

	// Lets check the string and make sure we have text. If not, return
	// with an error message.
	if (*string == '\0') {
		fprintf(stderr, "Error. Empty string.\n");
		return 0;
	}

	// Seems to be needed for utf-8 support
	setlocale(LC_ALL, "");

	// Start and initialize curses mode
	initscr();
	cbreak();
	noecho();
	scrollok(stdscr, true);
	if (SHOW_CURSOR == 0)
		curs_set(0);

	// Setting up and starting colors if terminal supports them
	if (has_colors()) {
		start_color();
		init_pair(1, foregroundColor, COLOR_BLACK);
	}
	
	// Get terminal window size
	maxRows = getmaxy(stdscr);

	// Seed my random number generator with the current time
	srand(time(NULL));

	// Geting input
	for (i = 0; string[i] != '\0'; ++i) {

		// Allocate memory for next list link
		if (list_pointer == NULL) {
			list_pointer = malloc(sizeof(struct winpos));
			start = list_pointer;
		} else {
			list_pointer->next = malloc(sizeof(struct winpos));
			list_pointer = list_pointer->next;
		}

		// Get character's byte-length and store character.
		if (mblen(&string[i], 4) > 0) {
			list_pointer->source = malloc(mblen(&string[i], 4) + 1);
			strncpy(list_pointer->source, &string[i], mblen(&string[i], 4));
			list_pointer->source[mblen(&string[i], 4)] = '\0';
			i += (mblen(&string[i], 4) - 1);
		} else {
			endwin();
			fprintf(stderr, "Unknown character encountered. Quitting.\n");
			return 0;
		}

		// Set flag if we have a whitespace character
		if (strlen(list_pointer->source) == 1 && isspace(list_pointer->source[0]))
			list_pointer->is_space = 1;
		else
			list_pointer->is_space = 0;

		// Set initial mask chharacter
		list_pointer->mask = maskCharTable[rand() % MASK_CHAR_COUNT];

		// Set reveal times
		r_time = rand() % 50;
		r_time_s = r_time * .25;
		r_time_l = r_time * .75;
		r_time *= 100;
		r_time_s *= 100;
		r_time_l *= 100;
		
		list_pointer->s1_time = r_time > 1000 ? r_time_l : r_time;
		list_pointer->s2_time = r_time > 1000 ? r_time_s : 0;
		list_pointer->next = NULL;

		// Set character column width
		wchar_t widec[sizeof(list_pointer->source)] = {};
		mbstowcs(widec, list_pointer->source, sizeof(list_pointer->source));
		list_pointer->width = wcwidth(*widec);
	}
	
	// Position cursor to top-left
	move(0,0);

	// Initially display the characters in the terminal with a 'type effect'.
	for (list_pointer = start; list_pointer != NULL; list_pointer = list_pointer->next) {
		
		// break out of loop if we reach the bottom of the terminal
		if (getcury(stdscr) == maxRows - 1) {
			break;
		}
		
		if (list_pointer->is_space) {
			addstr(list_pointer->source);
		} else {
			addstr(list_pointer->mask);
			if (list_pointer->width == 2) {
				addstr(maskCharTable[rand() % MASK_CHAR_COUNT]);
			}
		}
		refresh();
		nms_sleep(TYPE_EFFECT_SPEED);
	}

	// Flush any input up to this point
	flushinp();

	// If autoDecrypt flag is set, we sleep. Otherwise, reopen stdin for interactive
	// input (keyboard), then require user to press a key to continue.
	if (autoDecrypt || (!isatty(STDIN_FILENO) && !freopen ("/dev/tty", "r", stdin)))
		sleep(1);
	else
		getch();

	// Jumble loop
	for (i = 0; i < (JUMBLE_SECONDS * 1000) / JUMBLE_LOOP_SPEED; ++i) {
		move(0,0);
		for (list_pointer = start; list_pointer != NULL; list_pointer = list_pointer->next) {
			
			// break out of loop if we reach the bottom of the terminal
			if (getcury(stdscr) == maxRows - 1) {
				break;
			}
		
			if (list_pointer->is_space) {
				addstr(list_pointer->source);
			} else {
				addstr(maskCharTable[rand() % MASK_CHAR_COUNT]);
				if (list_pointer->width == 2) {
					addstr(maskCharTable[rand() % MASK_CHAR_COUNT]);
				}
			}
		}
		refresh();
		nms_sleep(JUMBLE_LOOP_SPEED);
	}

	// Reveal loop
	int s1_remask_time = 500;     // time, in milliseconds, we change the mask for stage 1
	bool loop = true;
	while (loop) {
		move(0,0);
		loop = false;
		for (list_pointer = start; list_pointer != NULL; list_pointer = list_pointer->next) {
			
			// break out of loop if we reach the bottom of the terminal
			if (getcury(stdscr) == maxRows - 1) {
				break;
			}
			
			if (list_pointer->is_space) {
				addstr(list_pointer->source);
				continue;
			}
			
			if (list_pointer->s1_time > 0) {
				loop = true;
				list_pointer->s1_time -= REVEAL_LOOP_SPEED;
				if (list_pointer->s1_time % s1_remask_time == 0) {
					list_pointer->mask = maskCharTable[rand() % MASK_CHAR_COUNT];
				}
				addstr(list_pointer->mask);
				if (list_pointer->width == 2) {
					addstr(maskCharTable[rand() % MASK_CHAR_COUNT]);
				}
			} else if (list_pointer->s2_time > 0) {
				loop = true;
				list_pointer->s2_time -= REVEAL_LOOP_SPEED;
				addstr(maskCharTable[rand() % MASK_CHAR_COUNT]);
				if (list_pointer->width == 2) {
					addstr(maskCharTable[rand() % MASK_CHAR_COUNT]);
				}
			} else {
				attron(A_BOLD);
				if (has_colors())
					attron(COLOR_PAIR(1));
				addstr(list_pointer->source);
			}
			
			refresh();

			attroff(A_BOLD);
			if (has_colors())
				attroff(COLOR_PAIR(1));
		}
		nms_sleep(REVEAL_LOOP_SPEED);
	}

	// Flush any input up to this point
	flushinp();

	// Position cursor
	if (inputPositionY >= 0 && inputPositionX >= 0) {
		move(inputPositionY, inputPositionX);
		curs_set(1);
	}

	// If stdin is set to the keyboard, user must press a key to continue
	if (isatty(STDIN_FILENO)) {
		ret = getch();
		if (returnOpts != NULL && strlen(returnOpts) > 0)
			while (index(returnOpts, ret) == NULL) {
				beep();
				ret = getch();
			}
	} else
		sleep(2);


	// End curses mode
	endwin();

	// Freeing the list. 
	list_pointer = start;
	while (list_pointer != NULL) {
		temp = list_pointer;
		list_pointer = list_pointer->next;
		free(temp->source);
		free(temp);
	}

	return ret;
}

void nms_sleep(int t) {
	struct timespec ts;
	
	ts.tv_sec = t / 1000;
	ts.tv_nsec = (t % 1000) * 1000000;
	
	nanosleep(&ts, NULL);
}

void nms_set_foreground_color(char *color) {

	if(strcmp("white", color) == 0)
		foregroundColor =  COLOR_WHITE;
	else if(strcmp("yellow", color) == 0)
		foregroundColor = COLOR_YELLOW;
	else if(strcmp("black", color) == 0)
		foregroundColor = COLOR_BLACK;
	else if(strcmp("magenta", color) == 0)
		foregroundColor = COLOR_MAGENTA;
	else if(strcmp("blue", color) == 0)
		foregroundColor = COLOR_BLUE;
	else if(strcmp("green", color) == 0)
		foregroundColor = COLOR_GREEN;
	else if(strcmp("red", color) == 0)
		foregroundColor = COLOR_RED;
	else
		foregroundColor = COLOR_BLUE;
}

void nms_set_return_opts(char *opts) {
	returnOpts = realloc(returnOpts, strlen(opts) + 1);
	strcpy(returnOpts, opts);
}

void nms_set_auto_decrypt(int setting) {
	if (setting)
		autoDecrypt = 1;
	else
		autoDecrypt = 0;
}

void nms_set_input_position(int x, int y) {
	if (x >= 0 && y >= 0) {
		inputPositionX = x;
		inputPositionY = y;
	}
}
