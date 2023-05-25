#include "framework.h"
#include "WindowsProject1.h"

constexpr auto MAX_LOADSTRING = 100;

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна
HMENU menu;

HBITMAP hbm1, hbm2;
BITMAP bmi; // структура для изображения

std::wstring tmp_file, save_as_file; // строка для файлов

HWND fileInfoWindowPixelAmount;

int pxCount;

TCHAR pixelCount[256];
TCHAR bitsPerPixel[256];
TCHAR usedColors[256];
TCHAR fileSize[256];
TCHAR widthHeght[256];
TCHAR *compressionType;
LPTSTR lpsz;

// флаги состояний
bool isLoaded = false;
bool isSave = true;
bool isSaveAs = false;

int lastState = 0;

// режим работы

enum modes {Normal = 0, Brightness, Grayscale, Negative, ContrastF, Colors, Multicolors};

// создание объекта класса для работы с изображением
ImageEdit img;

// прямоугольные области для отрисовки
RECT rc, histRc, backgroundRc, text1Rc, text2Rc, pixelCountRc;

// функция для рисования линий
BOOL Line(HDC hdc, int x1, int y1, int x2, int y2);

// функция для сохранения файлов
void Save(HWND, int);

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Bright(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    Contrast(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    ColorBalance(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    MultiColorBalance(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    FileInformation(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    MultiColorHistograms(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

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

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   HWND hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, 1100, 700, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   menu = GetMenu(hWnd);

   EnableMenuItem(menu, IDM_SAVE, MF_DISABLED);
   EnableMenuItem(menu, IDM_SAVEAS, MF_DISABLED);

   EnableMenuItem(menu, ID_EDIT_BRIGHTNESS, MF_DISABLED);
   EnableMenuItem(menu, ID_EDIT_GRAYSCALE, MF_DISABLED);
   EnableMenuItem(menu, ID_EDIT_NEGATIVE, MF_DISABLED);
   EnableMenuItem(menu, ID_EDIT_CONTRAST, MF_DISABLED);
   EnableMenuItem(menu, ID_EDIT_COLORBALANCE, MF_DISABLED);
   EnableMenuItem(menu, ID_EDIT_MULTICOLOR, MF_DISABLED);

   EnableMenuItem(menu, ID_HELP_FILEINFO, MF_DISABLED);
   EnableMenuItem(menu, ID_HELP_MULTICOLOR_HISTOGRAM, MF_DISABLED);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_CREATE:
        
        break;
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About); // вывод спраки
                break;
            case IDM_OPEN: // открытие файлов
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
                    isLoaded = true;

                    EnableMenuItem(menu, IDM_SAVE, MF_ENABLED);
                    EnableMenuItem(menu, IDM_SAVEAS, MF_ENABLED);

                    EnableMenuItem(menu, ID_EDIT_BRIGHTNESS, MF_ENABLED);
                    EnableMenuItem(menu, ID_EDIT_GRAYSCALE, MF_ENABLED);
                    EnableMenuItem(menu, ID_EDIT_NEGATIVE, MF_ENABLED);
                    EnableMenuItem(menu, ID_EDIT_CONTRAST, MF_ENABLED);
                    EnableMenuItem(menu, ID_EDIT_COLORBALANCE, MF_ENABLED);
                    EnableMenuItem(menu, ID_EDIT_MULTICOLOR, MF_ENABLED);

                    EnableMenuItem(menu, ID_HELP_FILEINFO, MF_ENABLED);
                    EnableMenuItem(menu, ID_HELP_MULTICOLOR_HISTOGRAM, MF_ENABLED);

                    std::wstring ws = szFileName;

                    tmp_file = ws;

                    hbm1 = (HBITMAP)LoadImage(0, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

                    RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);

                    std::string str(tmp_file.begin(), tmp_file.end());

                    const char *filename = str.c_str();

                    img.Histogram(filename);

                    RedrawWindow(hWnd, &histRc, NULL, RDW_INVALIDATE);
                }
                break;
            case IDM_SAVE: // сохранение файла в то же место
                if (isLoaded) {
                    MessageBox(NULL, L"Success", L"File saving", MB_OK);

                    if (isLoaded && !isSave) {
                        Save(hWnd, lastState);
                    }
                }
                else {
                    MessageBox(NULL, L"File not open!", L"Error", MB_OK);
                }
                
                break;
            case IDM_SAVEAS: // сохранение с конкретным путем
                if (isLoaded) {
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

                    isSave = true;

                    if (GetSaveFileName(&save_ofn)) {
                        MessageBox(NULL, L"Success", L"File saving", MB_OK);

                        std::wstring ws = szFile;

                        save_as_file = ws;

                        isSaveAs = true;

                        Save(hWnd, lastState);
                    }
                }
                else {
                    MessageBox(NULL, L"File not open!", L"Error", MB_OK);
                }
                
                break;
            case ID_EDIT_BRIGHTNESS: // открытие настроек яркости
                if (tmp_file == L"") {
                    MessageBox(NULL, L"File not open!", L"Critical error", MB_OK);
                    break;
                }
                else {
                    if (isSave){
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_BRIGHTNESS), hWnd, Bright);
                        
                        break;
                    }
                    else {
                        int chk = MessageBox(NULL, L"File not saving. Save it now?", L"Error", MB_YESNO);

                        if (chk == IDYES) {
                            Save(hWnd, lastState);
                        }
                        else if (chk == IDNO) {
                            DialogBox(hInst, MAKEINTRESOURCE(IDD_BRIGHTNESS), hWnd, Bright);

                            break;
                        }
                    }
                    
                }
                break;
            case ID_EDIT_GRAYSCALE: // бинаризация изображения
                if (tmp_file == L"") {
                    MessageBox(NULL, L"File not open!", L"Critical error", MB_OK);
                    break;
                }
                else {
                    if (isSave) {
                        std::string str(tmp_file.begin(), tmp_file.end());

                        const char *filename = str.c_str();

                        img.Grayscale(filename);

                        isSave = false;
                        lastState = Grayscale;

                        break;
                    }
                    else {
                        int chk = MessageBox(NULL, L"File not saving. Save it now?", L"Error", MB_YESNO);

                        if (chk == IDYES) {
                            Save(hWnd, lastState);
                        }
                        else if (chk == IDNO) {
                            std::string str(tmp_file.begin(), tmp_file.end());

                            const char *filename = str.c_str();

                            img.Grayscale(filename);

                            isSave = false;
                            lastState = Grayscale;

                            break;
                        }
                    }
                }
                break;
            case ID_EDIT_NEGATIVE: // цветовое инвертирование изображения - негатив
                if (tmp_file == L"") {
                    MessageBox(NULL, L"File not open!", L"Critical error", MB_OK);
                    break;
                }
                else {
                    if (isSave) {
                        std::string str(tmp_file.begin(), tmp_file.end());

                        const char *filename = str.c_str();

                        img.Negative(filename);

                        isSave = false;
                        lastState = Negative;

                        break;
                    }
                    else {
                        int chk = MessageBox(NULL, L"File not saving. Save it now?", L"Error", MB_YESNO);

                        if (chk == IDYES) {
                            Save(hWnd, lastState);
                        }
                        else if (chk == IDNO) {
                            std::string str(tmp_file.begin(), tmp_file.end());

                            const char *filename = str.c_str();

                            img.Negative(filename);

                            isSave = false;
                            lastState = Negative;

                            break;
                        }
                    }
                }
                break;
            case ID_EDIT_CONTRAST: // настройка контраста изображения
                if (tmp_file == L"") {
                    MessageBox(NULL, L"File not open!", L"Critical error", MB_OK);
                    break;
                }
                else {
                    if (isSave) {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_CONTRAST), hWnd, Contrast);
                        break;
                    }
                    else {
                        int chk = MessageBox(NULL, L"File not saving. Save it now?", L"Error", MB_YESNO);

                        if (chk == IDYES) {
                            Save(hWnd, lastState);
                        }
                        else if (chk == IDNO) {
                            DialogBox(hInst, MAKEINTRESOURCE(IDD_CONTRAST), hWnd, Contrast);
                            break;
                        }
                    }
                }
                break;
            case ID_EDIT_COLORBALANCE: // настройка цветового баланса
                if (tmp_file == L"") {
                    MessageBox(NULL, L"File not open!", L"Critical error", MB_OK);
                    break;
                }
                else {
                    if (isSave) {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_COLORBALANCE), hWnd, ColorBalance);
                        break;
                    }
                    else {
                        int chk = MessageBox(NULL, L"File not saving. Save it now?", L"Error", MB_YESNO);

                        if (chk == IDYES) {
                            Save(hWnd, lastState);
                        }
                        else if (chk == IDNO) {
                            DialogBox(hInst, MAKEINTRESOURCE(IDD_COLORBALANCE), hWnd, ColorBalance);
                            break;
                        }
                    }
                }
                break;
            case ID_EDIT_MULTICOLOR: // одновременное изменение цветовых параметров
                if (tmp_file == L"") {
                    MessageBox(NULL, L"File not open!", L"Critical error", MB_OK);
                    break;
                }
                else {
                    if (isSave) {
                        DialogBox(hInst, MAKEINTRESOURCE(IDD_MULTIBALANCE), hWnd, MultiColorBalance);
                        break;
                    }
                    else {
                        int chk = MessageBox(NULL, L"File not saving. Save it now?", L"Error", MB_YESNO);

                        if (chk == IDYES) {
                            Save(hWnd, lastState);
                        }
                        else if (chk == IDNO) {
                            DialogBox(hInst, MAKEINTRESOURCE(IDD_MULTIBALANCE), hWnd, MultiColorBalance);
                            break;
                        }
                    }
                }
                break;
            case ID_HELP_FILEINFO:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_FILE_INFO), hWnd, FileInformation);
                break;
            case ID_HELP_MULTICOLOR_HISTOGRAM:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_MULTICOLOR_HISTOGRAMS), hWnd, MultiColorHistograms);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT: // отрисовка
    {
        
        GetObject(hbm1, sizeof(BITMAP), &bmi); // получение изображения
        
        PAINTSTRUCT ps; // структура дял возможности взаимодействовать с отрисовкой
        HDC hdc = BeginPaint(hWnd, &ps); // подготовка окна приложения для отрисовки
        HDC cdc = CreateCompatibleDC(hdc); // создание в ОЗУ контекста изображения
        HBRUSH hbr = CreateSolidBrush(RGB(0, 0, 0)); // создание кисти

        SelectObject(cdc, hbm1); // выбор bmp из памяти

        // получение координат окна
        GetClientRect(hWnd, &rc);
        GetClientRect(hWnd, &histRc);
        GetClientRect(hWnd, &backgroundRc);

        // настройка размеров прямоугольников для рисования и их координаты
        SetRect(&histRc, 650, 151, 1040, 512);
        SetRect(&backgroundRc, 649, 150, 1041, 513);
        SetRect(&rc, 16, 16, 512, 512);

        // установка режима растягивания изображения
        SetStretchBltMode(hdc, HALFTONE);

        // копирование изображения из памяти в целевой прямоугольник
        StretchBlt(hdc, 16,16,rc.right, rc.bottom, cdc, 0,0,bmi.bmWidth, bmi.bmHeight, SRCCOPY);

        // рисование границ прямоугольных областей
        FrameRect(hdc, &rc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));
        FrameRect(hdc, &histRc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));
        FrameRect(hdc, &backgroundRc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));

        // выгрузка изображения из памяти
        DeleteDC(cdc);

        if (isLoaded) { // если изображение получено, выводим его гистограмму

            int w = img.width;
            int h = img.height;

            float div;

            //установка соотношения сторон изображения для удобного вывода гистограммы
            if (w > h) div = (w % h) * 0.175;
            else div = (h % w) * 0.175;

            if (div == 0) div = 1; // для предотвращения деления на 0

            // заливаем область белым цветом
            FillRect(hdc, &histRc, HBRUSH(CreateSolidBrush(RGB(255, 255, 255))));

            // выбираем кисть для рисования
            SelectObject(hdc, hbr);

            for (int i = 0; i < 256; i++)
            {
                img.H[i] /= div;
                Line(hdc, 601 + i*1.5 + 50, 512, 601 + i*1.5 + 50, (512 - img.H[i]));
            }
        }

        TCHAR text1[] = L"0";
        TCHAR text2[] = L"255";

        GetClientRect(hWnd, &text1Rc);
        SetRect(&text1Rc, 650, 525, 700, 600);
        DrawText(hdc, text1, ARRAYSIZE(text1), &text1Rc, DT_SINGLELINE);
        
        GetClientRect(hWnd, &text2Rc);
        SetRect(&text2Rc, 1020, 525, 1070, 600);
        DrawText(hdc, text2, ARRAYSIZE(text2), &text2Rc, DT_SINGLELINE);

        DeleteObject(hbr);

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

