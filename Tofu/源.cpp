#include <windows.h>
#include <gdiplus.h>
#include <ctime>
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#pragma comment(lib, "gdiplus.lib")

#define FLAG_PATH "flag.loveyou"
#define IMAGE_PATH "Tofv.jpg"

#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))

using namespace Gdiplus;

HINSTANCE hInst;
Image* emojiImage = nullptr;
POINT imgPos = { 200, 200 };
const int IMG_WIDTH = 150;
const int IMG_HEIGHT = 150;
HWND hMainWnd;

void btea(uint32_t* v, int n, uint32_t const key[4])
{
    uint32_t y, z, sum;
    unsigned p, rounds, e;
    if (n > 1)            /* Coding Part */
    {
        rounds = 6 + 52 / n;
        sum = 0;
        z = v[n - 1];
        do
        {
            sum += DELTA;
            e = (sum >> 2) & 3;
            for (p = 0; p < n - 1; p++)
            {
                y = v[p + 1];
                z = v[p] += MX;
            }
            y = v[0];
            z = v[n - 1] += MX;
        } while (--rounds);
    }
}

void OutputCheck(int flag) {
    // Yooo00000u Winnnnnn!!!
    char succeed[] = { 0x71, 0x47, 0x47, 0x47, 0x18, 0x18, 0x18, 0x18, 0x18, 0x5d, 0x8, 0x7f, 0x41, 0x46, 0x46, 0x46, 0x46, 0x46, 0x46, 0x9, 0x9, 0x9, 0x00};
    // OoooooOO00h Nooo00OO!!!
    char error[] = { 0x6e, 0x4e, 0x4e, 0x4e, 0x4e, 0x4e, 0x6e, 0x6e, 0x11, 0x11, 0x49, 0x1, 0x6f, 0x4e, 0x4e, 0x4e, 0x11, 0x11, 0x6e, 0x6e, 0x02, 0x02, 0x02, 0x00};
    
    if (flag == 1) {
        for (int i = 0; i < (sizeof(succeed) / sizeof(succeed[0])) - 1; i++) {
            succeed[i] ^= 0x28;
        }
        MessageBoxA(NULL, succeed, "Message", NULL);
    }
    else {
        for (int i = 0; i < (sizeof(error) / sizeof(error[0])) - 1; i++) {
            error[i] ^= 0x21;
        }
        MessageBoxA(NULL, error, "Message", NULL);
    }
}

