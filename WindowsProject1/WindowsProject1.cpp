#define CRT_SECURE_NO_WARNINGS
#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

HBITMAP hbm;
BITMAP bmi;

RGBQUAD rgb;

RECT rc, histRc;

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void ReadBMP(char* filename);

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

    return RegisterClassExW(&wcex);
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
                    //std::wstring ws(szFileName);

                    //std::string str(ws.begin(), ws.end());

                    //const char* file = str.c_str();

                    

                    hbm = (HBITMAP)LoadImage(0, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

                    RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);

                    MessageBox(NULL, szFileName, L"Open file", MB_OK);
                    //char* filename = const_cast<char*>(file);

                    //ReadBMP(filename);
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
            case IDM_EDIT_FADING:
                MessageBox(NULL, L"Fading", L"OK", MB_OK); // TODO
                // Create function ChildProc same as WndProc, add handler, register and so on
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
        
        GetObject(hbm, sizeof(BITMAP), &bmi);
        
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        HDC cdc = CreateCompatibleDC(hdc);
        SelectObject(cdc, hbm);
        GetClientRect(hWnd, &rc);
        GetClientRect(hWnd, &histRc);
        SetRect(&histRc, 600, 16, 1000, 400);
        SetRect(&rc, 16, 16, 512, 512);
        SetStretchBltMode(hdc, HALFTONE);
        StretchBlt(hdc, 16,16,rc.right, rc.bottom, cdc, 0,0,bmi.bmWidth, bmi.bmHeight, SRCCOPY);
        FrameRect(hdc, &rc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));
        FrameRect(hdc, &histRc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));
        DeleteDC(cdc);
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

void ReadBMP(char* filename) {
    hbm = (HBITMAP)LoadImage(0, (LPCWSTR)filename, IMAGE_BITMAP, 32, 32, LR_LOADFROMFILE);
    GetObject(hbm, sizeof(BITMAP), &bmi);

#pragma warning(suppress : 4996)
    FILE* f = fopen(filename, "rb");

    if (f == NULL) throw "Argument Exception";

    unsigned char info[54];

    fread(info, sizeof(unsigned char), 54, f);

    int width = *(int*)&info[18];
    int height = *(int*)&info[22];

    int row_padded = (width * 3 + 3) & (~3);
    unsigned char* data = new unsigned char[row_padded];
    //unsigned char tmp;
}