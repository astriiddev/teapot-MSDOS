#ifndef _FILE_PARSER_H_
#define _FILE_PARSER_H_

#include <stdio.h>

/*  increase or decrease to change number of polygons
	DON'T INCREASE PAST 6 IN 16BIT REAL MODE (not enough memory) */
#define INTERPOLATION_POINTS 4

typedef struct vertex_v
{
	float x;
	float y;
	float z;
	float w;
} vertex;

struct patch_v
{
	int p[16];
};

typedef struct
{
	vertex v1;
	vertex v2;
	vertex v3;
	int visible;
} triangle;

typedef struct {
	int x;
	int y;
	int z;
} point;

typedef struct patch_v patch;

FILE * fp_open(char * filename);
void fp_close(FILE * file);
int fp_get_vertices(FILE * file, vertex * vertices, int max_vertex);
int fp_get_patches(FILE * file, patch * patches, int max_vertex);
int fp_get_next_vertex(FILE *f, vertex * v);
int  fp_get_next_patch(FILE *f, patch * p);

#endif
