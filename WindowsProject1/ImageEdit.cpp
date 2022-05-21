#include "ImageEdit.h"
#include "framework.h"

void ImageEdit::Brightness(const char* filename, int coeff) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        MessageBox(NULL, L"Error while file opening!", L"Critical error", MB_OK);
        throw "IO Exception";
    }

    unsigned char info[54];

    fread(info, sizeof(unsigned char), 54, f);

    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int row_padded = (width * 3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];

    std::remove("brightness.bmp");
    FILE* file = fopen("brightness.bmp", "ab");
    if (file == NULL) throw "Argument Exception";

    fwrite(info, sizeof(unsigned char), 54, file);

    for (int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for (int j = 0; j < width * 3; j += 3)
        {
            int tmp;

            tmp = data[j];
            tmp += coeff * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j] = tmp;

            tmp = data[j + 1];
            tmp += coeff * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j + 1] = tmp;

            tmp = data[j + 2];
            tmp += coeff * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j + 2] = tmp;
        }
        fwrite(data, sizeof(unsigned char), row_padded, file);
    }

    fclose(f);
    fclose(file);
}

void ImageEdit::Grayscale(const char* filename) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        MessageBox(NULL, L"Error while file opening!", L"Critical error", MB_OK);
        throw "IO Exception";
    }

    unsigned char info[54];

    fread(info, sizeof(unsigned char), 54, f);

    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int row_padded = (width * 3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];

    std::remove("grayscale.bmp");
    FILE* file = fopen("grayscale.bmp", "ab");
    if (file == NULL) throw "Argument Exception";

    fwrite(info, sizeof(unsigned char), 54, file);

    for (int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for (int j = 0; j < width * 3; j += 3)
        {
            int k = ((int)data[j] + (int)data[j + 1] + (int)data[j + 2]) / 3;
            data[j] = k;    //blue
            data[j + 1] = k; //green
            data[j + 2] = k; //red
        }
        fwrite(data, sizeof(unsigned char), row_padded, file);
    }

    fclose(f);
    fclose(file);
}



void ImageEdit::Negative(const char* filename) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        MessageBox(NULL, L"Error while file opening!", L"Critical error", MB_OK);
        throw "IO Exception";
    }

    unsigned char info[54];

    fread(info, sizeof(unsigned char), 54, f);

    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int row_padded = (width * 3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];

    std::remove("negative.bmp");
    FILE* file = fopen("negative.bmp", "ab");
    if (file == NULL) throw "Argument Exception";

    fwrite(info, sizeof(unsigned char), 54, file);

    for (int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for (int j = 0; j < width * 3; j += 3)
        {
            data[j] = (255 - (int)data[j]);    //blue
            data[j + 1] = (255 - (int)data[j + 1]); //green
            data[j + 2] = (255 - (int)data[j + 2]); //red
        }
        fwrite(data, sizeof(unsigned char), row_padded, file);
    }

    fclose(f);
    fclose(file);
}



void ImageEdit::Histogram(const char* filename) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        MessageBox(NULL, L"Error while file opening!", L"Critical error", MB_OK);
        throw "IO Exception";
    }

    unsigned char info[54];

    fread(info, sizeof(unsigned char), 54, f);

    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int row_padded = (width * 3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];

    for (int k = 0; k < 256; k++)
    {
        for (int y = 0; y < height; y++)
        {
            fread(data, sizeof(unsigned char), row_padded, f);
            for (int x = 0; x < width * 3; x += 3)
            {
                int c = (0.3 * (int)data[x] + 0.59 * (int)data[x + 1] + 0.11 * (int)data[x + 2]);
                if (c == k) H[k]++;
            }
        }
    }
}