/* 
	CS4 Graphics Practical 1

	Rebecca Cran <brucec@dcs.ed.ac.uk>

	This is my implementation of the classis bresenham algorithm.
	The simple midpoint algorithm code has been optimized to remove
	floating-point calculations.
*/

#ifndef _VIDEO_UTIL_H_
#define _VIDEO_UTIL_H_

#include "defs.h"

int init(void);
void redraw_screen(void);
void clear_screen(void);

void cleanupAndQuit(void);
int handle_events(void);

int isvisible(triangle t);
int tri_init(const size_t size);
void tri_clear(void);
void tri_add(triangle t);
void tri_render(void);

#endif
