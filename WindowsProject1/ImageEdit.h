#ifndef H_IMAGE_EDIT
#define H_IMAGE_EDIT

class ImageEdit
{
private:
	unsigned char info[54];
	unsigned char* data;
	int row_padded;
	const char* fl;
	void OpenFile(const char*);

public:
	int width, height;
	int filesize;
	int bitPerPixel;
	int colorsCount;
	int compression;
	int H[256] = { 0 };
	int H_R[256] = { 0 };
	int H_G[256] = { 0 };
	int H_B[256] = { 0 };
	void Histogram(const char*);
	void MultiColorHistogram(const char*);
	void Brightness(const char*, int);
	void Grayscale(const char*);
	void Negative(const char*);
	void Contrast(const char*, int, bool);
	void ColorBalance(const char*, int, char);
	void MultiColorBalance(const char*, int, int, int);
};

#endif
