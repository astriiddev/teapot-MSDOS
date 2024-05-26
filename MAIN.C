/*
	Based on Rebecca Cran's SDL1.2 Utah Teapot:
		https://github.com/bcran/teapot
*/

#include <stdio.h>
#include <string.h>
#include <math.h>

#include "bezier.h"
#include "matrix.h"
#include "video.h"

#define MAX_LINE_LEN 128

int main(int argc, char ** argv)
{
	FILE *teapot_file = NULL;

	vertex *vertices = NULL;
	patch *patches = NULL;

	point rot = { 0, 0, 0 };

	int i = 0;

	if (!init())
	{
		perror("Failed to initialize graphics\n");
		exit(EXIT_FAILURE);
	}

	if((teapot_file = fp_open("utah.dat")) == NULL)
	{
		printf("Unable to open utah data!\n");
		cleanupAndQuit();
		exit(EXIT_FAILURE);
	}

	/* ok, we've opened the data file, now start reading lines */

	vertices = calloc(306, sizeof *vertices);
	patches  = calloc(306, sizeof *patches);

	fp_get_vertices(teapot_file, vertices, 306);
	fp_get_patches(teapot_file, patches, 32);

	initRotPoints();

	if(!tri_init(33 * INTERPOLATION_POINTS * INTERPOLATION_POINTS))
	{
		cleanupAndQuit();
		printf("Not enough memory!\n");
		return 1;
	}

	for(; i < 306; i++)
	{
		const float temp = vertices[i].y;

		vertices[i].y = vertices[i].z;
		vertices[i].z = temp;
	}

	draw_bezier_surface(vertices, patches, rot);
	tri_render();
	redraw_screen();

	while(1)
	{
		clear_screen();
		draw_bezier_surface(vertices,patches, rot);
		tri_render();
		redraw_screen();

		rot.y++;

		if(handle_events()) break;
	}

	free(vertices);
	free(patches);

	cleanupAndQuit();

	(void) argc;
	(void) argv;

	tri_init(0);

	printf("Exiting\n");

	return EXIT_SUCCESS;
}

FILE * fp_open(char * filename)
{
	FILE * file = fopen(filename, "r");
	return file;
}

void fp_close(FILE * file) 
{
	fclose(file);
}

int fp_get_vertices(FILE * file, vertex * vertices, int max_vertex)
{
	int rc = 0, v_num = 0;

	for(; v_num < max_vertex; v_num++) 
	{
		vertex v;
		if((rc = fp_get_next_vertex(file, &v)) != 0) break;
		vertices[v_num] = v;
	}
		
	return rc;
}

int fp_get_patches(FILE * file, patch * patches, int max_patch)
{
	int rc = 0, p_num = 0;
	
	for(; p_num < max_patch; p_num++) 
	{
		patch p;
		if((rc = fp_get_next_patch(file, &p)) != 0) break;
		patches[p_num] = p;
	}
	
	return rc;
}

int fp_get_next_vertex(FILE * f, vertex * v)
{
	int rc = v == NULL ? -1 : 0;
	char line[MAX_LINE_LEN];
	float a,b,c;
	
	memset(line,0,MAX_LINE_LEN);

	while(line[0] != 'V' && rc == 0) {
		if (fgets(line, MAX_LINE_LEN, f) == 0)
			rc = -1;
	}

	if(rc == 0) 
	{
		/* we've found a vertex line */
		sscanf(line, "V%*d %f %f %f", &a,&b,&c);
		v->x = (float) a;
		v->y = (float) b;
		v->z = (float) c;
	}

	return rc;
}

int fp_get_next_patch(FILE * f, patch * p)
{
	int rc = p == NULL ? -1 : 0, i = 0;
	char line[MAX_LINE_LEN];

	memset(&line, 0, MAX_LINE_LEN);

	while(line[0] != 'P' && rc == 0) 
	{
		if(fgets(line, MAX_LINE_LEN, f) == 0)
			rc = -1;
	}

	if(rc == 0) {
		/* we've found a patch line */
		sscanf(line, "P%*d %d %d  %d %d %d %d %d %d %d %d %d %d %d %d %d %d", &p->p[0], &p->p[1], &p->p[2], &p->p[3], &p->p[4], &p->p[5], &p->p[6], &p->p[7], &p->p[8], &p->p[9], &p->p[10], &p->p[11], &p->p[12], &p->p[13], &p->p[14], &p->p[15]);
	}

	for(; i < 16; i++ ) {
		p->p[i]--;
	}
	
	return rc;
}
