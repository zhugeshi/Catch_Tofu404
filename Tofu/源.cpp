#include <windows.h>
#include <gdiplus.h>
#include <ctime>
#include <cstdlib>
#include <stdint.h>
#include <stdio.h>
#pragma comment(lib, "gdiplus.lib")

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
    else if (n < -1)      /* Decoding Part */
    {
        n = -n;
        rounds = 6 + 52 / n;
        sum = rounds * DELTA;
        y = v[0];
        do
        {
            e = (sum >> 2) & 3;
            for (p = n - 1; p > 0; p--)
            {
                z = v[p - 1];
                y = v[p] -= MX;
            }
            z = v[n - 1];
            y = v[0] -= MX;
            sum -= DELTA;
        } while (--rounds);
    }
}

void OutPutFlag() {
    _asm {
        jz label1
        jnz label1
        _emit 0xe9
        label1 :
    }
    uint32_t arr[8] = {
        0xba54140,0x3bd1a90c,0xb2ab71c,0x7dc898a7,0x46166664,0x761c15ab,0xb2cbfe43,0xb5e15396
    };
    // 解密密钥
    uint32_t const k[4] = { 2, 2, 3, 4 };
    // 解密
    btea(arr, -8, k);
    // 转换为字符串
    char flag[33] = { 0 };  // 32字节 + \0
    memcpy(flag, arr, 32);
    MessageBoxA(hMainWnd, flag, "Flag", MB_OK);
}

void MoveImageRandomly(int wndWidth, int wndHeight) {
    imgPos.x = rand() % (wndWidth - IMG_WIDTH);
    imgPos.y = rand() % (wndHeight - IMG_HEIGHT);
    InvalidateRect(hMainWnd, NULL, TRUE); // 清除原来的图像
}

void LoadImage() {
    //填写图片路径
    emojiImage = new Image(L"911705D976A72DB2A78C70FC24C48F94.jpg");
}

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

void InitGDIPlus(ULONG_PTR* gdiplusToken) {
    GdiplusStartupInput gdiplusStartupInput;
    GdiplusStartup(gdiplusToken, &gdiplusStartupInput, nullptr);
}

void DrawImage(HDC hdc) {
    Graphics graphics(hdc);
    graphics.Clear(Color(255, 255, 255)); // 背景清除为白色
    if (emojiImage)
        graphics.DrawImage(emojiImage, imgPos.x, imgPos.y, IMG_WIDTH, IMG_HEIGHT);
}

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int nCmdShow) {
    srand((unsigned)time(NULL));
    hInst = hInstance;
    ULONG_PTR gdiplusToken;
    InitGDIPlus(&gdiplusToken);
    LoadImage();

    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Let's Catch T0FV";
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1); // 确保背景自动刷新
    RegisterClass(&wc);

    hMainWnd = CreateWindowEx(0, L"Let's Catch T0FV", L"XiXiXi", WS_OVERLAPPEDWINDOW,
        100, 100, 800, 600, nullptr, nullptr, hInstance, nullptr);
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
            GetClientRect(hwnd, &rect);
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
            OutPutFlag();
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
