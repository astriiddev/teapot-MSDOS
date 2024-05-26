/*
	Bezier maths from Rebecca Cran's SDL 1.2 Utah Teapot:
		https://github.com/bcran/teapot/blob/master/bezier.cpp
*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

#include "bezier.h"
#include "matrix.h"
#include "video.h"

int draw_bezier_surface(vertex * vertices, patch * patches, point rotation)
{
	const float t_inc = 1.0f / (float) INTERPOLATION_POINTS;
	const float s_inc = 1.0f / (float) INTERPOLATION_POINTS;

	int patch = 0;

	float x_c[4][4], y_c[4][4], z_c[4][4];
	float x_1, y_1, z_1, s, t;
			
	float m_g_mt__x[4][4];
	float m_g_mt__y[4][4];
	float m_g_mt__z[4][4];

	vertex vert[INTERPOLATION_POINTS + 1][INTERPOLATION_POINTS + 1];
		
	tri_clear();
	
	for (; patch < 32; patch++) 
	{
		int row = 0, col = 0;
		memset(vert, 0, sizeof(vertex) * ((INTERPOLATION_POINTS + 1) * (INTERPOLATION_POINTS + 1)));
		
		for (row = 0; row < 4; row++) 
		{
			for(col = 0; col < 4; col++) 
			{
				const int arraypos = (4 * row) + col;

				x_c[row][col] = vertices[patches[patch].p[arraypos]].x;
				y_c[row][col] = vertices[patches[patch].p[arraypos]].y;
				z_c[row][col] = vertices[patches[patch].p[arraypos]].z;
			}
		}
		
		M_G_MT_calc(x_c, m_g_mt__x);
		M_G_MT_calc(y_c, m_g_mt__y);
		M_G_MT_calc(z_c, m_g_mt__z); 

		for(t = 0; t <= 1.0 + t_inc; t += t_inc) 
		{
			row = (int) (t * (float)INTERPOLATION_POINTS);
			if(row > INTERPOLATION_POINTS) break;
		
			for(s = 0; s <= 1.0+s_inc; s+=s_inc) 
			{
				col = (int) (s * (float) INTERPOLATION_POINTS);
				if(col > INTERPOLATION_POINTS) break;
				
				mult_S_M_G_MT_T(s, t, m_g_mt__x, &x_1);
				mult_S_M_G_MT_T(s, t, m_g_mt__y, &y_1);
				mult_S_M_G_MT_T(s, t, m_g_mt__z, &z_1);
			
				vert[row][col].x = x_1;
				vert[row][col].y = y_1;
				vert[row][col].z = z_1;

				rotate(&vert[row][col], 0, rotation.y, 0);
				rotate(&vert[row][col], rotation.x, 0, 0);
				rotate(&vert[row][col], 0, 0, rotation.z);
			}
		}
		
		/*  ok, we've got a X*X array */
		for(row = 0; row < INTERPOLATION_POINTS; row++) 
		{
			for(col = 0; col < INTERPOLATION_POINTS; col++) 
			{
				triangle t1, t2;

				t1.v1 = vert[row + 0][col + 0];
				t1.v3 = vert[row + 1][col + 1];
				t1.v2 = vert[row + 0][col + 1];

				t2.v1 = vert[row + 0][col + 0];
				t2.v3 = vert[row + 1][col + 0];
				t2.v2 = vert[row + 1][col + 1];

				if(isvisible(t1)) tri_add(t1);
				if(isvisible(t2)) tri_add(t2);
			}
		}
	}

	return 0;
}

