#include "..\header\bitmap.h"

void FILEHEADER::printInfo(){
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

BITMAP::BITMAP() :FileHeader(0), _24bit_img(0), _8bit_img(0){}
BITMAP::BITMAP(BITMAP& copy):FileHeader(0), _24bit_img(0), _8bit_img(0){
	int i, j, W, H;
	FileHeader = (FILEHEADER *)malloc(sizeof(FILEHEADER));
	memcpy(FileHeader,copy.FileHeader,sizeof(FILEHEADER));

	W = FileHeader->biWidth;
	H = FileHeader->biHeight;

	_8bit_img = (BYTE **)malloc(H*sizeof(BYTE *));
	for (i = 0; i < H; i++){
		_8bit_img[i] = (BYTE *)malloc(W*sizeof(BYTE));
	}
}
void BITMAP::clear(){
	if (isOpen()==false){
		printf("Error function BITMAP::clear() not opened\n");
		return ;
	}
	int i, j, W, H;
	W = FileHeader->biWidth;
	H = FileHeader->biHeight;
	for (i = 0; i < H; i++){
		memset(_8bit_img[i],(char)255,sizeof(BYTE)*W);
	}		
}

bool BITMAP::isOpen(){
	return FileHeader != 0;
}

bool BITMAP::Read_BMP(const char *filename){
		// printf("Read_BMP() function called\n");
	FILE *ptr;
	int i, j, W, H;

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

		// FileHeader->printInfo();

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

bool BITMAP::Close_BMP(){
	if (isOpen()){
		int i, H = FileHeader->biHeight;
		for (int i = 0; i < H; i++){
			free(_8bit_img[i]);
		}
		if(_24bit_img!=0){
			for (int i = 0; i < H; i++){
				free(_24bit_img[i]);
			}
			free(_24bit_img);
		}
		free(_8bit_img);
		free(FileHeader);
		return true;
	}
	else{
		printf("Error function BITMAP::Close_BMP()  BMP File isn't open\n");
		return false;
	}
}

void BITMAP::Edge_Detection(){
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
			MIN = min(MIN, calc[i][j]);
			MAX = max(MAX, calc[i][j]);
		}
	}
		//(요소값 -최소값)/(최대값-최소값)
	GAP = (MAX - MIN);

	for (i = 0; i < H; i++){
		for (j = 0; j < W; j++){
			_8bit_img[i][j] = ((calc[i][j] - MIN) * LUMEN_LEVEL / GAP) * LINE_LUMEN;
			_8bit_img[i][j] = (255 - _8bit_img[i][j]);
		}
	}

		//close 
	for (i = 0; i < H; i++){
		free(calc[i]);
	}
	free(calc);
}

void BITMAP::Write_BMP(const char *filename){
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
		printf("Error function BITMAP::Write_BMP() %s fopen_fail\n",filename);
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


void BITMAP::plot(int x, int y){
	if (y < 0 || x < 0 || y >= FileHeader->biHeight || x >= FileHeader->biWidth){
		return;
	}
	if (_8bit_img[y][x] - LINE_LUMEN >= 0){
		_8bit_img[y][x] -= LINE_LUMEN;
	}
}

void BITMAP::line(int x1,int y1,int degree){
	double radi = M_PI* degree / 180.0;//lookup_table

	int y2 = cos(radi) * LINE_LENGTH + y1;
	int x2 = sin(radi) * LINE_LENGTH + x1;

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


BITMAP::~BITMAP(){
	Close_BMP();
}
