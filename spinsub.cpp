#include "spin.h"
#include "cos.h"
#include "sin.h"



void ZXrotate(float data[], int xangle, int zangle,float output[], int datanx, int datany, int datanz, int zdia)
//rotate a volume set in data by zangle around the commonly defined z-axis and xangle around commonly-defined x-axis
//output: projection image of the rotated volume
{
	int ix,iy,iz,px,pz,xoffset1,xoffset2,yoffset1,zoffset1,zoffset2,mz,mzplus,zindex,yindex,nz1;
	float ixprime, iyprime, izprime, xval,xr,zr,fx,fz,dataxydia,datanxny;
	float sinxangle,cosxangle,sinzangle,coszangle,yrot1,yrot2,zrot,sinxcosz,sinxsinz;
	
	// look up table for sin and cos computation
	sinxangle = vsin[xangle];
	cosxangle = vcos[xangle];
	sinzangle = vsin[zangle];
	coszangle = vcos[zangle];
	sinxcosz = sinxangle*coszangle;
	sinxsinz = sinxangle*sinzangle;

	datanxny = datanx*datany;
	dataxydia = round(sqrt((float)(datanx*datanx + datany*datany)));

	
	//check for odd or even dimension
	xoffset1 = centeroffset(datanx);
	yoffset1 = centeroffset(datany);
	xoffset2 = centeroffset(dataxydia);
	zoffset1 = centeroffset(datanz);
	zoffset2 = centeroffset(zdia);

	for(iz=0;iz<zdia*dataxydia;iz++){
		output[iz] = -500;
	}

	//#pragma omp parallel for private(xval,ixprime,fx,px,xr,izprime,fz,pz,zr,mz,mzplus)
	for(iz=0; iz<datanz; iz++){
		zindex = datanxny*iz;
		zrot = (iz - zoffset1)*cosxangle;
		
		for(iy=0; iy<datany; iy++){
			yrot1 = (iy-yoffset1)*sinzangle;
			yrot2 = (iy-yoffset1)*sinxcosz;
			yindex = datanx*iy; 

			for(ix=0; ix<datanx; ix++){
				xval = data[zindex + yindex + ix];
				ixprime = (ix-xoffset1)*coszangle - yrot1;
				fx = ixprime + xoffset2;
				px = (int)fx;
				xr = fx - px;

				izprime = (ix-xoffset1)*sinxsinz + yrot2 + zrot; 
				fz = izprime + zoffset2;
				pz = (int)fz;
				zr = fz - pz;
				mz = pz*dataxydia;
				mzplus = mz + dataxydia;
				
				//SUM
				//output[mz+px] += xval*(1-xr)*(1-zr);
				//output[mzplus+px] += xval*(1-xr)*zr;
				//output[mz +px + 1] += xval*xr*(1-zr);
				//output[mzplus +px + 1] += xval*xr*zr;

				//Maximum intensity projection
				output[mz+px] = vmax(output[mz+px],xval);
				
				//output[mz+px] = vmax(output[mz+px],xval*(1-xr)*(1-zr));
				//output[mzplus+px] = vmax(output[mzplus+px],xval*(1-xr)*zr);
				//output[mz +px + 1] = vmax(output[mz +px + 1],xval*xr*(1-zr));
				//output[mzplus +px + 1] = vmax(output[mzplus +px + 1],xval*xr*zr);
			
			}
		}
	}
}


int centeroffset(int len)
//return the number of pixels needed to be offset in order to center
//the (0,0) index in the middle of an array based on its given length, len
//check to see if len is even or odd					 
{
	int os;
	if(len%2 != 0)
		os = floor(len/2.0);
	else
		os = (len/2.0) - 1;
	
	return os;
}

float vmax(float a, float b)
{

	if(a > b)
		return a;
	else 
		return b;
}

int floatIMread(char fname[],int imgnx,int imgny, int nheader,float image[])
{
	FILE *pt;
	float xheader[100];
    int ix,iy,a,indx;

	
	pt = fopen( fname,"rb" );
	if(pt==NULL){
		printf("File not found: %s\n",fname );
		return( 99 );
	}

	fread(xheader,sizeof(float),nheader,pt);  //read off the header
	fread(image,sizeof(float),imgnx*imgny,pt);
	fclose(pt);
	return( 0 );
}

int UnsignedCharIMread(char fname[],int imgnx,int imgny, unsigned char image[])
{
	FILE *pt;
    int ix,iy,a,indx;
	pt = fopen( fname,"rb" );
	if(pt==NULL){
		printf("File not found: %s\n",fname );
		return( 99 );
	}

	fread(image,sizeof(unsigned char),imgnx*imgny,pt);
	
	return( 0 );
}


int WritefloatImg(char file[], float img[],int size)
//write img[] to file
{
	FILE *pt;
	
	pt = fopen(file,"wb");
	if(pt==NULL){
		printf("File not found: %s\n",file);
		return 99;
	}
	fwrite(img,sizeof(float),size,pt);
	fclose(pt);

	return 0;
}

void makenoskinimage(float *img, unsigned char *segim, float *combo, int imgnx, int imgny)
//make an image by setting the SKIN + AREA pixels == 0
{
	int ix,iy,a,indx;
	unsigned char segval;

	memset((void *)combo,0,sizeof(float)*imgnx*imgny);
	//#pragma omp parallel for shared(combo,img) private(a,indx,segval)
	for (iy = 0; iy<imgny; iy++){
		a = iy*imgnx;
		for(ix=0; ix<imgnx; ix++){
			indx = a+ix;
			segval = segim[indx];
			if(segval == 0 || segval == 5)
				combo[indx] = -500.00;
			else
				combo[indx] = img[indx];
		}
	}
}


