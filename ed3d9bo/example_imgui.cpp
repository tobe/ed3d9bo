// Include the files
#include "overlay.h"
#include "draw.h"
// Include ImGui
#include "imgui\imgui.h"
#include "imgui\imgui_internal.h"
#include "imgui\imgui_impl_dx9.h"

// Check out ImGui docs for more on this one
extern LRESULT ImGui_ImplDX9_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// Extend the class itself
class MyOverlay : public ed3d9bo::Overlay{
public:
    // Call the default constructor with the same parameters
    MyOverlay(HINSTANCE hInstance, LPCTSTR lpClassName, LPCTSTR lpWindowName,
        LPCTSTR lpszOwnWindowName, LRESULT(CALLBACK *MyWndProc)(HWND, UINT, WPARAM, LPARAM))
        : Overlay(hInstance, lpClassName, lpWindowName, lpszOwnWindowName, MyWndProc) {};

    // By overloading DXInit, we can specify our ImGui initialization
    bool MyOverlay::DXInit() {
        // First, call the parent method
        Overlay::DXInit();

        // Now, initialize ImGui itself
        ImGui_ImplDX9_Init(this->m_hwOwn, this->m_pDevice);
        ImGuiIO &io = ImGui::GetIO();
        io.Fonts->AddFontFromFileTTF("C:\\Windows\\Fonts\\Tahoma.ttf", 16.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
        io.ImeWindowHandle = this->m_hwOwn;

        return 0;
    }

    bool MyOverlay::PreRender() {
        // We need to call this before each render hit
        ImGui_ImplDX9_NewFrame();

        if(Overlay::PreRender())
            return true;

        return false;
    }
};

// Define the pointers to the objects
MyOverlay *g_pOverlay;
ed3d9bo::Draw *g_pDraw;

// And the callback rendering function. This is where we will render ImGui.
void My_Render();

/*
Define a custom WndProc callback function, to parse some messages
Keep in mind the Overlay class itself has a WndProc which parses some *core* messages (such as WM_PAINT)
However, since we are using ImGui, this is going to be handled by ImGui itself anyway
*/
LRESULT CALLBACK MyWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    if(ImGui_ImplDX9_WndProcHandler(hWnd, Message, wParam, lParam))
        return true;

    return false;
}

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR  lpCmdLine, int nCmdShow) {
    // Debug console...
    AllocConsole();
    freopen("CONOUT$", "w", stdout);

    try {
        // Initialize the Overlay
        g_pOverlay = new MyOverlay(hInstance, "Notepad", "Untitled - Notepad", "Testing", MyWndProc);
        g_pOverlay->WindowInit();
        g_pOverlay->DXInit();

        // Render stuff -- this is the while(1) loop
        g_pOverlay->Loop(My_Render);
    } catch(const std::exception &e) {
        printf("Exception caught: %s", e.what());
        getchar();
    }
}

// Now we can initialize ImGui here
void My_Render() {
    /*
    Since we make the whole overlay transparent, there is no way we can use pure 0x00000000 as a background.
    Why? Because, it will get transparent as well! There is a DirectX11 workaround around this, and numerous issues have been
    posted. However, this is not an ImGui problem and therefore we need a workaround for DirectX9. By setting the background
    1 shade away from pure black, we achieve essentially the same thing, a solid background.
    */
    ImGui::GetStyle().Colors[ImGuiCol_WindowBg] = ImVec4(0.1f, 0.1f, 0.1f, 1.0f);
    ImGui::Text("Hello, world");
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
    ImGui::Render();
}