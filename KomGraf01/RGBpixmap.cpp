#include <fstream>
#include <iostream>
#include <GL/glut.h>

#define checkImageWidth 512
#define checkImageHeight 512

using namespace std;

typedef unsigned short ushort;
typedef unsigned long ulong;
typedef unsigned char uchar;

fstream inf;

static GLubyte checkImage[checkImageHeight][checkImageWidth][4];

class mRGB{
public:
	uchar r,g,b,a;
	mRGB(){r = g = b = 0,a=255;}
};

class RGBpixmap{
	public:
		int nRows,nCols;
		mRGB *pixel;
		void readBMPFile(char *frame);
		void makeCheckerBoard();
		void makeCheckImage();
		void setTexture(GLuint textureName);
};


void RGBpixmap::makeCheckImage(){
	int i, j, c;
	for (i = 0; i < checkImageHeight; i++) {
		for (j = 0; j < checkImageWidth; j++) {
			c = ( ((i&0x8)==0) ^ ((j&0x8)==0) )*255;
			checkImage[i][j][0] = (GLubyte) c;
			checkImage[i][j][1] = (GLubyte) c;
			checkImage[i][j][2] = (GLubyte) c;
			checkImage[i][j][3] = (GLubyte) 255;
		}
	}
}

void RGBpixmap::makeCheckerBoard(){
	nRows=nCols=64;
	pixel = new mRGB[3*nRows*nCols];
	if(!pixel){
		cout<<"Out of memory!";
		return;
	}
	long count=0;
	for(int i=0;i<nRows;i++)
		for(int j=0;j<nCols;j++){
			int c=(( (i/8)+(j/8) ) %2)*255;
			pixel[count].r=c;	//merah
			pixel[count].g=c;	//hijau
			pixel[count++].b=0;	//biru
		}
}

void RGBpixmap::setTexture(GLuint textureName){
	glBindTexture(GL_TEXTURE_2D,textureName);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_OBJECT_LINEAR);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	if(textureName==1)
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, checkImageWidth, checkImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, checkImage);
	else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, nRows, nCols, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixel);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
}

ushort getShort(){
	char ic;
	ushort ip;
	inf.get(ic);
	ip = ic;
	inf.get(ic);
	ip |= ((ushort)ic<<8);
	return ip;
}

ulong getLong(){
	ulong ip = 0;
	char ic = 0;
	unsigned char uc = ic;
	inf.get(ic);
	uc = ic; ip = uc;
	inf.get(ic); uc = ic; ip |= ((ulong)uc<<8);
	inf.get(ic); uc = ic; ip |= ((ulong)uc<<16);
	inf.get(ic); uc = ic; ip |= ((ulong)uc<<24);
	return ip;
}

void RGBpixmap::readBMPFile( char *fname){
	inf.open(fname, ios::in | ios::binary);
	if(!inf){
		cout<<"cannot open file!!!!"<<fname<<endl;
		return ;
	}
	int k, row, col, numPadBytes, nBytesInRow;
	char ch1, ch2;
	inf.get(ch1);
	inf.get(ch2);
	ulong fileSize = getLong();
	ushort reserved1 = getShort();
	ushort reserved2 = getShort();
	ulong offBits = getLong();
	ulong headerSize = getLong();
	ulong numCols = getLong();
	ulong numRows = getLong();
	ushort planes = getShort();
	ushort bitsPerPixel = getShort();
	ulong compression = getLong();
	ulong imageSize = getLong();
	ulong xPels = getLong();
	ulong yPels = getLong();
	ulong numLUTentries = getLong();
	ulong impColors = getLong();
	if(bitsPerPixel != 24){
		cout<<"Not a 24 bit pixel image or is compressed"<<endl;
		inf.close();
		return ;
	}
	
	nBytesInRow = ((3*numCols + 3)/4)*4;
	numPadBytes = nBytesInRow - 3 * numCols;
	nRows = numRows;
	nCols = numCols;
	pixel = new mRGB[nRows * nCols];
	if(!pixel)
		return ; //out of memory
	long count = 0;
	char dum;
	for(row = 0; row < nRows; row++){
		for (col = 0; col < nCols; col++)
		{
			char r,g,b;
			inf.get(b); inf.get(g); inf.get(r);
			pixel[count].r = r;
			pixel[count].g = g;
			pixel[count++].b = b;
		}
		for (k = 0; k < numPadBytes; k++){
			inf >> dum;
		}
	}
	inf.close();
	return ;
}
