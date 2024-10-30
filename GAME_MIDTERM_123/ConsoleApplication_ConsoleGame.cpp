#include "MyUtil.h"
#include <cwchar>
#define _USE_MATH_DEFINES
#include <cmath>
#include <complex>
#include <winsock2.h>
#include <WinUser.h>
#include <strsafe.h>
#include "KInput.h"
#include <vector>
#include <iostream>
#include "client.h"
#include <conio.h>

#pragma comment(lib, "ws2_32.lib")

double g_drawScale = 1.0;

enum Mode { GAME_MODE, CHAT_MODE };
Mode currentMode = GAME_MODE;


class KVector2
{
public:
    double x;
    double y;
};

KVector2 g_characterPos{ 10,10 };

void DrawLine(double x, double y, double x2, double y2, char ch)
{
    KVector2 center{ g_width / 2.0, g_height / 2.0 };
    ScanLine(int(x * g_drawScale + center.x), int(-y * g_drawScale + center.y)
        , int(x2 * g_drawScale + center.x), int(-y2 * g_drawScale + center.y), ch);
}

void Update(double elapsedTime)
{
    g_drawScale = 1.0;
    DrawLine(-g_width / 2, 0, g_width / 2, 0, '.');
    DrawLine(0, -g_height / 2, 0, g_height / 2, '.');

    PutTextf(0, 0, "%g", elapsedTime);
    //
    // game object update logic here
    //
    if (Input.GetKeyDown(VK_LEFT))
        g_characterPos.x -= 1;
    if (Input.GetKeyDown(VK_RIGHT))
        g_characterPos.x += 1;
    if (Input.GetKeyDown(VK_UP))
        g_characterPos.y -= 1;
    if (Input.GetKeyDown(VK_DOWN))
        g_characterPos.y += 1;
}

void DrawGameWorld() {
    //
    // game object drawing routine here
    //
    float h = Input.GetAxis("Horizontal");
    float v = Input.GetAxis("Vertical");
    PutTextf(1, 1, "Simultaneous Key Processing:");
    PutTextf(1, 2, "h = %g", h);
    PutTextf(1, 3, "v = %g", v);
    PutText(g_characterPos.x, g_characterPos.y, "P");
    DrawBuffer();
}

int main(void)
{
    if (!initializeWinsock()) return -1;

    std::string ipAddress;
    std::cout << "Enter the server IP address: ";
    std::getline(std::cin, ipAddress);

    SOCKET sock = createSocket();
    if (sock == INVALID_SOCKET) return -1;

    if (!connectToServer(sock, ipAddress.c_str(), 5150)) {
        cleanupWinsock(sock);
        return -1;
    }

    g_hwndConsole = GetConsoleWindow();
    g_hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    ShowCursor(false);

    bool isGameLoop = true;
    clock_t prevClock = clock();
    clock_t currClock = prevClock;
    Mode currentMode = GAME_MODE;
    std::string chatMessage;

    while (isGameLoop)
    {
        if (_kbhit()) {
            int key = _getch();

            if (key == VK_ESCAPE) {
                isGameLoop = false;
                break; 
            }

            if (key == VK_RETURN) {
                if (currentMode == GAME_MODE) {
                    currentMode = CHAT_MODE;
                    ShowCursor(true);  

                    std::cout << "\r" << std::string(80, ' ') << "\r";
                    std::cout << "\n" << std::string(80, ' ') << "\r";

                    std::cout << "Enter message: ";
                    std::getline(std::cin, chatMessage);

                    sendData(sock, chatMessage.c_str());
                    chatMessage.clear();

                    std::string serverResponse = receiveData(sock);
                    std::cout << "\r" << std::string(80, ' ') << "\r"; 
                    if (!serverResponse.empty()) {
                        std::cout << "Server: " << serverResponse << "\n";
                    }

                    currentMode = GAME_MODE;  
                    ShowCursor(false);
                }
            }
        }


        if (currentMode == GAME_MODE) {
            prevClock = currClock;
            currClock = clock();
            const double elapsedTime = ((double)currClock - (double)prevClock) / CLOCKS_PER_SEC;
            ClearBuffer();
            Input.Update(elapsedTime);
            Update(elapsedTime);
            Sleep(10);
            DrawGameWorld();
        }
    }

    cleanupWinsock(sock);  
    return 0;
}