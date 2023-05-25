#include "framework.h"

FILE *f;

void ImageEdit::Histogram(const char* filename) {
    OpenFile(filename);

    row_padded = (width * 3 + 3) & (~3);
    data = new unsigned char[row_padded];

    int brightness = 0;

    for (int y = 0; y < height; y++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for (int x = 0; x < width; x++)
        {
            brightness = ((int)(data[x] * 0.11) + (int)(data[x + 1] * 0.59) + (int)(data[x + 2] * 0.3)); // берется параметр яркости, вычисляемый для кадого пикселя по формуле I = 0.3 * R + 0.59 * G + 0.11 * B
            H[brightness]++; // запись количества пикселей на каждый уровень яркости
        }
    }
}

void ImageEdit::MultiColorHistogram(const char* filename) {
    OpenFile(filename);

    row_padded = (width * 3 + 3) & (~3);
    data = new unsigned char[row_padded];

    int brightness = 0;

    for (int y = 0; y < height; y++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for (int x = 0; x < width; x++)
        {
            brightness = ((int)(data[x + 2] * 0.3)); // берется параметр яркости, вычисляемый для кадого пикселя по формуле I = 0.3 * R + 0.59 * G + 0.11 * B
            H_R[brightness]++; // запись количества пикселей на каждый уровень яркости
        }
    }

    for (int y = 0; y < height; y++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for (int x = 0; x < width; x++)
        {
            brightness = ((int)((int)(data[x + 1] * 0.59))); // берется параметр яркости, вычисляемый для кадого пикселя по формуле I = 0.3 * R + 0.59 * G + 0.11 * B
            H_G[brightness]++; // запись количества пикселей на каждый уровень яркости
        }
    }

    for (int y = 0; y < height; y++)
    {
        fread(data, sizeof(unsigned char), row_padded, f);
        for (int x = 0; x < width; x++)
        {
            brightness = ((int)(data[x] * 0.11)); // берется параметр яркости, вычисляемый для кадого пикселя по формуле I = 0.3 * R + 0.59 * G + 0.11 * B
            H_B[brightness]++; // запись количества пикселей на каждый уровень яркости
        }
    }
}

void ImageEdit::Brightness(const char* filename, int coeff) {
    OpenFile(filename);

    fl = "brightness.bmp"; // файл сохранения

    std::remove(fl);
    FILE* file = fopen(fl, "ab"); // создание файла сохранения
    if (file == NULL) {
        throw "Argument Exception";
        exit(1);
    }

    fwrite(info, sizeof(unsigned char), 54, file); // копирование данных для bmp

    for (int i = 0; i < height; i++)
    {
        fread(data, sizeof(unsigned char), row_padded, f); // построчное считывание данных
        for (int j = 0; j < width * 3; j += 3)
        {
            int tmp;

            // побитовая обработка для каждого цветового канала
            //blue
            tmp = data[j];
            tmp += coeff * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j] = tmp;

            //green
            tmp = data[j + 1];
            tmp += coeff * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j + 1] = tmp;

            //red
            tmp = data[j + 2];
            tmp += coeff * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j + 2] = tmp;
        }
        // запись измененных данных в файл
        fwrite(data, sizeof(unsigned char), row_padded, file);
    }

    fclose(f);
    fclose(file);
}

void ImageEdit::Grayscale(const char* filename) {
    OpenFile(filename);

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
            // проведение бинаризации
            // (B + G + R) / 3
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
    OpenFile(filename);

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
            // инвертирование цветов - негатив
            data[j] = (255 - (int)data[j]);    //blue
            data[j + 1] = (255 - (int)data[j + 1]); //green
            data[j + 2] = (255 - (int)data[j + 2]); //red
        }
        fwrite(data, sizeof(unsigned char), row_padded, file);
    }

    fclose(f);
    fclose(file);
}

void ImageEdit::Contrast(const char* filename, int coeff, bool isMinus) {
    OpenFile(filename);

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

            // coeff - процент изменения, N
            if (isMinus) {
                tmp = data[j];
                tmp = (tmp * (100 - coeff) + 128 * coeff) / 100; // (B * (100 - N) + 128 * N) / 100
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j] = tmp;

                tmp = data[j + 1];
                tmp = (tmp * (100 - coeff) + 128 * coeff) / 100; // (G * (100 - N) + 128 * N) / 100
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 1] = tmp;

                tmp = data[j + 2];
                tmp = (tmp * (100 - coeff) + 128 * coeff) / 100; // (R * (100 - N) + 128 * N) / 100
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 2] = tmp;
            }
            else {
                if (coeff == 100) coeff = 99;
                tmp = data[j];
                tmp = (tmp * 100 - 128 * coeff) / (100 - coeff); // (B * 100 - 128 * N) / (100 - N)
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j] = tmp;

                tmp = data[j + 1];
                tmp = (tmp * 100 - 128 * coeff) / (100 - coeff); // (G * 100 - 128 * N) / (100 - N)
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 1] = tmp;

                tmp = data[j + 2];
                tmp = (tmp * 100 - 128 * coeff) / (100 - coeff); // (R * 100 - 128 * N) / (100 - N)
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
    OpenFile(filename);

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
            switch (state) { // выбор цветового канала дял обработки
            case 'R':

                tmp = data[j+2];
                tmp = tmp + coeff * 128 / 100; // (R + 128 * N) / 100
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j+2] = tmp;

                break;
            case 'G':

                tmp = data[j + 1];
                tmp = tmp + coeff * 128 / 100; // (G + 128 * N) / 100
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j + 1] = tmp;

                break;
            case 'B':

                tmp = data[j];
                tmp = tmp + coeff * 128 / 100; // (B + 128 * N) / 100
                if (tmp > 255) tmp = 255;
                if (tmp < 0) tmp = 0;
                data[j] = tmp;

                break;
            case 'W': //Проход по всем каналам сразу. Аналогично изменению яркости

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
    OpenFile(filename);

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

            // Одновременное изменение цветовых параметров. Для каждого цветового канала свое значение

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

void ImageEdit::OpenFile(const char* filename) {
    f = fopen(filename, "rb"); // подключение к файлу

    if (f == NULL) {
        MessageBox(NULL, L"Error while file opening!", L"Critical error", MB_OK);
        throw "IO Exception";
    }

    fread(info, sizeof(unsigned char), 54, f); // чтение данных о файле

    width = *(int*)&info[18]; // получение ширины изображения
    height = *(int*)&info[22]; // получение высоты изображения

    bitPerPixel = *(int*)&info[28];

    colorsCount = *(int*)&info[46];

    filesize = *(int*)&info[2];

    compression = *(int*)&info[30];

    row_padded = (width * 3 + 3) & (~3); // получение количества строк
    data = new unsigned char[row_padded]; // создание массива для чтения данных
}