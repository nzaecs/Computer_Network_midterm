#include <iostream>
#include <winsock2.h>
#include <client.h>
#include <ws2tcpip.h>
#include <string>
#pragma comment(lib, "ws2_32.lib") 

bool initializeWinsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "Error initializing Winsock: " << WSAGetLastError() << "\n";
        return false;
    }
    return true;
}

SOCKET createSocket() {
    SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        std::cerr << "Error creating socket: " << WSAGetLastError() << "\n";
    }
    return sock;
}

bool connectToServer(SOCKET sock, const char* ipAddress, int port) {
    sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);

    if (inet_pton(AF_INET, ipAddress, &server_address.sin_addr) <= 0) {
        std::cerr << "Error transforming IP address\n";
        return false;
    }

    if (connect(sock, (sockaddr*)&server_address, sizeof(server_address)) == SOCKET_ERROR) {
        std::cerr << "Error connecting to server: " << WSAGetLastError() << "\n";
        return false;
    }
    return true;
}

bool sendData(SOCKET sock, const char* data) {
    if (send(sock, data, static_cast<int>(strlen(data)), 0) == SOCKET_ERROR) {
        std::cerr << "Error sending data: " << WSAGetLastError() << "\n";
        return false;
    }
    return true;
}



std::string receiveData(SOCKET sock) {
    char buffer[1024];
    memset(buffer, 0, sizeof(buffer));

    int bytesReceived = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (bytesReceived == SOCKET_ERROR) {
        std::cerr << "Error receiving data: " << WSAGetLastError() << "\n";
        return "";
    }

    return std::string(buffer, bytesReceived);
}

void cleanupWinsock(SOCKET sock) {
    closesocket(sock);
    WSACleanup();
}

void runNetworkExample() {
    if (!initializeWinsock()) return;

    SOCKET sock = createSocket();
    if (sock == INVALID_SOCKET) return;

    if (!connectToServer(sock, "192.168.111.119", 5150)) {
        cleanupWinsock(sock);
        return;
    }

    std::cout << "Connected to server. Type messages below (type 'exit' to quit):\n";

    while (true) {
        // Get user input
        std::string userInput;
        std::getline(std::cin, userInput);

        // Check for exit condition
        if (userInput == "exit") {
            break;
        }

        // Send data to server
        if (!sendData(sock, userInput.c_str())) {
            break;
        }

        // Receive and display server response
        std::string serverResponse = receiveData(sock);
        if (!serverResponse.empty()) {
            std::cout << "Server responded: " << serverResponse << "\n";
        }
    }

    cleanupWinsock(sock);
}

