#define _USE_MATH_DEFINES 
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define min(a,b) ((a)<(b))?(a):(b)
#define max(a,b) ((a)>(b))?(a):(b)
#define abs(a) ((a) >= (0)) ? (a):-(a)

typedef unsigned char BYTE;

#pragma pack(push,1)
typedef struct PIXEL{
	BYTE B, G, R;
} PIXEL;
//
//typedef struct IMAGEHEADER{
//	BYTE biSize[4];
//	BYTE biWidth[4];
//	BYTE biHeight[4];
//	BYTE biPlanes[2];
//	BYTE biBitCount[2];
//	BYTE biCompression[4];
//	BYTE biSizeImage[4];
//	BYTE biXPelsPerMeter[4];
//	BYTE biYPelsPerMeter[4];
//	BYTE biClrUsed[4];
//	BYTE biClrImportant[4];
//} IMAGEHEADER;


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
	/*
	unsigned int convert(BYTE *ptr){
	return *(unsigned int *)ptr;
	}
	*/
	void printInfo(){
		printf("--------------------------------\n");
		printf("bfType         :%10hd\n", bfType);
		printf("bfSize         :%10u\n", bfSize);
		printf("bfOffBits      :%10u\n", bfOffBits);
		printf("--------------------------------\n");
		printf("biSize         :%10hd\n", biSize);
		printf("biWidth        :%10hd\n", biWidth);
		printf("biHeight       :%10u\n", biHeight);
		printf("biPlanes       :%10hd\n", biPlanes);
		printf("biBitCount     :%10hd\n", biBitCount);
		printf("biCompression  :%10u\n", biCompression);
		printf("biSizeImage    :%10u\n", biSizeImage);
		printf("biXPelsPerMeter:%10u\n", biXPelsPerMeter);
		printf("biYPelsPerMeter:%10u\n", biYPelsPerMeter);
		printf("biClrUsed      :%10u\n", biClrUsed);
		printf("biClrImportant :%10u\n", biClrImportant);
		printf("--------------------------------\n");
	}
}FILEHEADER;
#pragma pack(pop)
typedef struct BITMAP{
#define LINE_LENGHT 50
#define LINE_LUMEN 20
#define RGB2GRAY(r, g, b) (((b)*117 + (g)*601 + (r)*306) >> 10)
	FILEHEADER *FileHeader;
	PIXEL **_24bit_img;
	BYTE **_8bit_img;
	BITMAP() :FileHeader(0), _24bit_img(0), _8bit_img(0){}

	~BITMAP(){
		Close_BMP();
	}

	bool isOpen(){
		return FileHeader != 0;
	}

	bool Read_BMP(const char *filename){
		FILE *ptr;
		int i, j, W, H, B = 0;

		if (isOpen()){
			printf("Error function BITMAP::Read_BMP()  already Opened\n");
			return false;
		}

		ptr = fopen(filename, "rb");

		if (ptr == NULL){
			printf("Error function BITMAP::Read_BMP()  Read fail\n");
			return false;
		}

		FileHeader = (FILEHEADER *)malloc(sizeof(FILEHEADER));

		fread(FileHeader, sizeof(FILEHEADER), 1, ptr);

		FileHeader->printInfo();

		fseek(ptr, FileHeader->bfOffBits, SEEK_SET);

		W = FileHeader->biWidth;
		H = FileHeader->biHeight;

		_24bit_img = (PIXEL **)malloc(H*sizeof(PIXEL *));
		_8bit_img = (BYTE **)malloc(H*sizeof(BYTE *));
		for (i = 0; i < H; i++){
			_24bit_img[i] = (PIXEL *)malloc(W*sizeof(PIXEL));
			_8bit_img[i] = (BYTE *)malloc(W*sizeof(BYTE));
		}

		for (i = 0; i < H; i++){
			for (j = 0; j < W; j++){
				fread(&_24bit_img[i][j], sizeof(PIXEL), 1, ptr);
				_8bit_img[i][j] = RGB2GRAY(_24bit_img[i][j].R, _24bit_img[i][j].G, _24bit_img[i][j].B);
			}
		}

		//close
		fclose(ptr);

		return true;
	}

	bool Close_BMP(){
		if (isOpen()){
			int i, H = FileHeader->biHeight;
			for (int i = 0; i < H; i++){
				free(_24bit_img[i]);
				free(_8bit_img[i]);
			}
			free(_24bit_img);
			free(_8bit_img);
			free(FileHeader);
			return true;
		}
		else{
			printf("Error function BITMAP::Close_BMP()  BMP File isn't open\n");
			return false;
		}
	}

	void Edge_Detection(){
		if (isOpen() == false){
			printf("Error function BITMAP::Edge_Detection()  BMP File isn't open\n");
			return;
		}


		BYTE MIN = 255, MAX = 0, GAP;
		BYTE **calc;
		int H = FileHeader->biHeight, W = FileHeader->biWidth;
		int d[8][2] = { 1, 0, -1, 0, 0, 1, 0, -1, -1, -1, -1, 1, 1, -1, 1, 1 };
		int i, j, k, ni, nj;

		calc = (BYTE **)malloc(H*sizeof(BYTE *));
		for (i = 0; i < H; i++){
			calc[i] = (BYTE *)malloc(W*sizeof(BYTE));
		}

		for (i = 0; i < H; i++){
			for (j = 0; j < W; j++){
				calc[i][j] = 0;
				for (k = 0; k < 8; k++){
					ni = i + d[k][0];
					nj = j + d[k][1];
					if (ni < 0 || nj < 0 || ni >= H || nj >= W){
						continue;
					}
					calc[i][j] += abs(_8bit_img[i][j] - _8bit_img[ni][nj]);
				}
				calc[i][j] = (255 - calc[i][j]);
				MIN = min(MIN, calc[i][j]);
				MAX = max(MAX, calc[i][j]);
			}
		}
		//(요소값 -최소값)/(최대값-최소값)
		GAP = (MAX - MIN);

		for (i = 0; i < H; i++){
			for (j = 0; j < W; j++){
				_8bit_img[i][j] = ((calc[i][j] - MIN) * 255 / GAP);
			}
		}

		//close 
		for (i = 0; i < H; i++){
			free(calc[i]);
		}
		free(calc);
	}

	void Write_BMP(const char *filename){
		if (isOpen() == false){
			printf("Error function BITMAP::Write_BMP()  BMP File isn't open\n");
			return;
		}
		//output
		int i, j;
		int H = FileHeader->biHeight, W = FileHeader->biWidth;
		FILE *file_ptr = 0;

		file_ptr = fopen(filename, "wb");
		if (file_ptr == 0){
			printf("Error function BITMAP::Write_BMP()  fopen_fail\n");
			return;
		}

		fwrite(FileHeader, sizeof(char), sizeof(FILEHEADER), file_ptr);

		for (i = 0; i < H; i++){
			for (j = 0; j < W; j++){
				fwrite(&_8bit_img[i][j], sizeof(BYTE), 3, file_ptr);
			}
		}

		fclose(file_ptr);
	}


	void plot(int y, int x){
		if (y < 0 || x < 0 || y >= FileHeader->biHeight || x >= FileHeader->biWidth){
			return;
		}

		if (_8bit_img[y][x] + LINE_LUMEN < 256){
			_8bit_img[y][x] += LINE_LUMEN;
		}
		_8bit_img[y][x] = 0;
	}
	void line(int y1, int x1, int r){
		double radi = M_PI* r / 180.0;//lookup_table

		int y2 = cos(radi) * LINE_LENGHT + y1;
		int x2 = sin(radi) * LINE_LENGHT + x1;

		int dx, dy;
		int p_value;
		int inc_2dy;
		int inc_2dydx;
		int inc_value;
		int ndx;

		dx = abs(x2 -x1);
		dy = abs(y2 -y1);

		if ( dy <= dx){
			inc_2dy     = 2 * dy;
			inc_2dydx   = 2 * ( dy - dx);

			if ( x2 < x1){
				ndx   = x1;
				x1    = x2;
				x2    = ndx;

				ndx   = y1;
				y1    = y2;
				y2    = ndx;
			}
			if ( y1 < y2){
				inc_value   = 1;
			}else{
				inc_value   = -1;
			}

			plot( x1, y1);
			p_value  = 2 * dy - dx;    
			for (ndx = x1; ndx < x2; ndx++){
				if ( 0 > p_value){
					p_value  += inc_2dy;
				}else{
					p_value  += inc_2dydx;
					y1       += inc_value;
				}
				plot( ndx, y1);
			}
		}else{
			inc_2dy     = 2 * dx;
			inc_2dydx   = 2 * ( dx - dy);

			if ( y2 < y1){
				ndx   = y1;
				y1    = y2;
				y2    = ndx;

				ndx   = x1;
				x1    = x2;
				x2    = ndx;
			}
			if ( x1 < x2){
				inc_value   = 1;
			}else{
				inc_value   = -1;
			}

			plot( x1, y1);
			p_value  = 2 * dx - dy;    
			for (ndx = y1; ndx < y2; ndx++){
				if ( 0 > p_value){
					p_value  += inc_2dy;
				}else{
					p_value  += inc_2dydx;
					x1       += inc_value;
				}
				plot( x1, ndx);
			}
		}
	}  
};

int main(){
	BITMAP myBitMap;
	myBitMap.Read_BMP("panda.dib");
	myBitMap.Edge_Detection();
	for(int r=0;r<360;r++){
		myBitMap.line(100, 100, r);
	}
	myBitMap.Write_BMP("out.dib");

	return 0;
}