void CheckYourFlag() {
    // 读取flag文件
    HANDLE hFile = CreateFile(TEXT(FLAG_PATH), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    if (hFile == NULL) {
        MessageBox(NULL, L"Can't find you file", L"Error", NULL);
        return;
    }

    const DWORD BUFFER_SIZE = 1024;
    char buffer[BUFFER_SIZE + 1];
    DWORD bytesRead;
    int Length = 0;

    while (ReadFile(hFile, buffer, BUFFER_SIZE, &bytesRead, NULL) && bytesRead > 0) {
        buffer[bytesRead] = '\0';
        Length = bytesRead;
        //OutputDebugStringA(buffer);
    }
   

   /* AllocConsole();
    FILE* fp;
    freopen_s(&fp, "CONOUT$", "w", stdout);

    printf("the length of string is : %d\n", Length);*/

    uint32_t* data = (uint32_t*)calloc(8, sizeof(uint32_t));
    memcpy(data, buffer, Length);

    uint32_t const key[] = { 0x11111111, 0x22222222, 0x33333333, 0x44444444 };
    // printf("plain text is : %s\n", buffer);

    btea(data, 8, key);
    // printf("加密后十六进制: \n");

    //for (int i = 0; i < 8; i++) {
    //    printf("%08x ", data[i]);
    //}
    //printf("\n");

    uint32_t cipher[] = { 0xa050a8b8, 0xa9ce2ed5, 0x5eccd823, 0x3dfdd731, 0x1e1d219f, 0x661f3884, 0x72bb3281, 0x985a7480 };
    if (memcmp(data, cipher, 32) == 0) {
        OutputCheck(1);
    }
    else {
        OutputCheck(0);
    }

    CloseHandle(hFile);

    //system("pause");
    //FreeConsole();
}

void MoveImageRandomly(int wndWidth, int wndHeight) {
    imgPos.x = rand() % (wndWidth - IMG_WIDTH);
    imgPos.y = rand() % (wndHeight - IMG_HEIGHT);
    InvalidateRect(hMainWnd, NULL, TRUE); // 清除原来的图像
}

void LoadImage() {
    //填写图片路径
    emojiImage = new Image(TEXT(IMAGE_PATH));
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void InitGDIPlus(ULONG_PTR* gdiplusToken) {
    GdiplusStartupInput gdiplusStartupInput;
    /* 
    extern "C" Status WINAPI GdiplusStartup(
        OUT ULONG_PTR *token,
        const GdiplusStartupInput *input,
        OUTGdiplusStartupOutput *output    
    */
    GdiplusStartup(gdiplusToken, &gdiplusStartupInput, nullptr);
}

void DrawImage(HDC hdc) {
    Graphics graphics(hdc); // 创建一个画笔对象
    graphics.Clear(Color(255, 255, 255)); // 背景清除为白色
    if (emojiImage)
        graphics.DrawImage(emojiImage, imgPos.x, imgPos.y, IMG_WIDTH, IMG_HEIGHT);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    srand((unsigned)time(NULL));
    hInst = hInstance;
    ULONG_PTR gdiplusToken;
    InitGDIPlus(&gdiplusToken); // 启动绘图
    LoadImage();

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Let's Catch T0FV";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 确保背景自动刷新
    RegisterClass(&wc);
    
    /**************************************************************
    HWND CreateWindowEx(
        DWORD     dwExStyle,    // 
        LPCTSTR   lpClassName,  // 窗口类名（模板）
        LPCTSTR   lpWindowName, // 窗口标题
        DWORD     dwStyle,      // 普通样式
        int       x,            // X 位置
        int       y,            // Y 位置
        int       nWidth,       // 宽度
        int       nHeight,      // 高度
        HWND      hWndParent,   // 父窗口
        HMENU     hMenu,        // 菜单句柄/控件ID
        HINSTANCE hInstance,    // 程序实例句柄
        LPVOID    lpParam       // 传递给窗口的参数
        );
    ***************************************************************/
    hMainWnd = CreateWindowEx(WS_EX_LAYERED, TEXT("Let's Catch T0FV"), TEXT("Can You?"), WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600, nullptr, nullptr, hInstance, nullptr);
    SetLayeredWindowAttributes(hMainWnd, 0, 242, LWA_ALPHA); // 设置窗口的透明度
    ShowWindow(hMainWnd, nCmdShow);
    UpdateWindow(hMainWnd);
    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    delete emojiImage;
    GdiplusShutdown(gdiplusToken);
    return (int)msg.wParam;
}

bool IsMouseOverImage(int mouseX, int mouseY) {
    return (mouseX >= imgPos.x && mouseX <= imgPos.x + IMG_WIDTH &&
        mouseY >= imgPos.y && mouseY <= imgPos.y + IMG_HEIGHT);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_MOUSEMOVE: 
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (IsMouseOverImage(x, y)) {
            RECT rect;
            GetClientRect(hwnd, &rect); // 获得用户窗口范围
            MoveImageRandomly(rect.right, rect.bottom);
        }
        break;
    }
    case WM_LBUTTONDOWN: 
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (IsMouseOverImage(x, y)) {
            MessageBox(hwnd, L"Somthing Wrong ,What happened?", L"OHHHHH", MB_OK);
        }
        break;
    }
    case WM_RBUTTONDOWN:
    {
        int x = LOWORD(lParam);
        int y = HIWORD(lParam);
        if (IsMouseOverImage(x, y)) {
            CheckYourFlag();
        }
    }
    case WM_PAINT: 
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        DrawImage(hdc); // 重绘图像
        EndPaint(hwnd, &ps);
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}
