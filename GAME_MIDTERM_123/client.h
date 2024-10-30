#pragma once

#ifndef NETWORK_H
#define NETWORK_H

#include <string>
#include <winsock2.h>

bool initializeWinsock();
SOCKET createSocket();
bool connectToServer(SOCKET sock, const char* ipAddress, int port);
bool sendData(SOCKET sock, const char* data);
std::string receiveData(SOCKET sock);
void cleanupWinsock(SOCKET sock);
void runNetworkExample();

#endif