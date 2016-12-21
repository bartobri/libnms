#ifndef LIBNMS_H
#define LIBNMS_H 1

// Display the characters stored in the display queue
char nms_exec(char *);
void nms_set_foreground_color(char *);
void nms_set_return_opts(char *);
void nms_set_auto_decrypt(int);
void nms_set_input_position(int, int);

#endif
