#include <windows.h>
#include <iostream>
#include <commdlg.h> // For file dialog

HBITMAP hBitmap = NULL;
BITMAP bitmap;

bool LoadBMP(const char* filename) {
    hBitmap = (HBITMAP)LoadImage(NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    if (!hBitmap) return false;
    GetObject(hBitmap, sizeof(BITMAP), &bitmap);
    return true;
}

std::string OpenFileDialog() {
    OPENFILENAME ofn = { 0 };
    char szFile[260] = { 0 };

    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "BMP Files\0*.BMP\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrTitle = "Select an Image";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (GetOpenFileName(&ofn)) return std::string(ofn.lpstrFile);
    return "";
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    static std::string filePath;
    switch (uMsg) {
        case WM_CREATE:
            filePath = OpenFileDialog();
            if (filePath.empty() || !LoadBMP(filePath.c_str())) {
                MessageBox(hwnd, "Failed to load image.", "Error", MB_OK | MB_ICONERROR);
                PostQuitMessage(0);
            }
            SetWindowPos(hwnd, NULL, 0, 0, bitmap.bmWidth, bitmap.bmHeight, SWP_NOMOVE | SWP_NOZORDER);
            break;
        case WM_PAINT: {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hwnd, &ps);
            HDC hdcMem = CreateCompatibleDC(hdc);
            SelectObject(hdcMem, hBitmap);
            BitBlt(hdc, 0, 0, bitmap.bmWidth, bitmap.bmHeight, hdcMem, 0, 0, SRCCOPY);
            DeleteDC(hdcMem);
            EndPaint(hwnd, &ps);
        } break;
        case WM_DESTROY:
            if (hBitmap) DeleteObject(hBitmap);
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int main() {
    const char* className = "ImageViewerClass";
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = className;

    if (!RegisterClass(&wc)) return 1;

    HWND hwnd = CreateWindowEx(0, className, "Image Viewer", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, wc.hInstance, NULL);
    if (!hwnd) return 1;

    ShowWindow(hwnd, SW_SHOW);
    UpdateWindow(hwnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
