#include "ed3d9bo.h"

ed3d9bo::ed3d9bo(HINSTANCE hInstance, LPCTSTR lpClassName, LPCTSTR lpWindowName, LPCTSTR lpszOwnWindowName, LRESULT(CALLBACK *WndProc)(HWND, UINT, WPARAM, LPARAM)) {
    // Instantiate these.
    this->m_hInstance           = hInstance;
    this->m_lpTargetClassName   = lpClassName;
    this->m_lpTargetWindowName  = lpWindowName;
    this->m_lpszOwnWindowName   = lpszOwnWindowName;
    this->WndProc               = WndProc;
}

bool ed3d9bo::Window_Init() {
    // Initialize the window
    WNDCLASSEX wndOverlay;
    ZeroMemory(&wndOverlay, sizeof(wndOverlay));
    wndOverlay.cbSize           = sizeof(WNDCLASSEX);
    wndOverlay.style            = CS_HREDRAW | CS_VREDRAW; // Redraw both vertically and horizontally if a a movement or size adjustment changes the width of the client area.
    wndOverlay.lpfnWndProc      = this->WndProc; // User-specified WndProc
    wndOverlay.cbClsExtra       = 0;
    wndOverlay.cbWndExtra       = 0;
    wndOverlay.hInstance        = this->m_hInstance;
    wndOverlay.hCursor          = LoadCursor(0, IDC_ARROW);
    wndOverlay.hIcon            = LoadIcon(0, IDI_APPLICATION);
    wndOverlay.hIconSm          = LoadIcon(0, IDI_APPLICATION);
    wndOverlay.hbrBackground    = (HBRUSH)CreateSolidBrush(RGB(0, 0, 0)); // Try 255,255,255 here?
    wndOverlay.lpszClassName    = this->m_lpszOwnWindowName;
    wndOverlay.lpszMenuName     = this->m_lpszOwnWindowName;

    if(!RegisterClassEx(&wndOverlay))
        throw std::runtime_error("Failed to register the overlay.");

    // Find the target window handle.
    this->m_hwTarget = FindWindow(m_lpTargetClassName, m_lpTargetWindowName);
    if(!this->m_hwTarget)
        throw std::runtime_error("Failed to find the target window.");

    // Check to see if the overlay dimensions are set, if not then assume target window width & height.
    if(!this->m_iHeight || !this->m_iWidth) {
        // Try to find the target dimensions
        if(!GetWindowRect(this->m_hwTarget, &this->m_RectTarget))
            throw std::runtime_error("Failed to grab the target window dimensions.");

        this->m_iWidth  = this->m_RectTarget.right  - this->m_RectTarget.left;
        this->m_iHeight = this->m_RectTarget.bottom - this->m_RectTarget.top;
    }

    // Create the window
    // TODO: fix this
    this->m_hwOwn = CreateWindowEx(WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_NOACTIVATE, this->m_lpszOwnWindowName, this->m_lpszOwnWindowName,
                                   WS_POPUP, 100, 100, this->m_iWidth, this->m_iHeight, 0, 0, this->m_hInstance, 0);
    if(!this->m_hwOwn)
        throw std::runtime_error("Failed to CreateWindowEx.");

    // Set alpha values and the color key now.
    SetLayeredWindowAttributes(this->m_hwOwn, 0, 0, LWA_ALPHA);
    SetLayeredWindowAttributes(this->m_hwOwn, 0, RGB(0, 0, 0), LWA_COLORKEY);

    // Extend the window frame into the client area (AERO Fix)
    DwmExtendFrameIntoClientArea(this->m_hwOwn, &this->m_Margins);

    // Display the window & update it with the newly set attributes
    ShowWindow(this->m_hwOwn, SW_SHOW);
    UpdateWindow(this->m_hwOwn);

    // Success.
    return 0;
}

