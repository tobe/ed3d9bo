#pragma once

#include <Windows.h>
#include <d3dx9.h>
#include <d3d9.h>
#include <iostream>
#include <functional>
#include <dwmapi.h>
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "dwmapi.lib")

namespace ed3d9bo {
    class Overlay {
    public:
        Overlay(HINSTANCE, LPCTSTR, LPCTSTR, LPCTSTR, LRESULT(CALLBACK *)(HWND, UINT, WPARAM, LPARAM));
        ~Overlay();

        bool Window_Init(); // Initializes the window
        bool DX_Init(); // Initializes DX.
        bool SetOverlayDimensions(int, int); // Sets overlay dimensions
        void Loop(std::function<void(void)>); // Loops!
        int DrawText(char* String, int x, int y, int r, int g, int b);

        IDirect3DDevice9Ex    *m_pDevice; // TODO: move down, make getter
        LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM) = nullptr;

        // TODO: Move these in a thing yeah ok
        DWORD dwFrames = 0;
        DWORD dwCurrentTime = 0;
        DWORD dwLastUpdateTime = 0;
        DWORD dwElapsedTime = 0;
        char szFPS[32] = {0x00};

    protected:
        // WndProc callback
        //std::function<LRESULT(HWND, UINT, WPARAM, LPARAM)>WndProc = nullptr;

        IDirect3D9Ex          *m_pD3D9; // TODO: make getter
        D3DPRESENT_PARAMETERS  m_D3D9_P; //
        ID3DXFont             *m_pFont;// TODO: Make getter

        HWND m_hwTarget;       // Target window handle
        HWND m_hwOwn;          // Own window handle
        HINSTANCE m_hInstance; // Application instance
        MSG Message;           // For events

        MARGINS m_Margins;            // Window Margins
        RECT    m_RectTarget;         // Target RECT
        RECT    m_OwnRect;            // Own RECT
        LPCTSTR m_lpTargetClassName;  // Target window class
        LPCTSTR m_lpTargetWindowName; // Target window name (window title, actually)
        LPCTSTR m_lpszOwnWindowName;  // Own window name (title)

                                      // Preferred overlay width and height
        int m_iWidth = NULL;
        int m_iHeight = NULL;
        // TODO: Add a setter for this
        int m_dwSleep = 17; // How much to sleep between renders? (1/60)*1000 = 16.6666666667ms for 60FPS.

        bool FixPositioning();
        bool Pre_Render(); // Stuff to do BEFORE drawing
        bool Post_Render(); // Stuff to do AFTER drawing
    };
}