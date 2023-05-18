#ifndef H_IMAGE_EDIT
#define H_IMAGE_EDIT

class ImageEdit
{
private:
	unsigned char info[54];
	unsigned char* data;
	int width, height, row_padded;
	const char* fl;

public:
	int H[256] = { 0 };
	void Brightness(const char*, int);
	void Grayscale(const char*);
	void Negative(const char*);
	void Histogram(const char*);
	void Contrast(const char*, int, bool);
	void ColorBalance(const char*, int, char);
	void MultiColorBalance(const char*, int, int, int);
};

#endif
