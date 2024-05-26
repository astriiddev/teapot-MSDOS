#ifndef _MATRIX_MATHS_H_
#define _MATRIX_MATHS_H_

#include "defs.h"

void initRotPoints(void);
void M_G_MT_calc(float G[4][4], float result[4][4]);
void mult_S_M_G_MT_T(double s, double t, float m_g_mt[4][4], float * result);
void rotate(vertex * p, int x, int y, int z);

#endif
