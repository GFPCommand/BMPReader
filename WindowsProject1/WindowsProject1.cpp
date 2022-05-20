#define _CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

HBITMAP hbm1, hbm2;
BITMAP bmi;

std::wstring tmp_file;

bool isLoaded = false;

int H[256] = { 0 };

TCHAR ChildName[] = _T("1");

RGBQUAD rgb;

RECT rc, histRc;

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2);

void Brightness(const char*);
void Grayscale(const char*);
void Negative(const char*);
void Histogram(const char*);


// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    ChildProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT1));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }
    return (int) msg.wParam;
}

//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;
    WNDCLASSEXW wc;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    if (RegisterClassEx(&wcex)) return 0;

    return 0;
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static HWND hBtn; // дескриптор кнопки
    static HWND hEdt1, hEdt2; // дескрипторы полей редактирования
    static HWND hStat; // дескриптор статического текста
    switch (message)
    {
    case WM_CREATE:
        
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_OPEN:
                OPENFILENAME ofn;

                wchar_t szFileName[MAX_PATH];

                ZeroMemory(&ofn, sizeof(ofn));
                szFileName[0] = 0;

                ofn.lStructSize = sizeof(ofn);
                ofn.hwndOwner = NULL;
                ofn.lpstrFilter = L"BMP Files (*.bmp)\0*.bmp\0\0";
                ofn.lpstrFile = szFileName;
                ofn.lpstrTitle = L"Open file";
                ofn.nMaxFile = MAX_PATH;
                ofn.lpstrDefExt = L"";
                ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

                if (GetOpenFileName(&ofn))
                {
                    std::wstring ws = szFileName;

                    tmp_file = ws;

                    hbm1 = (HBITMAP)LoadImage(0, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

                    RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);

                    std::string str(tmp_file.begin(), tmp_file.end());

                    const char* filename = str.c_str();

                    Histogram(filename);

                    isLoaded = true;

                    RedrawWindow(hWnd, &histRc, NULL, RDW_INVALIDATE);
                }
                break;
            case IDM_SAVE:
                MessageBox(NULL, L"Success", L"File saving", MB_OK);
                break;
            case IDM_SAVEAS:
                OPENFILENAME save_ofn;
                ZeroMemory(&save_ofn, sizeof(OPENFILENAME));

                wchar_t szFile[MAX_PATH];
                ZeroMemory(szFile, MAX_PATH);
                szFile[0] = 0;

                save_ofn.lStructSize = sizeof(OPENFILENAME);
                save_ofn.hwndOwner = NULL;
                save_ofn.lpstrFile = szFile;
                save_ofn.nMaxFile = MAX_PATH;
                save_ofn.lpstrFilter = L"BMP Files (*.bmp)\0*.bmp\0";
                save_ofn.nFilterIndex = 3;
                save_ofn.lpstrTitle = L"Save as";
                save_ofn.lpstrInitialDir = L"C\\Users\\User";
                save_ofn.lpstrDefExt = L"";
                save_ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_OVERWRITEPROMPT;

                if (GetSaveFileName(&save_ofn)) {
                    MessageBox(NULL, szFile, L"File path", MB_OK);
                }

                break;
            case ID_EDIT_BRIGHTNESS:
                if (tmp_file == L"") {
                    MessageBox(NULL, L"NullReferenceException", L"Critical error", MB_OK);
                    break;
                }
                else {
                    std::string str(tmp_file.begin(), tmp_file.end());

                    const char* filename = str.c_str();

                    Brightness(filename);
                    break;
                }
                break;
            case ID_EDIT_GRAYSCALE:
                if (tmp_file == L"") {
                    MessageBox(NULL, L"NullReferenceException", L"Critical error", MB_OK);
                    break;
                }
                else {
                    std::string str(tmp_file.begin(), tmp_file.end());

                    const char* filename = str.c_str();

                    Grayscale(filename);
                    break;
                }
                break;
            case ID_EDIT_NEGATIVE:
                if (tmp_file == L"") {
                    MessageBox(NULL, L"NullReferenceException", L"Critical error", MB_OK);
                    break;
                }
                else {
                    std::string str(tmp_file.begin(), tmp_file.end());

                    const char* filename = str.c_str();

                    Negative(filename);
                    break;
                }
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
    {
        
        GetObject(hbm1, sizeof(BITMAP), &bmi);
        
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HDC cdc = CreateCompatibleDC(hdc);
        HBRUSH hbr = CreateSolidBrush(RGB(0, 0, 0));
        SelectObject(cdc, hbm1);
        GetClientRect(hWnd, &rc);
        GetClientRect(hWnd, &histRc);
        SetRect(&histRc, 600, 16, 1112, 512);
        SetRect(&rc, 16, 16, 512, 512);
        SetStretchBltMode(hdc, HALFTONE);
        StretchBlt(hdc, 16,16,rc.right, rc.bottom, cdc, 0,0,bmi.bmWidth, bmi.bmHeight, SRCCOPY);
        FrameRect(hdc, &rc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));
        FrameRect(hdc, &histRc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));
        DeleteDC(cdc);
        if (isLoaded) {
            
            SelectObject(hdc, hbr);

            for (int i = 0; i < 256; i++)
            {
                H[i] /= 100;
                Line(hdc, 601 + i + 50, 512, 601 + i + 50, (512 - H[i]));
            }
        }
        EndPaint(hWnd, &ps);
    }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Обработчик сообщений для окна "О программе".
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

