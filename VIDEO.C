/*
	MSDOS video and line drawing based on root42's Let's Code MSDOS series:
		https://youtu.be/6C0ch7kV7B4?si=ok-Q5Dj5uV9axztg
		https://youtu.be/pMZ-nWpHJdo?si=nAvZkKT1lmDWA9rb

	and PC-GPE's MSDOS tutorial series:
		http://bespin.org/~qz/pc-gpe/


	Triangle drawing based on Rebecca Cran's SDL1.2 Utah Teapot:
		https://github.com/bcran/teapot/blob/master/line_util.cpp
*/

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include <dos.h>
#include <conio.h>

#include "video.h"

#if defined (__TURBOC__)

#define VIDEO_INT 10h

static unsigned char far *VGA = NULL, far *pixel_buffer = NULL;
static triangle far *tri_list = NULL;

#elif defined (__DJGPP__)

#include <dpmi.h>
#include <sys/nearptr.h>

#define VIDEO_INT 0x10

static unsigned char *VGA = NULL, *pixel_buffer = NULL;
static triangle *tri_list = NULL;

#else

#define VIDEO_INT 0x10

static unsigned char far *VGA = NULL, far *pixel_buffer = NULL;
static triangle far *tri_list = NULL;

#endif

#define VIDEO_MODE 0x0013
#define TEXT_MODE  0x0003

#define GRID_SIZE 240

#define SETPIX(x, y, c)  *(pixel_buffer + (x) + (y) * WINDOW_WIDTH) = c

#define ABS(a)   ((a) >  0 ? (a) : -(a))
#define SGN(a,b) ((a) < (b) ? 1 : -1)

#define WINDOW_WIDTH  320
#define WINDOW_HEIGHT 200

static unsigned char current_color = 0x30;
static size_t t_size = 0, t_index = 0;

static void set_video(const int mode)
{
#if defined (__TURBOC__)
	asm{
		mov ax, mode
		int VIDEO_INT
	}
#elif defined (__DJGPP__)
	 __dpmi_regs regs;

	 memset(&regs, 0, sizeof regs);
	 regs.x.ax = mode;
	 __dpmi_int(VIDEO_INT, &regs);
#else
	union REGS regs;

	memset(&regs, 0, sizeof regs);

	regs.h.ah = (unsigned char) ((mode & 0xFF00) >> 8);
	regs.h.al = (unsigned char)  (mode & 0x00FF);

	int86(VIDEO_INT, &regs, &regs);
#endif
}

int init(void)
{
	const int area = WINDOW_WIDTH * WINDOW_HEIGHT;

#if defined (__DJGPP__)
	if(! __djgpp_nearptr_enable()) return 0;
	VGA = (unsigned char*) (0xA0000 + __djgpp_conventional_base);
#else
	VGA = (unsigned char far*) (0xA0000000L);
#endif

	pixel_buffer = calloc(area, sizeof *pixel_buffer);

	if(VGA == NULL || pixel_buffer == NULL) return 0;

	set_video(VIDEO_MODE);

	memset(VGA, 0, area * sizeof *VGA);
	memset(pixel_buffer, 0, area * sizeof *pixel_buffer);

	return 1;
}

int handle_events(void)
{
	int quit = 0;

	while (kbhit()) {
		quit = (getch() == 27);
	}

	return quit;
}

void cleanupAndQuit(void)
{
	if(pixel_buffer != NULL) free(pixel_buffer);

	set_video(TEXT_MODE);

#if defined (__DJGPP__)
	__djgpp_nearptr_disable();
#endif
}

void redraw_screen(void)
{
	memcpy(VGA, pixel_buffer, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof *VGA);

	while( (inp(0x03DA) & 0x08));
	while(!(inp(0x03DA) & 0x08));

#if defined (__DJGPP__)
	VGA = (unsigned char*) (0xA0000 + __djgpp_conventional_base);
#endif
}

void clear_screen(void)
{
	memset(pixel_buffer, 0, WINDOW_WIDTH * WINDOW_HEIGHT * sizeof *pixel_buffer);
}

static void draw_line(const int x1, const int y1, const int x2, const int y2)
{
	int x, y, dx, dy, sx, sy, error, e2, c = current_color;

	x = x1, y = y1;

	dx = ABS(x2 - x1);
	sx = SGN(x1, x2);
	dy = ABS(y2 - y1);
	sy = SGN(y1, y2);

	error = dx - dy;

	while (1)
	{
		SETPIX(x, y, c);
		if (x == x2 && y == y2) break;
		e2 = error << 1;
		if (e2 >= -dy)
		{
			if (x == x2) break;
			error -= dy;
			x += sx;
		}
		if (e2 <= dx)
		{
			if (y == y2) break;
			error += dx;
			y += sy;
		}
	}
}

static int get_point_coord(const float t_point) { return (int) ((40 * t_point) + (GRID_SIZE / 2.0)); }

static point get_point(const vertex *v)
{
	point p;

	p.x = get_point_coord(v->x) + 20;
	p.y = get_point_coord(v->y) - 60;
	p.z = get_point_coord(v->z);

	return p;
}

static void drawPolygon(const point (*p)[3])
{
	point t[3];
	int i = 0;

	if(p == NULL) return;

	for(; i < 3; i++)
	{
		t[i].x = (short) (*p)[i].x;
		t[i].y = (short) (WINDOW_HEIGHT - (*p)[i].y);
	}

	draw_line(t[0].x, t[0].y, t[1].x, t[1].y);
	draw_line(t[1].x, t[1].y, t[2].x, t[2].y);
	draw_line(t[0].x, t[0].y, t[2].x, t[2].y);
}

static void draw_triangle(triangle t)
{
	point points[3];

	points[0] = get_point(&t.v1);
	points[1] = get_point(&t.v2);
	points[2] = get_point(&t.v3);

	drawPolygon((const point (*)[3]) &points);
}

int tri_init(const size_t size)
{
	t_index = 0;

	if(tri_list != NULL)
	{
		free(tri_list);
		tri_list = NULL;
	}

	if((t_size = size) <= 0) return 1;

	if((tri_list = calloc(t_size, sizeof *tri_list)) == NULL) return 0;

	return 1;
}

void tri_clear(void)
{
	t_index = 0;
	memset(tri_list, 0, t_size * sizeof *tri_list);
}

void tri_add(triangle t)
{
	if(t_index >= t_size) return;

	t.visible = 1;
	tri_list[t_index] = t;
	t_index++;
}

static void crossproduct(vertex * r, vertex p, vertex q) {
	r->x = p.y*q.z - p.z*q.y;
	r->y = p.z*q.x - p.x*q.z,
	r->z = p.x*q.y - p.y*q.x;
}

int isvisible(triangle t)
{
	/* see if the triangle is visible */
	vertex v1 = t.v1, v2 = t.v2, v3 = t.v3;
	vertex n1, n2, crossp;

	/* v2-v1 */
	n1.x = v2.x-v1.x;
	n1.y = v2.y-v1.y;
	n1.z = v2.z-v1.z;

	/* v3-v2 */
	n2.x = v3.x-v2.x;
	n2.y = v3.y-v2.y;
	n2.z = v3.z-v2.z;

	crossproduct(&crossp, n1,n2);

	return (!(crossp.z > 0));
}

void tri_render(void)
{
	unsigned int i = 0;
	for(; i < t_index; i++)
		draw_triangle(tri_list[i]);
}
