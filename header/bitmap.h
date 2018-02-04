#include "common.h"

#ifndef BITMAP_H
#define BITMAP_H

#pragma pack(push,1)
typedef struct PIXEL{
	BYTE B, G, R;
} PIXEL;

typedef struct FILEHEADER{
	//FILEHEADER
	unsigned short	bfType;
	unsigned int	bfSize;
	unsigned short	bfReserved1;
	unsigned short	bfReserved2;
	unsigned int	bfOffBits;
	//IMAGEHEADER
	unsigned int	biSize;
	unsigned int	biWidth;
	unsigned int	biHeight;
	unsigned short	biPlanes;
	unsigned short	biBitCount;
	unsigned int	biCompression;
	unsigned int	biSizeImage;
	unsigned int	biXPelsPerMeter;
	unsigned int	biYPelsPerMeter;
	unsigned int	biClrUsed;
	unsigned int	biClrImportant;  //40 total 54
	 
	void printInfo();

}FILEHEADER;
#pragma pack(pop)

typedef struct BITMAP{
	FILEHEADER *FileHeader;
	PIXEL **_24bit_img;
	BYTE **_8bit_img;
	BITMAP();
	BITMAP(BITMAP& copy);
	bool Read_BMP(const char *filename);
	void Write_BMP(const char *filename);
	void Edge_Detection();

	void plot(int x, int y);
	void line(int x1,int y1,int degree);

	bool isOpen();
	bool Close_BMP();
	void clear();
	~BITMAP();
}BITMAP;

#endif