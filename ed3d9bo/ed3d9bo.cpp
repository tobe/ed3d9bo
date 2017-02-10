#include "ed3d9bo.h"

ed3d9bo::ed3d9bo(LPCTSTR lpClassName, LPCTSTR lpWindowName, LPCTSTR lpszOwnWindowName) {
    // Set these
    this->m_lpTargetClassName  = lpClassName;
    this->m_lpTargetWindowName = lpWindowName;
    this->m_lpszOwnWindowName  = lpszOwnWindowName;

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
    wndOverlay.lpszClassName    = lpszOwnWindowName;
    wndOverlay.lpszMenuName     = lpszOwnWindowName;
    wndOverlay.hIconSm          = LoadIcon(wndOverlay.hInstance, IDI_APPLICATION);

    if(!RegisterClassEx(&wndOverlay))
        throw std::runtime_error("Failed to register the overlay.");

    // Find the target window handle.
    this->m_hwTarget = FindWindow(lpClassName, lpWindowName);
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
    this->m_hwOwn = CreateWindowEx(WS_EX_LAYERED | WS_EX_NOACTIVATE, lpszOwnWindowName, lpszOwnWindowName, 
                                   WS_POPUP, 100, 100, this->m_iWidth, this->m_iHeight, 0, 0, this->m_hInstance, 0);
    if(!this->m_hwOwn)
        throw std::runtime_error("Failed to CreateWindowEx.");

    // Set alpha values and the color key now.
    SetLayeredWindowAttributes(this->m_hwOwn, 0, 0, LWA_ALPHA);
    SetLayeredWindowAttributes(this->m_hwOwn, 0, RGB(0, 0, 0), LWA_COLORKEY);

    // Display the window & update it with the newly set attributes
    ShowWindow(this->m_hwOwn, SW_SHOW);
    UpdateWindow(this->m_hwOwn);

    // Extend the window frame into the client area
    DwmExtendFrameIntoClientArea(this->m_hwOwn, &this->m_Margins);

    // Success.
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
    D3DXCreateFont(this->m_pDevice, 48, 0, FW_BOLD, 1, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                   DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, L"Arial", &this->m_Font);
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

ed3d9bo::~ed3d9bo() {}