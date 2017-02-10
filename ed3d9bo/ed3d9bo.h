#pragma once

#include <Windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <iostream>
#include <functional>
#include <dwmapi.h>

class ed3d9bo {
public:
    ed3d9bo(LPCTSTR, LPCTSTR, LPCTSTR);
    ~ed3d9bo();

    bool DX_Init(); // Initializes DX.

    bool SetOverlayDimensions(int, int);

private:
    // WndProc callback
    //std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>WndProc = nullptr;
    LRESULT (CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM) = nullptr;

    IDirect3DDevice9Ex    *m_pDevice;
    IDirect3D9Ex          *m_pD3D9;
    D3DPRESENT_PARAMETERS  m_D3D9_P;
    LPD3DXFONT             m_Font;

    HWND m_hwTarget;       // Target window handle
    HWND m_hwOwn;          // Own window handle
    HINSTANCE m_hInstance; // Application instance

    MARGINS m_Margins;     // Window Margins
    RECT    m_RectTarget;  // Target RECT
    RECT    m_OwnRect;     // Own RECT
    LPCTSTR m_lpTargetClassName; // Target window class
    LPCTSTR m_lpTargetWindowName; // Target window name (window title, actually)
    LPCTSTR m_lpszOwnWindowName; // Own window name (title)

    // Preferred overlay width and height
    int m_iWidth  = NULL;
    int m_iHeight = NULL;
};