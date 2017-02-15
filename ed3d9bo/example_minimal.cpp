// Include the files
#include "overlay.h"
#include "draw.h"

// Define the pointers to the objects
ed3d9bo::Overlay *g_pOverlay;
ed3d9bo::Draw *g_pDraw;

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR  lpCmdLine, int nCmdShow) {
    try {
        // Initialize the Overlay -- pass a nullptr if you don't have a custom WndProc.
        g_pOverlay = new ed3d9bo::Overlay(hInstance, "Notepad", "Untitled - Notepad", "Testing", nullptr);
        g_pOverlay->WindowInit();
        g_pOverlay->DXInit();

        g_pDraw = new ed3d9bo::Draw(g_pOverlay->GetFont(), g_pOverlay->GetDevice());

        // Render stuff -- this is the while(1) loop
        g_pOverlay->Loop([]() {
            g_pDraw->DrawTextA("Hello world!", 30, 50, 255, 0, 0);
        });
    } catch(const std::exception &e) {
        exit(-1);
    }
}