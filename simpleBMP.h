#ifndef SIMPLEBMP_H
#define SIMPLEBMP_H

#include <vector>
using namespace std;
#pragma pack(1)// For MSVC,disable struct Pack,or short will take 32bit mem as long


//BMP structure infomation : http://www.cnblogs.com/xiekeli/archive/2012/05/09/2491191.html
typedef struct {
	unsigned short bfType;
	unsigned long bfSize;
	unsigned short bfReserved1;
	unsigned short bfReserved2;
	unsigned long bfOffBits;
} ClBitMapFileHeader;

typedef struct {
	unsigned long biSize;
	long biWidth;
	long biHeight;
	unsigned short biPlanes;
	unsigned short biBitCount;
	unsigned long biCompression;
	unsigned long biSizeImage;
	long biXPelsPerMeter;
	long biYPelsPerMeter;
	unsigned long biClrUsed;
	unsigned long biClrImportant;
} ClBitMapInfoHeader;

typedef struct {
	unsigned char rgbBlue;
	unsigned char rgbGreen;
	unsigned char rgbRed;
	unsigned char rgbReserved;
} ClrgbMap;

class ClImgBMP {
public:
	ClBitMapFileHeader bmpFileHeaderData;
	ClBitMapInfoHeader bmpInfoHeaderData;
	ClrgbMap colorMap[256];
	vector<unsigned char> imgData;

	bool LoadImage(const char* path);
	bool SaveImage(const char* path);
};
#pragma pack()// reset to default

#define _CRT_SECURE_NO_WARNINGS //For MSVC ,ignore fopen sercure problem
#include <stdio.h>

bool ClImgBMP::LoadImage(const char* path) {
	FILE* pFile;
	pFile = fopen(path, "rb");
	if (!pFile) {
		return 0;
	}

	int channels = 0;
	int offset = 0;
	unsigned char pixVal;
	fread(&bmpFileHeaderData, sizeof(ClBitMapFileHeader), 1, pFile);
	if (bmpFileHeaderData.bfType == 0x4D42) {//判断是否为RGB图
		fread(&bmpInfoHeaderData, sizeof(ClBitMapInfoHeader), 1, pFile);
		if (bmpInfoHeaderData.biBitCount == 8) {
			//灰度
			channels = 1;
			fread(&colorMap, sizeof(ClrgbMap), 256, pFile);
		}
		else if (bmpInfoHeaderData.biBitCount == 24) {
			//真彩图
			channels = 3;
		}
		int linelength = bmpInfoHeaderData.biWidth * channels;
		offset = linelength % 4;
		if (offset > 0) {
			offset = 4 - offset;
		}

		for (int i = 0; i < bmpInfoHeaderData.biHeight; i++) {
			for (int j = 0; j < linelength; j++) {
				fread(&pixVal, sizeof(unsigned char), 1, pFile);
				imgData.push_back(pixVal);
			}
			for (int k = 0; k < offset; k++) {
				fread(&pixVal, sizeof(unsigned char), 1, pFile);
			}

		}

	}
	else {
		return 0;
	}


	fclose(pFile);
	return 1;
}

bool ClImgBMP::SaveImage(const char* path) {
	FILE* pFile;
	pFile = fopen(path, "wb");
	if (!pFile) {
		return 0;
	}
	unsigned char pixVal;
	fwrite(&bmpFileHeaderData, sizeof(ClBitMapFileHeader), 1, pFile);
	fwrite(&bmpInfoHeaderData, sizeof(ClBitMapInfoHeader), 1, pFile);

	int channels = 0;
	int offset = 0;
	if (bmpInfoHeaderData.biBitCount == 8) {
		//灰度
		channels = 1;
		fwrite(&colorMap, sizeof(ClrgbMap), 256, pFile);
	}
	else if (bmpInfoHeaderData.biBitCount == 24) {
		//真彩图
		channels = 3;
	}
	int linelength = bmpInfoHeaderData.biWidth * channels;
	offset = (channels * bmpInfoHeaderData.biWidth) % 4;
	if (offset > 0) {
		offset = 4 - offset;
	}

	auto iter = imgData.begin();
	for (int i = 0; i < bmpInfoHeaderData.biHeight; i++) {
		for (int j = 0; j < linelength; j++) {
			pixVal = *iter;
			fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
			iter += 1;
		}
		pixVal = 0;
		for (int k = 0; k < offset; k++) {
			fwrite(&pixVal, sizeof(unsigned char), 1, pFile);
		}

	}
	fclose(pFile);
	return 1;
}


#endif
