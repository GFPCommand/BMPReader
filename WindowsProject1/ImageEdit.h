#pragma once
class ImageEdit
{
public:
	int H[256] = { 0 };
	void Brightness(const char*, int);
	void Grayscale(const char*);
	void Negative(const char*);
	void Histogram(const char*);
};

