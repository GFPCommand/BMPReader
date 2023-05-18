#include "framework.h"

void ImageEdit::Brightness(const char* filename, int coeff) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        MessageBox(NULL, L"Error while file opening!", L"Critical error", MB_OK);
        throw "IO Exception";
    }

    fread(info, sizeof(unsigned char), 54, f);

    width = *(int*)&info[18];
    height = *(int*)&info[22];

    row_padded = (width * 3 + 3) & (~3);
    data = new unsigned char[row_padded];

    fl = "brightness.bmp";

    std::remove(fl);
    FILE* file = fopen(fl, "ab");
    if (file == NULL) {
        throw "Argument Exception";
        exit(1);
    }

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

    fread(info, sizeof(unsigned char), 54, f);

    width = *(int*)&info[18];
    height = *(int*)&info[22];

    row_padded = (width * 3 + 3) & (~3);
    data = new unsigned char[row_padded];

    fl = "grayscale.bmp";

    std::remove(fl);
    FILE* file = fopen(fl, "ab");
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

    fread(info, sizeof(unsigned char), 54, f);

    width = *(int*)&info[18];
    height = *(int*)&info[22];

    row_padded = (width * 3 + 3) & (~3);
    data = new unsigned char[row_padded];

    fl = "negative.bmp";

    std::remove(fl);
    FILE* file = fopen(fl, "ab");
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

    fread(info, sizeof(unsigned char), 54, f);

    width = *(int*)&info[18];
    height = *(int*)&info[22];

    row_padded = (width * 3 + 3) & (~3);
    data = new unsigned char[row_padded];

    for (int k = 0; k < 256; k++)
    {
        H[k] = 0;
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

void ImageEdit::Contrast(const char* filename, int coeff, bool isMinus) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        MessageBox(NULL, L"Error while file opening!", L"Critical error", MB_OK);
        throw "IO exception";
    }

    fread(info, sizeof(unsigned char), 54, f);

    width = *(int*)&info[18];
    height = *(int*)&info[22];

    row_padded = (width * 3 + 3) & (~3);
    data = new unsigned char[row_padded];

    fl = "contrast.bmp";

    std::remove(fl);
    FILE* file = fopen(fl, "ab");
    if (file == NULL) throw "Argument Exception";

    fwrite(info, sizeof(unsigned char), 54, file);

    for (int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for (int j = 0; j < width * 3; j += 3)
        {
            int tmp;

            if (isMinus) {
                tmp = data[j];
                tmp = (tmp * (100 - coeff) + 128 * coeff) / 100;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j] = tmp;

                tmp = data[j + 1];
                tmp = (tmp * (100 - coeff) + 128 * coeff) / 100;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 1] = tmp;

                tmp = data[j + 2];
                tmp = (tmp * (100 - coeff) + 128 * coeff) / 100;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 2] = tmp;
            }
            else {
                if (coeff == 100) coeff = 99;
                tmp = data[j];
                tmp = (tmp * 100 - 128 * coeff) / (100 - coeff);
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j] = tmp;

                tmp = data[j + 1];
                tmp = (tmp * 100 - 128 * coeff) / (100 - coeff);
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 1] = tmp;

                tmp = data[j + 2];
                tmp = (tmp * 100 - 128 * coeff) / (100 - coeff);
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 2] = tmp;
            }
            
        }
        fwrite(data, sizeof(unsigned char), row_padded, file);
    }

    fclose(f);
    fclose(file);
}

void ImageEdit::ColorBalance(const char* filename, int coeff, char state)
{
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        MessageBox(NULL, L"Error while file opening!", L"Critical error", MB_OK);
        throw "IO exception";
    }

    fread(info, sizeof(unsigned char), 54, f);

    width = *(int*)&info[18];
    height = *(int*)&info[22];

    row_padded = (width * 3 + 3) & (~3);
    data = new unsigned char[row_padded];

    fl = "balance.bmp";

    std::remove(fl);
    FILE* file = fopen("balance.bmp", "ab");
    if (file == NULL) throw "Argument Exception";

    fwrite(info, sizeof(unsigned char), 54, file);

    for (int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);

        for (int j = 0; j < width * 3; j += 3)
        {
            int tmp;
            switch (state) {
            case 'R':

                tmp = data[j+2];
                tmp = tmp + coeff * 128 / 100;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j+2] = tmp;

                break;
            case 'G':

                tmp = data[j + 1];
                tmp = tmp + coeff * 128 / 100;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 1] = tmp;

                break;
            case 'B':

                tmp = data[j];
                tmp = tmp + coeff * 128 / 100;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j] = tmp;

                break;
            case 'W': //this feauture looks like brightness

                tmp = data[j];
                tmp = tmp + coeff * 128 / 100;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j] = tmp;

                tmp = data[j + 1];
                tmp = tmp + coeff * 128 / 100;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 1] = tmp;

                tmp = data[j + 2];
                tmp = tmp + coeff * 128 / 100;
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 2] = tmp;
                break;
            }
        }
        fwrite(data, sizeof(unsigned char), row_padded, file);
    }

    fclose(f);
    fclose(file);
}

void ImageEdit::MultiColorBalance(const char* filename, int R, int G, int B)
{
    FILE* f = fopen(filename, "rb");

    if (f == NULL) {
        MessageBox(NULL, L"Error while file opening!", L"Critical error", MB_OK);
        throw "IO exception";
    }

    fread(info, sizeof(unsigned char), 54, f);

    width = *(int*)&info[18];
    height = *(int*)&info[22];

    row_padded = (width * 3 + 3) & (~3);
    data = new unsigned char[row_padded];

    fl = "multibalance.bmp";

    std::remove(fl);
    FILE* file = fopen(fl, "ab");
    if (file == NULL) throw "Argument Exception";

    fwrite(info, sizeof(unsigned char), 54, file);

    for (int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);

        for (int j = 0; j < width * 3; j += 3)
        {
            int tmp;

            tmp = data[j];
            tmp = tmp + B * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j] = tmp;

            tmp = data[j + 1];
            tmp = tmp + G * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j + 1] = tmp;

            tmp = data[j + 2];
            tmp = tmp + R * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j + 2] = tmp;
        }
        fwrite(data, sizeof(unsigned char), row_padded, file);
    }

    fclose(f);
    fclose(file);
}

const char* ImageEdit::get_file()
{
    return fl;
}
