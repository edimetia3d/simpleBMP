#include "simpleBMP.h"
#include <math.h>

void otsu(vector<unsigned char>::iterator iterbeg, vector<unsigned char>::iterator iterend) {

	int numofpixal = iterend - iterbeg;
	double pi[256] = {0};
	double px = 1.0 / numofpixal;
	for (int i = 0; i < numofpixal; i++) {
		int value = *(iterbeg + i);
		pi[value] += px;
	}

	double p1k[256] = {0};
	double mk[256] = {0};
	double cumsumpi = 0;
	double mg = 0;
	for (int i = 0; i < 255; i++) {
		cumsumpi += pi[i];
		p1k[i] = cumsumpi;
		mg += i * pi[i];
		mk[i] = mg;
	}

	int numofbestk = 1;
	int bestk = 0;
	double besttaub = 0;
	double taubk[256] = {0};
	for (int i = 0; i < 255; i++) {
		taubk[i] = pow(mg * p1k[i] - mk[i], 2) / (p1k[i]) / (1 - p1k[i]);
		if (taubk[i] > besttaub) {
			bestk = i;
			besttaub = taubk[i];
			numofbestk = 1;
		}
		else if (taubk[i] == besttaub) {
			bestk += i;
			numofbestk += 1;
		}
	}
	bestk /= numofbestk;
	for (int i = 0; i < numofpixal; i++) {
		int value = *(iterbeg + i);
		*(iterbeg + i) = ((unsigned char)(value > bestk)) * 255;
	}
}

int main(int argc, char* argv[]) {
	if (argc == 1) {
		return 0;
	}
	ClImgBMP img;
	img.LoadImage(argv[1]);
	otsu(img.imgData.begin(), img.imgData.end());
	
	string path = argv[1];
	path.insert(path.size() - 4, "[otsu]");
	img.SaveImage(path.c_str());
	return 1;
}

