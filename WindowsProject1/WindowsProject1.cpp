#include "framework.h"
#include "WindowsProject1.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

HBITMAP hbm1, hbm2;
BITMAP bmi;

std::wstring tmp_file, save_as_file;

bool isLoaded = false;
bool isSave = true;
bool isSaveAs = false;

int lastState = 0;

enum modes {Normal = 0, Brightness, Grayscale, Negative, ContrastF, Colors, Multicolors};

ImageEdit img;

wchar_t ChildName[] = _T("1");

RGBQUAD rgb;

RECT rc, histRc, backgroundRc, text1Rc, text2Rc;

BOOL Line(HDC hdc, int x1, int y1, int x2, int y2);
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

                isLoaded = true;

                if (GetOpenFileName(&ofn))
                {
                    std::wstring ws = szFileName;

                    tmp_file = ws;

                    hbm1 = (HBITMAP)LoadImage(0, szFileName, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

                    RedrawWindow(hWnd, &rc, NULL, RDW_INVALIDATE);

                    std::string str(tmp_file.begin(), tmp_file.end());

                    const char* filename = str.c_str();

                    img.Histogram(filename);

                    RedrawWindow(hWnd, &histRc, NULL, RDW_INVALIDATE);
                }
                break;
            case IDM_SAVE:
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
            case IDM_SAVEAS:
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
            case ID_EDIT_BRIGHTNESS:
                if (tmp_file == L"") {
                    MessageBox(NULL, L"NullReferenceException", L"Critical error", MB_OK);
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
            case ID_EDIT_GRAYSCALE:
                if (tmp_file == L"") {
                    MessageBox(NULL, L"NullReferenceException", L"Critical error", MB_OK);
                    break;
                }
                else {
                    if (isSave) {
                        std::string str(tmp_file.begin(), tmp_file.end());

                        const char* filename = str.c_str();

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

                            const char* filename = str.c_str();

                            img.Grayscale(filename);

                            isSave = false;
                            lastState = Grayscale;

                            break;
                        }
                    }
                }
                break;
            case ID_EDIT_NEGATIVE:
                if (tmp_file == L"") {
                    MessageBox(NULL, L"NullReferenceException", L"Critical error", MB_OK);
                    break;
                }
                else {
                    if (isSave) {
                        std::string str(tmp_file.begin(), tmp_file.end());

                        const char* filename = str.c_str();

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

                            const char* filename = str.c_str();

                            img.Negative(filename);

                            isSave = false;
                            lastState = Negative;

                            break;
                        }
                    }
                }
                break;
            case ID_EDIT_CONTRAST:
                if (tmp_file == L"") {
                    MessageBox(NULL, L"NullReferenceException", L"Critical error", MB_OK);
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
            case ID_EDIT_COLORBALANCE:
                if (tmp_file == L"") {
                    MessageBox(NULL, L"NullReferenceException", L"CriticalError", MB_OK);
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
            case ID_EDIT_MULTICOLOR:
                if (tmp_file == L"") {
                    MessageBox(NULL, L"NullReferenceException", L"CriticalError", MB_OK);
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
        GetClientRect(hWnd, &backgroundRc);

        SetRect(&histRc, 600, 16, 1112, 512);
        SetRect(&backgroundRc, 599, 15, 1113, 513);
        SetRect(&rc, 16, 16, 512, 512);

        SetStretchBltMode(hdc, HALFTONE);

        StretchBlt(hdc, 16,16,rc.right, rc.bottom, cdc, 0,0,bmi.bmWidth, bmi.bmHeight, SRCCOPY);

        FrameRect(hdc, &rc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));
        FrameRect(hdc, &histRc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));
        FrameRect(hdc, &backgroundRc, HBRUSH(CreateSolidBrush(RGB(0, 0, 0))));

        DeleteDC(cdc);

        if (isLoaded) {

            int w = bmi.bmWidth;
            int h = bmi.bmHeight;

            float div;

            if (w > h) div = (w % h) / 2;
            else div = (h % w) / 2;

            if (div == 0) div = 1;

            FillRect(hdc, &histRc, HBRUSH(CreateSolidBrush(RGB(255, 255, 255))));

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
        SetRect(&text1Rc, 600, 525, 650, 600);
        DrawText(hdc, text1, ARRAYSIZE(text1), &text1Rc, DT_SINGLELINE);
        
        GetClientRect(hWnd, &text2Rc);
        SetRect(&text2Rc, 1090, 525, 1125, 600);
        DrawText(hdc, text2, ARRAYSIZE(text2), &text2Rc, DT_SINGLELINE);

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

            const char* filename = str.c_str();

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