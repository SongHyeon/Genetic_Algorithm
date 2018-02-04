#ifndef COMMON_H
#define COMMON_H

#define LUMEN_LEVEL 8
#define LINE_LENGTH 20
#define LINE_LUMEN (256/LUMEN_LEVEL)

#define LINE_NUM 4000
#define GENE_NUM 1000
#define GREAT_GENE 10
#define MUTATION_PERSENTAGE 0.1
#define WRITE_FREQUENCY 1

#define min(a,b) ((a)<(b))?(a):(b)
#define max(a,b) ((a)>(b))?(a):(b)
// #define abs(a) ((a) >= (0)) ? (a):-(a)
#define RGB2GRAY(r, g, b) (((b)*117 + (g)*601 + (r)*306) >> 10)

#define _USE_MATH_DEFINES 

#include <math.h>
#include <algorithm>
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char BYTE;

#endif