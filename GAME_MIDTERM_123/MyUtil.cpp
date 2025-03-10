#include "MyUtil.h"
#include <strsafe.h>

const int g_width = 80;
const int g_height = 25;
HWND g_hwndConsole = 0;
HANDLE g_hConsole = 0;
char g_buffer[g_height][g_width + 1];

void GotoXY(int x, int y)
{
    _COORD pos{ (short)x,(short)y };
    SetConsoleCursorPosition(g_hConsole, pos);
}

void ShowCursor(bool visible)
{
    CONSOLE_CURSOR_INFO cursorInfo{ 1,visible };
    SetConsoleCursorInfo(g_hConsole, &cursorInfo);
}

void PutCh(int x, int y, char ch)
{
    if (x >= 0 && x < g_width && y >= 0 && y <= g_height)
        g_buffer[y][x] = ch;
}

void PutText(int x, int y, const char* text)
{
    const size_t len = strlen(text);
    if (x + len >= g_width) return;
    const char oldCh = g_buffer[y][x + len];
    sprintf_s(&g_buffer[y][x], __min(len + 1, g_width - x), text);
    g_buffer[y][x + len] = oldCh;
}

void PutTextf(int x, int y, const char* format_, ...)
{
    char    buffer[1024];
    va_list argList;

    va_start(argList, format_);
    StringCchVPrintfA(buffer, 1024, format_, argList);
    va_end(argList);
    PutText(x, y, buffer);
}//PutTextf()

void ClearBuffer()
{
    memset(g_buffer, ' ', sizeof(g_buffer));
    for (int row = 0; row < g_height; ++row) {
        g_buffer[row][g_width] = 0;
    }
}

void DrawBuffer()
{
    for (int row = 0; row < g_height; ++row) {
        GotoXY(0, row);
        printf(g_buffer[row]);
    }
}

int sign(float delta)
{
    if (delta > 0.f)
        return 1;
    else
        return -1;
}

void _ScanLineLow(int x0, int y0, int x1, int y1, char ch)
{
    float deltax = float(x1 - x0);
    float deltay = float(y1 - y0);
    float deltaerr = fabsf(deltay / deltax); // Assume deltax != 0 (line is not vertical),
    // note that this division needs to be done in a way that preserves the fractional part
    float error = 0.0f; // No error at start
    int y = y0;
    for (int x = x0; x <= x1; ++x) {
        PutCh(x, y, ch);
        error = error + deltaerr;
        if (error >= 0.5f) {
            y = y + sign(deltay);
            error = error - 1.0f;
        }
    }
}

void _ScanLineHigh(int x0, int y0, int x1, int y1, char ch)
{
    float deltax = float(x1 - x0);
    float deltay = float(y1 - y0);
    float deltaerr = fabsf(deltax / deltay); // Assume deltax != 0 (line is not vertical),
    // note that this division needs to be done in a way that preserves the fractional part
    float error = 0.0f; // No error at start
    int x = x0;
    for (int y = y0; y <= y1; ++y) {
        PutCh(x, y, ch);
        error = error + deltaerr;
        if (error >= 0.5f) {
            x = x + sign(deltax);
            error = error - 1.0f;
        }
    }
}

void ScanLine(int x0, int y0, int x1, int y1, char ch)
{
    if (abs(y1 - y0) < abs(x1 - x0)) {
        if (x0 > x1) {
            _ScanLineLow(x1, y1, x0, y0, ch);
        }
        else {
            _ScanLineLow(x0, y0, x1, y1, ch);
        }

    }
    else {
        if (y0 > y1) {
            _ScanLineHigh(x1, y1, x0, y0, ch);
        }
        else {
            _ScanLineHigh(x0, y0, x1, y1, ch);
        }
    }
}
