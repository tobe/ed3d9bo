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

        bool WindowInit(); // Initializes the window
        bool DXInit(); // Initializes DX.
        void Loop(std::function<void(void)>); // Loops!
        LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM) = nullptr;

        IDirect3DDevice9Ex *GetDevice();
        IDirect3D9Ex       *GetObject();
        ID3DXFont          *GetFont();
        DWORD              GetFPS();

        bool SetOverlayDimensions(int, int); // Sets overlay dimension
        void SetSleepTime(DWORD); // Sets the time between two renders (sleep time)
    protected:
        IDirect3DDevice9Ex    *m_pDevice;
        IDirect3D9Ex          *m_pD3D9;
        D3DPRESENT_PARAMETERS  m_D3D9_P;
        ID3DXFont             *m_pFont;

        HWND      m_hwTarget;  // Target window handle
        HWND      m_hwOwn;     // Own window handle
        HINSTANCE m_hInstance; // Application instance
        MSG       Message;     // For events

        MARGINS m_Margins;            // Window Margins
        RECT    m_RectTarget;         // Target RECT
        RECT    m_OwnRect;            // Own RECT
        LPCTSTR m_lpTargetClassName;  // Target window class
        LPCTSTR m_lpTargetWindowName; // Target window name (window title, actually)
        LPCTSTR m_lpszOwnWindowName;  // Own window name (title)

        int   m_iWidth = NULL;   // Preferred overlay width
        int   m_iHeight = NULL;  // And height
        DWORD m_dwSleep = 17;  // How much to sleep between renders? (1/60)*1000 = 16.6666666667ms for 60FPS.

        // FPS measurement
        DWORD dwFrameCount  = 0;
        DWORD dwTimeNow     = 0;
        DWORD dwLastUpdated = 0;
        DWORD dwTimePassed  = 0;
        DWORD dwFPS         = 0;

        bool FixPositioning();  // Fixes positioning
        bool PreRender();       // Stuff to do BEFORE drawing
        bool PostRender();      // Stuff to do AFTER drawing
    };
}