LRESULT CALLBACK ChildProc(HWND hWnd, UINT message,WPARAM wParam, LPARAM lParam) {

    UNREFERENCED_PARAMETER(lParam);

    switch (message)
    {
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

//void Fading(const char* filename) {
//    FILE* f = fopen(filename, "rb");
//
//    if (f == NULL) throw "Argument Exception";
//
//    unsigned char info[54];
//
//    fread(info, sizeof(unsigned char), 54, f);
//
//    int width = *(int*)&info[18];
//    int height = *(int*)&info[22];
//
//    int row_padded = (width * 3 + 3) & (~3);
//    unsigned char* data = new unsigned char[row_padded];
//
//    std::remove("fading.bmp");
//    FILE* file = fopen("fading.bmp", "ab");
//    if (file == NULL) throw "Argument Exception";
//
//    fwrite(info, sizeof(unsigned char), 54, file);
//
//    for (int i = 0; i < height; i++)
//    {
//        fread(data, sizeof(unsigned char), row_padded, f);
//        for (int j = 0; j < width * 3; j += 3)
//        {
//            int k = (0.3 * (int)data[j] + 0.59 * (int)data[j + 1] + 0.11 * (int)data[j + 2]);
//            data[j] = k;    //blue
//            data[j + 1] = k; //green
//            data[j + 2] = k; //red
//        }
//        fwrite(data, sizeof(unsigned char), row_padded, file);
//    }
//
//    fclose(f);
//    fclose(file);
//}

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc, x1, y1, NULL); //сделать текущими координаты x1, y1
    return LineTo(hdc, x2, y2); //нарисовать линию
}

void Brightness(const char* filename) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) throw "Argument Exception";

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
            tmp += 50 * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j] = tmp;

            tmp = data[j + 1];
            tmp += 50 * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j + 1] = tmp;

            tmp = data[j + 2];
            tmp += 50 * 128 / 100;
            if (tmp > 255) tmp = 255;
            if (tmp < 0) tmp = 0;
            data[j + 2] = tmp;
        }
        fwrite(data, sizeof(unsigned char), row_padded, file);
    }

    fclose(f);
    fclose(file);
}

void Grayscale(const char* filename) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) throw "Argument Exception";

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

void Negative(const char* filename) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) throw "Argument Exception";

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

void Histogram(const char* filename) {
    FILE* f = fopen(filename, "rb");

    if (f == NULL) throw "Argument Exception";

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