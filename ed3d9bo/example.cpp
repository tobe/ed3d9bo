// Include the files
#include "overlay.h"
#include "draw.h"

// Define the pointers to the objects
ed3d9bo::Overlay *g_pOverlay;
ed3d9bo::Draw *g_pDraw;

// And the callback rendering function
void My_Render();

/*
Define a custom WndProc callback function, to parse some messages
Keep in mind the class itself has a WndProc which parses messages!
*/

LRESULT CALLBACK MyWndProc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        // E.g: print this out in the debug console every time the left mouse button has been pressed over the overlay.
        case WM_LBUTTONDOWN:
            printf("Left button down!");
        break;
    }

    // Remember to return false
    return 0;
}

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR  lpCmdLine, int nCmdShow) {
    // Debug console...
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    
    try {
        // Initialize the Overlay
        g_pOverlay = new ed3d9bo::Overlay(hInstance, "Notepad", "Untitled - Notepad", "Testing", MyWndProc);
        g_pOverlay->WindowInit();
        g_pOverlay->DXInit();

        /*
        After the initialization may we only initialize the Draw!
        This is because we need the Device and Font objects which are created when WindowInit() and DXInit() have been called.
        */
        g_pDraw = new ed3d9bo::Draw(g_pOverlay->GetFont(), g_pOverlay->GetDevice());

        // Render stuff -- this is the while(1) loop
        g_pOverlay->Loop(My_Render);
    }catch(const std::exception &e) {
        // Catch exceptions
        printf("Exception caught: %s", e.what());
        getchar();
    }
}

// This is where stuff to be rendered is specified
void My_Render() {
    g_pDraw->DrawTextA("Hello world!", 30, 50, 255, 0, 0);
}