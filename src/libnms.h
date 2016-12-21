#ifndef LIBNMS_H
#define LIBNMS_H 1

#include <stddef.h>
#include <stdbool.h>

// Default arguments for nms_exec()
#define INIT_NMSARGS { NULL, NULL, NULL, -1, -1, false, false }

// Argument structure for nms_exec()
typedef struct {
	char *src;
	char *foreground_color;
	char *return_opts;
	int input_cursor_x;
	int input_cursor_y;
	bool auto_decrypt;
} NmsArgs;

// Display the characters stored in the display queue
char nms_exec(NmsArgs *);

#endif