// Overload this for ImGui
bool ed3d9bo::DX_Init() {
    auto hCreateMe = Direct3DCreate9Ex(D3D_SDK_VERSION, &this->m_pD3D9);
    if(!SUCCEEDED(hCreateMe)) { // Uh...
        UnregisterClass(this->m_lpszOwnWindowName, this->m_hInstance);
        return false;
    }

    ZeroMemory(&this->m_D3D9_P, sizeof(this->m_D3D9_P));
    m_D3D9_P.BackBufferWidth        = this->m_iWidth; // Set tie width
    m_D3D9_P.BackBufferHeight       = this->m_iHeight; // And the height
    m_D3D9_P.BackBufferFormat       = D3DFMT_X8R8G8B8; // 32-bit
    m_D3D9_P.MultiSampleQuality     = D3DMULTISAMPLE_NONE; // No multisampling
    m_D3D9_P.SwapEffect             = D3DSWAPEFFECT_DISCARD; // Discard old frames
    m_D3D9_P.hDeviceWindow          = this->m_hwOwn; // Register the window handle
    m_D3D9_P.Windowed               = TRUE; // Target must be windowed.
    m_D3D9_P.EnableAutoDepthStencil = TRUE; // Automatically manage depth buffers
    m_D3D9_P.AutoDepthStencilFormat = D3DFMT_D16; // format of ^
    m_D3D9_P.FullScreen_RefreshRateInHz = 0; // Must be 0 for windowed.

    // Try to create the device
    if(!SUCCEEDED(this->m_pD3D9->CreateDeviceEx(
        D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, this->m_hwOwn, D3DCREATE_HARDWARE_VERTEXPROCESSING,
        &this->m_D3D9_P, NULL, &this->m_pDevice))) {
            this->m_pD3D9->Release();
            UnregisterClass(this->m_lpszOwnWindowName, this->m_hInstance);
            return false;
    }

    // Spawn a font.
    D3DXCreateFont(this->m_pDevice, 16, 0, FW_NORMAL, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, "Arial", &this->m_pFont);
}

void ed3d9bo::Loop(std::function<void(void)> Render) {
    while(1) {
        if(PeekMessage(&this->Message, this->m_hwOwn, 0, 0, PM_REMOVE)) {
            TranslateMessage(&this->Message);
            DispatchMessage(&this->Message);
        }

        // Fix Positioning (keep up!)
        if(!this->FixPositioning())
            exit(0);

        // Render time!
        if(this->Pre_Render()) {
            Render();
        }
        this->Post_Render();

        // Sleep
        Sleep(this->m_dwSleep);
    }
}

bool ed3d9bo::FixPositioning() {
    // Get the target window
    this->m_hwTarget = FindWindow(this->m_lpTargetClassName, this->m_lpTargetWindowName);
    if(!this->m_hwTarget) // No longer exists (in theory)
        return false;

    // Get the RECT of the target
    GetWindowRect(this->m_hwTarget, &this->m_RectTarget);
    // Move the overlay
    MoveWindow(this->m_hwOwn, 
        this->m_RectTarget.left, // X: 0
        this->m_RectTarget.top,  // Y: 0
        this->m_RectTarget.right - this->m_RectTarget.left, // Width
        this->m_RectTarget.bottom - this->m_RectTarget.top, // Height
        true); // Redraw after moving

    return true;
}

bool ed3d9bo::SetOverlayDimensions(int iWidth, int iHeight) {
    RECT rectDesktop;
    const HWND hDesktop = GetDesktopWindow();
    if(!hDesktop)
        return false;

    GetWindowRect(hDesktop, &rectDesktop);

    // Overlay cannot be bigger than the desktop resolution itself.
    if(iWidth > rectDesktop.right || iHeight > rectDesktop.bottom)
        return false;

    // Everything was good
    this->m_iWidth  = iWidth;
    this->m_iHeight = iHeight;

    return true;
}

bool ed3d9bo::Pre_Render() {
    // Clear old stuff
    m_pDevice->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.0f, 0);
    m_pDevice->BeginScene();

    // Is the target focused (in foreground?). If so, return true as in, continue rendering.
    if(this->m_hwTarget == GetForegroundWindow())
        return true;

    return false; // Othewise, there's no point in rendering if the application isn't even focused.
}

bool ed3d9bo::Post_Render() {
    /*this->m_pDevice->SetRenderState(D3DRS_ZENABLE, FALSE); // Turn off Z-buffering
    this->m_pDevice->SetRenderState(D3DRS_LIGHTING, FALSE); // Turn off 3D-lightning
    this->m_pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);*/

    dwFrames++;
    dwCurrentTime = GetTickCount(); // Even better to use timeGetTime()
    dwElapsedTime = dwCurrentTime - dwLastUpdateTime;

    if(dwElapsedTime >= 1000) {
        wsprintf(szFPS, "FPS = %u", (UINT)(dwFrames * 1000.0 / dwElapsedTime));
        dwFrames = 0;
        dwLastUpdateTime = dwCurrentTime;
    }

    this->DrawTextA(szFPS, 30, 100, 0, 255, 0);

    this->m_pDevice->EndScene();
    this->m_pDevice->PresentEx(0, 0, 0, 0, 0);

    return true;
}

ed3d9bo::~ed3d9bo() {}