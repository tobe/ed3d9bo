#include <Windows.h>
#include <d3dx9.h>
#include <d3d9.h>

namespace ed3d9bo {
    class Draw {
    public:
        Draw(ID3DXFont *, IDirect3DDevice9Ex *);
        void DrawTextA(LPCSTR lpszWhat, int, int, int, int, int);

    private:
        ID3DXFont *m_pFont = nullptr;
        IDirect3DDevice9Ex *m_pDevice = nullptr;
    };
}