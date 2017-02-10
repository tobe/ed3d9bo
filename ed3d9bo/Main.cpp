#include "overlay.h"
#include "Draw.h"

ed3d9bo::Overlay *test;
Draw *draw;
void My_Render();

LRESULT CALLBACK Proc(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam) {
    switch(Message) {
        case WM_LBUTTONDOWN:
            printf("left button down!");
        break;
    }

    return DefWindowProc(hWnd, Message, wParam, lParam);
}

INT APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR  lpCmdLine, int nCmdShow) {
    // Debug console...
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
    
    // Here's the class!
    try {
        test = new ed3d9bo::Overlay(hInstance, "Notepad", "Untitled - Notepad", "Testing", Proc);
        test->Window_Init();
        test->DX_Init();
        test->Loop(My_Render);
    }catch(const std::exception &e) {
        printf("fk: %s", e.what());
    }
}

void My_Render() {
    

    // Heya
    //test->DrawTextA("Hello #sector5d :)", 0, 0, 255, 0, 0);

    
}