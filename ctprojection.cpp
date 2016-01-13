#include "spin.h"

int main(int argc, char *argv[])
{
	FILE *pt;
// ---------------------------------------------------------------------------
//	FLOAT & DOUBLE
// ---------------------------------------------------------------------------
	float xval;
	double sec1,sec2;
	float *xvol,*ximage,*noskinimage,*temp;
	unsigned char *segim;
// ---------------------------------------------------------------------------
//	INT
// ---------------------------------------------------------------------------
	int scanid,yesnoskin,ifirstslice,ilastslice,series,a,indx1,indx2,degree;
	int nz,zdia,degreeinc,xzdia,nxnynz;
	int ix,iy,iz,flag,b,degstart,degend,issun;
// ---------------------------------------------------------------------------
//	CHAR
// ---------------------------------------------------------------------------
	char fname1[buffsize], fname2[buffsize], fname3[buffsize], fname4[buffsize];
	char sourcedrive[buffsize], filedrive[buffsize], scannerid[buffsize],savefiledir[buffsize];
	char jdate[buffsize], jday[buffsize],jtime[buffsize];

// ----------------------------------------------------------------------------------------------
//  Get SCANID then assign other directory locations
// ----------------------------------------------------------------------------------------------	
	scanid = atoi(argv[1]);       //mm
	series = atoi(argv[2]);
	strncpy(scannerid,argv[3],50);
	ifirstslice = atoi(argv[4]);
	ilastslice = atoi(argv[5]);
	yesnoskin = atoi(argv[6]);
	degstart = atoi(argv[7]);
	degend = atoi(argv[8]);
	degreeinc = atoi(argv[9]);
	issun = atoi(argv[10]);
	strncpy(filedrive,argv[11],50);
	strncpy(sourcedrive,argv[12],50);

	nz = ilastslice - ifirstslice + 1;
	zdia = round(sqrt((float)(ny*ny + nz*nz)));
	nxnynz = nxny*nz;
	xzdia = xdia*zdia;
	
	xvol = (float*)malloc(sizeof(float)*nxnynz);
	memset((void *)xvol,0,sizeof(float)*nxnynz);
	ximage = (float*)malloc(sizeof(float)*nxny);
	noskinimage = (float*)malloc(sizeof(float)*nxny);
	temp = (float*)malloc(sizeof(float)*xzdia);
	memset((void *)temp,0,sizeof(float)*xzdia);
	segim = (unsigned char*)malloc(sizeof(unsigned char)*nxny);
	memset((void *)segim,0,sizeof(unsigned char)*nxny);

	jmbtime( jdate,jday,jtime,&sec1);
	printf("date: %s, day: %s, time: %s\n",jdate,jday,jtime);
    printf("start loading the bCT volumes...\n");
	printf("nxnynz = %d\n",nxnynz);
	
    //MAKE BCT VOLUME WITH SKIN OR WITHOUT SKIN
	//#pragma omp parallel for shared(xvol) private(a,b,indx1,indx2)
	for(iz = 0; iz < nz; iz++){
		printf("slice #: %d\r",iz+ifirstslice);
		a = nxny*iz;
		memset((void *)ximage,0,sizeof(float)*nxny);
		if(issun == 0)
			sprintf(fname1,"%s\\CT%s%04d\\CTi\\CT%04d_%02d.%04d",sourcedrive,scannerid,scanid,scanid,series,iz+ifirstslice);
		else
			sprintf(fname1,"%s\\CT%s%04d_Sun\\CTi\\CT%04d_%02d.%04d",sourcedrive,scannerid,scanid,scanid,series,iz+ifirstslice);
		
		
		flag = floatIMread(fname1,nx,ny,20,ximage);
		if(flag == 99){
			printf("cannot open file: %s\n",fname1);
			return 0;
		}
		if(yesnoskin == 0){
			sprintf(fname2,"%s\\CT%s%04d\\SEG\\SEG%04d_%02d_%04d.raw",sourcedrive,scannerid,scanid,scanid,series,iz+ifirstslice);
			flag = UnsignedCharIMread(fname2,nx,ny,segim);
			if(flag == 99){
				printf("cannot open file: %s\n",fname2);
				return 0;
			}
			makenoskinimage(ximage,segim,noskinimage,nx,ny);
		}
		
		for (iy = 0; iy<ny; iy++){
			b = iy*nx;
			for(ix=0; ix<nx; ix++){
				indx1 = a + b + ix;
				indx2 = b + ix;
				if(yesnoskin == 0)
					xvol[indx1] = noskinimage[indx2];
				else
					xvol[indx1] = ximage[indx2];
				
			}
		}
	}

	printf("start the forward projection process with the specified rotation...\n");

	//Set up the output file name for the projection images
	if(issun == 0)
		sprintf(savefiledir,"%s\\proj%04d_%d",filedrive,scanid,yesnoskin);
	else
		sprintf(savefiledir,"%s\\proj%04d_Sun_%d",filedrive,scanid,yesnoskin);

	//#pragma omp parallel for private(degree)
	for(degree = degstart; degree <= degend; degree += degreeinc){
		printf("rotate %d degree around z-axis\n",degree);
		
        // rotate the CT volume wrt x-axis by 15-deg for each forward projection
		ZXrotate(xvol,15,degree,temp,nx,ny,nz,zdia);
		
		sprintf(fname3,"%s\\rotIM\\rot%03d.raw",savefiledir,degree);
		flag = WritefloatImg(fname3,temp,xzdia);
		if(flag > 0)
			return 0;
	}
	sprintf(fname4,"%s\\rotimginfo.txt",savefiledir);
	pt = fopen(fname4,"w");
	if(pt == NULL){
		printf("cannot write the file: %s\n",fname4);
		return 0;
	}
	fprintf(pt,"float32\nwidth=%d\nheight=%d\n",xdia,zdia);
	fprintf(pt,"first slice = %d\n",ifirstslice);
	fprintf(pt,"last slice = %d\n",ilastslice);
	fclose(pt);
	jmbtime( jdate,jday,jtime,&sec2);
	printf("time elapsed: %f\n",sec2-sec1);

	free(xvol);
	free(ximage);
	free(temp);
	free(segim);
	free(noskinimage);

	return 0;
}