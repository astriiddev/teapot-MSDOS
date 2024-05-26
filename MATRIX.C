/*
	Matrix maths from Rebecca Cran's SDL 1.2 Utah Teapot:
		https://github.com/bcran/teapot/blob/master/manip.cpp
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "matrix.h"

#ifndef PI
#define PI 3.141592f
#endif

static const float DEG45 = PI/4;
	
static float M[4][4] =  { { -1, 3,-3, 1},
						  {  3,-6, 3, 0},
						  { -3, 3, 0, 0},
						  {  1, 0, 0, 0}
						};

float rotx[4][4], roty[4][4], rotz[4][4];

void initRotPoints(void)
{
	const float c = cos(DEG45), s = sin(DEG45);

	memset(rotx, 0, 4 * 4 * sizeof(float));
	memset(roty, 0, 4 * 4 * sizeof(float));
	memset(rotz, 0, 4 * 4 * sizeof(float));

	rotx[0][0] = rotx[3][3] = roty[1][1] = rotz[2][2] = 1;

	rotx[1][1] =  rotx[2][2] = c;
	rotx[1][2] = -s;
	rotx[2][1] =  s;

	roty[0][0] =  rotx[2][2] = c;
	roty[0][2] = -s;
	roty[2][0] =  s;

	rotz[0][0] =  rotx[1][1] = c;
	rotz[0][1] =  s;
	rotz[1][0] = -s;
}

void  mult_matrix_4_4(float a[4][4], float b[4][4], float result[4][4])
{       
	int cur_row = 0;

	for(; cur_row < 4; cur_row++) 
	{
		int cur_col = 0;

		for(; cur_col < 4; cur_col++) 
		{
			int c = 0;

			result[cur_row][cur_col] = 0;

			for(; c < 4; c++) 
			{
				result[cur_row][cur_col] += a[cur_row][c] * b[c][cur_col];
			}
		}
	}
}

void M_G_MT_calc(float G[4][4], float result[4][4])
{
	float temp_result[4][4];
	mult_matrix_4_4(M, G, temp_result);
	mult_matrix_4_4(temp_result,M, result);
}

void mult_S_M_G_MT_T(double s, double t, float m_g_mt[4][4], float * result) 
{
	float s_m_g_mt[4];
	s_m_g_mt[0] = ((s*s*s)*m_g_mt[0][0]) + 
		      ((s*s)  *m_g_mt[1][0]) +
		      ((s)    *m_g_mt[2][0]) +
		      m_g_mt[3][0];
	
	s_m_g_mt[1] = ((s*s*s)*m_g_mt[0][1]) +
		      ((s*s)  *m_g_mt[1][1]) +
		      ((s)    *m_g_mt[2][1]) +
		      m_g_mt[3][1];

	s_m_g_mt[2] = ((s*s*s)*m_g_mt[0][2]) +
		      ((s*s)  *m_g_mt[1][2]) +
		      ((s)    *m_g_mt[2][2]) +
		      m_g_mt[3][2];

	s_m_g_mt[3] = ((s*s*s)*m_g_mt[0][3]) +
		      ((s*s)  *m_g_mt[1][3]) +
		      ((s)    *m_g_mt[2][3]) +
		      m_g_mt[3][3];

	*result = (s_m_g_mt[0]*(t*t*t)) + (s_m_g_mt[1]*(t*t)) + (s_m_g_mt[2]*t) + s_m_g_mt[3];
}

void rotate(vertex * p, int x, int y, int z)
{
	float gm[3];

	float radrotx = ((float) x / 180.0) * PI, 
		  radroty = ((float) y / 180.0) * PI,
		  radrotz = ((float) z / 180.0) * PI;

	gm[0] = p->x;
	gm[1] = p->y;
	gm[2] = p->z;

	rotx[1][1] = cos(radrotx);
	rotx[1][2] = -sin(radrotx);
	rotx[2][1] = sin(radrotx);
	rotx[2][2] = cos(radrotx);

	gm[0] = (rotx[0][0] * gm[0]) + (rotx[0][1] * gm[1]) + (rotx[0][2] * gm[2]) + rotx[0][3];
	gm[1] = (rotx[1][0] * gm[0]) + (rotx[1][1] * gm[1]) + (rotx[1][2] * gm[2]) + rotx[1][3];
	gm[2] = (rotx[2][0] * gm[0]) + (rotx[2][1] * gm[1]) + (rotx[2][2] * gm[2]) + rotx[2][3];

	roty[0][0] = cos(radroty);
	roty[0][2] = -sin(radroty);
	roty[2][0] = sin(radroty);
	roty[2][2] = cos(radroty);

	gm[0] = (roty[0][0] * gm[0]) + (roty[0][1] * gm[1]) + (roty[0][2] * gm[2]) + roty[0][3];
	gm[1] = (roty[1][0] * gm[0]) + (roty[1][1] * gm[1]) + (roty[1][2] * gm[2]) + roty[1][3];
	gm[2] = (roty[2][0] * gm[0]) + (roty[2][1] * gm[1]) + (roty[2][2] * gm[2]) + roty[2][3];

	rotz[0][0] =  cos(radrotz);
	rotz[0][1] =  sin(radrotz);
	rotz[1][0] = -sin(radrotz);
	rotz[1][1] =  cos(radrotz);

	gm[0] = (rotz[0][0] * gm[0]) + (rotz[0][1] * gm[1]) + (rotz[0][2]  *gm[2]) + rotz[0][3];
	gm[1] = (rotz[1][0] * gm[0]) + (rotz[1][1] * gm[1]) + (rotz[1][2]  *gm[2]) + rotz[1][3];
	gm[2] = (rotz[2][0] * gm[0]) + (rotz[2][1] * gm[1]) + (rotz[2][2]  *gm[2]) + rotz[2][3];
	
	p->x = gm[0];
	p->y = gm[1];
	p->z = gm[2];
}
