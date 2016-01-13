#include <windows.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <boonelib.h>
//#include <omp.h>
//#include <avifile.h>

/*  ----------------------- Main Program ----------- */

#define MX 1280
#define MY 1024
#define buffsize 100
#define	nx 512
#define ny 512
#define xdia 724   //sqrt(nx^2 + ny^2)
#define nxny  262144
#define pi 3.141592654
#define round(x) ((x)>0? (int) ((x) + 0.5):(int)((x) - 0.5))

static double twopi = 2. * pi;
static double dtr = pi / 180.;
static double rtd = 180. / pi;

//void Xrotate(float data[], double angle, float output[]);
//void YXrotate(float data[], double xangle, double yangle,float output[]);
void ZXrotate(float data[], int xangle, int zangle,float output[], int datanx, int datany, int datanz, int zdia);
//void ZYrotate(float data[], double yangle, double zangle,float output[]);
int centeroffset(int len);
float vmax(float a, float b);
int floatIMread(char fname[],int imgnx,int imgny, int nheader,float image[]);
int WritefloatImg(char file[], float img[],int size);
int UnsignedCharIMread(char fname[],int imgnx,int imgny, unsigned char image[]);
void makenoskinimage(float *img, unsigned char *segim, float *combo, int imgnx, int imgny);