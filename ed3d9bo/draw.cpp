#include "Draw.h"

Draw::Draw(ID3DXFont *m_pFont, IDirect3DDevice9Ex *m_pDevice) {
    this->m_pFont = m_pFont;
    this->m_pDevice = m_pDevice;
}

void Draw::DrawTextA(LPCSTR lpszWhat, int x, int y, int r, int g, int b) {
    RECT rectFont;
    rectFont.bottom = 0;
    rectFont.left = x;
    rectFont.top = y;
    rectFont.right = 0;

    this->m_pFont->DrawTextA(0, lpszWhat, strlen(lpszWhat), &rectFont, DT_NOCLIP, D3DCOLOR_ARGB(255, r, g, b));
}