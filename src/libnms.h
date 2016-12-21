#ifndef LIBNMS_H
#define LIBNMS_H 1

#include <stddef.h>
#include <stdbool.h>

// Default arguments for nms_exec()
#define INIT_NMSARGS { NULL, -1, -1 }

// Argument structure for nms_exec()
typedef struct {
	char *src;
	int input_cursor_x;
	int input_cursor_y;
} NmsArgs;

// Display the characters stored in the display queue
char nms_exec(NmsArgs *);
void nms_set_foreground_color(char *);
void nms_set_return_opts(char *);
void nms_set_auto_decrypt(int);

#endif