INT_PTR CALLBACK FileInformation(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        pxCount = img.width * img.height;

        _stprintf(pixelCount, TEXT("%d px"), pxCount);
        fileInfoWindowPixelAmount = GetDlgItem(hDlg, IDC_STATIC_PIXEL_AMOUNT);
        SetWindowText(fileInfoWindowPixelAmount, pixelCount);

        _stprintf(bitsPerPixel, TEXT("%d bit/px"), img.bitPerPixel);
        fileInfoWindowPixelAmount = GetDlgItem(hDlg, IDC_STATIC_BIT_PIXEL);
        SetWindowText(fileInfoWindowPixelAmount, bitsPerPixel);

        _stprintf(usedColors, TEXT("%d colors"), img.colorsCount);
        fileInfoWindowPixelAmount = GetDlgItem(hDlg, IDC_STATIC_COLORS);
        SetWindowText(fileInfoWindowPixelAmount, usedColors);

        _stprintf(fileSize, TEXT("%d Kb"), img.filesize / 1024);
        fileInfoWindowPixelAmount = GetDlgItem(hDlg, IDC_STATIC_FILESIZE);
        SetWindowText(fileInfoWindowPixelAmount, fileSize);

        _stprintf(widthHeght, TEXT("%dx%d px"), img.width, img.height);
        fileInfoWindowPixelAmount = GetDlgItem(hDlg, IDC_STATIC_FILE_WH);
        SetWindowText(fileInfoWindowPixelAmount, widthHeght);
        
        switch (img.compression)
        {
            case 0:
                compressionType = new TCHAR[256];

                _tcscpy(compressionType, L"BI_RGB no compression");

                fileInfoWindowPixelAmount = GetDlgItem(hDlg, IDC_STATIC_COMPRESSION_TYPE);
                SetWindowText(fileInfoWindowPixelAmount, compressionType);
                break;
            case 1:
                compressionType = new TCHAR[256];

                _tcscpy(compressionType, L"BI_RLE8 8bit RLE encoding");

                fileInfoWindowPixelAmount = GetDlgItem(hDlg, IDC_STATIC_COMPRESSION_TYPE);
                SetWindowText(fileInfoWindowPixelAmount, compressionType);
                break;
            case 2:
                compressionType = new TCHAR[256];

                _tcscpy(compressionType, L"BI_RLE4 4bit encoding");

                fileInfoWindowPixelAmount = GetDlgItem(hDlg, IDC_STATIC_COMPRESSION_TYPE);
                SetWindowText(fileInfoWindowPixelAmount, compressionType);
                break;
            default:
                break;
        }

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

INT_PTR CALLBACK MultiColorHistograms(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT histRc_R, histRc_G, histRc_B;

    std::string str(tmp_file.begin(), tmp_file.end());

    const char* filename = str.c_str();

    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        RedrawWindow(hDlg, nullptr, NULL, RDW_INVALIDATE);
        return (INT_PTR)TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_CANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
    case WM_PAINT:
        img.MultiColorHistogram(filename);

        PAINTSTRUCT ps; // структура дял возможности взаимодействовать с отрисовкой
        HDC hdc = BeginPaint(hDlg, &ps); // подготовка окна приложения для отрисовки
        HBRUSH hbr_R = CreateSolidBrush(RGB(255, 0, 0)); // создание кисти
        HBRUSH hbr_G = CreateSolidBrush(RGB(0, 255, 0)); // создание кисти
        HBRUSH hbr_B = CreateSolidBrush(RGB(0, 0, 255)); // создание кисти
        
        HPEN hp_R = CreatePen(PS_SOLID, 1, RGB(255, 0, 0)); // создание пера
        HPEN hp_G = CreatePen(PS_SOLID, 1, RGB(0, 255, 0)); // создание пера
        HPEN hp_B = CreatePen(PS_SOLID, 1, RGB(0, 0, 255)); // создание пера

        GetClientRect(hDlg, &histRc_R);
        GetClientRect(hDlg, &histRc_G);
        GetClientRect(hDlg, &histRc_B);

        SetRect(&histRc_R, 16, 16, 250, 250);
        SetRect(&histRc_G, 16, 260, 250, 500);
        SetRect(&histRc_B, 260, 16, 510, 250);

        if (isLoaded) {
            int w = img.width;
            int h = img.height;

            float div;

            //установка соотношения сторон изображения для удобного вывода гистограммы
            if (w > h) div = (w % h) * 0.5;
            else div = (h % w) * 0.5;

            if (div == 0) div = 0.01; // для предотвращения деления на 0

            FillRect(hdc, &histRc_R, HBRUSH(CreateSolidBrush(RGB(255, 255, 255))));

            SelectObject(hdc, hp_R);

            for (int i = 0; i < 256; i++)
            {
                img.H_R[i] /= div;
                img.H_R[i] = 250 - img.H_R[i] > 16 ? img.H_R[i] : 234;
                Line(hdc, 16 + i * 1.5 + 5, 249, 16 + i * 1.5 + 5, (250 - img.H_R[i]));
            }

            FillRect(hdc, &histRc_G, HBRUSH(CreateSolidBrush(RGB(255, 255, 255))));

            SelectObject(hdc, hp_G);

            for (int i = 0; i < 256; i++)
            {
                img.H_G[i] /= div;
                img.H_G[i] = 500 - img.H_G[i] > 260 ? img.H_G[i] : 234;
                Line(hdc, 16 + i * 1.5 + 5, 499, 16 + i * 1.5 + 5, (500 - img.H_G[i]));
            }

            FillRect(hdc, &histRc_B, HBRUSH(CreateSolidBrush(RGB(255, 255, 255))));

            SelectObject(hdc, hp_B);

            for (int i = 0; i < 256; i++)
            {
                img.H_B[i] /= div;
                img.H_B[i] = 250 - img.H_B[i] > 16 ? img.H_B[i] : 234;
                Line(hdc, 260 + i * 1.5 + 5, 249, 260 + i * 1.5 + 5, (250 - img.H_B[i]));
            }
        }

        FrameRect(hdc, &histRc_R, hbr_R);
        FrameRect(hdc, &histRc_G, hbr_G);
        FrameRect(hdc, &histRc_B, hbr_B);

        DeleteObject(hbr_R);
        DeleteObject(hbr_G);
        DeleteObject(hbr_B);

        EndPaint(hDlg, &ps);

        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Bright(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_APPLY)
        {
            HWND check = GetDlgItem(hDlg, IDC_CHECK_MODE);
            HWND input = GetDlgItem(hDlg, IDC_COEFF);

            bool isMinus = (SendMessage(check, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;
            
            std::string str(tmp_file.begin(), tmp_file.end());

            const char *filename = str.c_str();

            wchar_t str_coeff[5] = { 0 };

            int coeff = 0;
            GetWindowText(input, str_coeff, 5);
            coeff = _wtoi(str_coeff);

            if (isMinus) coeff *= -1;

            img.Brightness(filename, coeff);

            isSave = false;
            lastState = Brightness;

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_CANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK Contrast(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_APPLY)
        {
            HWND check = GetDlgItem(hDlg, IDC_CHECK_CONTRAST_MODE);
            HWND input = GetDlgItem(hDlg, IDC_COEFF_CONTRAST);

            bool isMinus = (SendMessage(check, BM_GETCHECK, 0, 0) == BST_CHECKED) ? true : false;

            std::string str(tmp_file.begin(), tmp_file.end());

            const char* filename = str.c_str();

            wchar_t str_coeff[5] = { 0 };

            int coeff = 0;
            GetWindowText(input, str_coeff, 5);
            coeff = _wtoi(str_coeff);

            img.Contrast(filename, coeff, isMinus);

            isSave = false;
            lastState = ContrastF;

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_CANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK ColorBalance(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_APPLY)
        {
            HWND checkR, checkG, checkB, checkW;
            HWND input;

            checkR = GetDlgItem(hDlg, IDC_RADIO_R);
            checkG = GetDlgItem(hDlg, IDC_RADIO_G);
            checkB = GetDlgItem(hDlg, IDC_RADIO_B);
            checkW = GetDlgItem(hDlg, IDC_RADIO_W);

            input = GetDlgItem(hDlg, IDC_COEFF_CB);

            std::string str(tmp_file.begin(), tmp_file.end());

            const char* filename = str.c_str();

            wchar_t str_coeff[5] = { 0 };

            int coeff = 0;

            GetWindowText(input, str_coeff, 5);

            coeff = _wtoi(str_coeff);

            if (IsDlgButtonChecked(hDlg, IDC_RADIO_R) == BST_CHECKED) {
                img.ColorBalance(filename, coeff, 'R');
            }
            else if (IsDlgButtonChecked(hDlg, IDC_RADIO_G) == BST_CHECKED) {
                img.ColorBalance(filename, coeff, 'G');
            }
            else if (IsDlgButtonChecked(hDlg, IDC_RADIO_B) == BST_CHECKED) {
                img.ColorBalance(filename, coeff, 'B');
            }
            else if (IsDlgButtonChecked(hDlg, IDC_RADIO_W) == BST_CHECKED) {
                img.ColorBalance(filename, coeff, 'W');
            }

            isSave = false;
            lastState = Colors;

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_CANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

INT_PTR CALLBACK MultiColorBalance(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDC_APPLY) {
            HWND inputR, inputG, inputB;

            inputR = GetDlgItem(hDlg, IDC_COEFF_CB_R);
            inputG = GetDlgItem(hDlg, IDC_COEFF_CB_G);
            inputB = GetDlgItem(hDlg, IDC_COEFF_CB_B);

            std::string str(tmp_file.begin(), tmp_file.end());

            const char* filename = str.c_str();

            wchar_t str_coeff_R[5] = { 0 };
            wchar_t str_coeff_G[5] = { 0 };
            wchar_t str_coeff_B[5] = { 0 };

            int coeff_R = 0;
            int coeff_G = 0;
            int coeff_B = 0;

            GetWindowText(inputR, str_coeff_R, 5);
            GetWindowText(inputG, str_coeff_G, 5);
            GetWindowText(inputB, str_coeff_B, 5);

            coeff_R = _wtoi(str_coeff_R);
            coeff_G = _wtoi(str_coeff_G);
            coeff_B = _wtoi(str_coeff_B);

            img.MultiColorBalance(filename, coeff_R, coeff_G, coeff_B);

            isSave = false;
            lastState = Multicolors;

            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        else if (LOWORD(wParam) == IDCANCEL || LOWORD(wParam) == IDC_CANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2)
{
    MoveToEx(hdc, x1, y1, NULL);
    return LineTo(hdc, x2, y2);
}

void Save(HWND hWnd, int state) {
    std::string cmd = "copy ";
    std::string src, dest;

    std::string str(tmp_file.begin(), tmp_file.end());

    const char* filename = str.c_str();

    switch (state)
    {
    case 0:
        src = str;
        cmd += src;
        break;
    case 1:
        src = "brightness.bmp";
        cmd += src;
        break;
    case 2:
        src = "grayscale.bmp";
        cmd += src;
        break;
    case 3:
        src = "negative.bmp";
        cmd += src;
        break;
    case 4:
        src = "contrast.bmp";
        cmd += src;
        break;
    case 5:
        src = "balance.bmp";
        cmd += src;
        break;
    case 6:
        src = "multibalance.bmp";
        cmd += src;
        break;
    default:
        break;
    }

    cmd += " ";

    if (isSaveAs) {
        std::string save_as_str(save_as_file.begin(), save_as_file.end());

        const char* save_file = save_as_str.c_str();

        cmd += save_file;
    } else
        cmd += filename;

    system(cmd.c_str());
    if (state != 0)
        std::remove(src.c_str());

    const wchar_t* file = tmp_file.c_str();

    hbm1 = (HBITMAP)LoadImage(0, file, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);

    img.Histogram(filename);

    isLoaded = true;
    isSave = true;

    lastState = Normal;

    RedrawWindow(hWnd, &histRc, NULL, RDW_INVALIDATE);
}