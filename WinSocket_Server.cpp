#include <Winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <vector>
#include <sstream>
#include <algorithm>

#pragma comment(lib, "ws2_32.lib")

#define MAX_PACKET_SIZE 4096
#define _WINSOCK_DEPRECATED_NO_WARNINGS

bool isNumeric(const std::string& str) 
{
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

bool processNumbers(const std::string& buffer) 
{
    std::istringstream iss(buffer);
    std::string token;

    int maximumFromBuffer = 0;
    while (std::getline(iss, token, ' ')) 
    {
        if (isNumeric(token)) {
            int number = std::stoi(token);
            if (number > maximumFromBuffer)
            {
                maximumFromBuffer = number;
            }
        }
    }

    if (maximumFromBuffer != 0)
    {
        std::cout << "Maximum number from array: " << maximumFromBuffer << std::endl;
        return true;
    }
    return false;
}

void ClientHandler(SOCKET communicationSocket, WSAEVENT event) 
{
    char buffer[MAX_PACKET_SIZE];
    int bytesReceived;

    while (true) 
    {
        WaitForSingleObject(event, INFINITE);
        WSAResetEvent(event);

        ioctlsocket(communicationSocket, FIONREAD, (u_long*)&bytesReceived);
        if (bytesReceived > 0) 
        {
            bytesReceived = recv(communicationSocket, buffer, MAX_PACKET_SIZE, 0);

            if (bytesReceived == 0)
                break;

            buffer[bytesReceived] = '\0';
            std::cout << "---------------\nNew message from a client:\n-------------- - " << std::endl;
            std::cout << "Bytes received: " << bytesReceived << std::endl;
            std::cout << buffer << std::endl;

            bool isValidData = processNumbers(buffer);
            if (!isValidData)
            {
                std::string response = "Invalid data. Please send only numeric array";
                if (send(communicationSocket, response.c_str(), response.length(), 0) == SOCKET_ERROR) 
                {
                    std::cout << "Failed to send response. Error code: " << WSAGetLastError() << std::endl;
                    break;
                }
            }
            else 
            {
                std::string response = "Valid data. Maximum number are founded";
                if (send(communicationSocket, response.c_str(), response.length(), 0) == SOCKET_ERROR) 
                {
                    std::cout << "Failed to send response. Error code: " << WSAGetLastError() << std::endl;
                    break;
                }
            }
        }
    }

    closesocket(communicationSocket);
}

int main() 
{
    WSADATA ws;
    if (!WSAStartup(MAKEWORD(2, 2), &ws)) 
    {
        SOCKET TCPSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (TCPSocket != INVALID_SOCKET) 
        {
            sockaddr_in server;
            ZeroMemory(&server, sizeof(server));
            server.sin_family = AF_INET;
            server.sin_addr.S_un.S_addr = INADDR_ANY;
            server.sin_port = htons(2222);

            if (!bind(TCPSocket, (sockaddr*)&server, sizeof(server))) 
            {
                if (!listen(TCPSocket, SOMAXCONN)) 
                {
                    std::cout << "Waiting for connection" << std::endl;

                    while (true) 
                    {
                        sockaddr_in clientAddr;
                        int clientAddrSize = sizeof(clientAddr);

                        SOCKET communicationSocket = accept(TCPSocket, (sockaddr*)&clientAddr, &clientAddrSize);
                        if (communicationSocket != INVALID_SOCKET) 
                        {
                            char clientIP[INET_ADDRSTRLEN];
                            InetNtopA(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);

                            std::cout << "Connected successfully" << std::endl;
                            std::cout << "Client IP: " << clientIP << std::endl;
                            std::cout << "Client port: " << ntohs(clientAddr.sin_port) << std::endl;

                            WSAEVENT event = WSACreateEvent();
                            WSAEventSelect(communicationSocket, event, FD_READ | FD_CLOSE);

                            std::thread clientThread(ClientHandler, communicationSocket, event);
                            clientThread.detach();
                        }
                        else
                        {
                            std::cout << "Failed to accept connection. Error code: " << WSAGetLastError() << std::endl;
                        }
                    }
                }
                else
                {
                    std::cout << "Failed to set your socket to listen mode. Error code: " << WSAGetLastError() << std::endl;
                }
            }
            else
            {
                std::cout << "Failed to bind socket. Error code: " << WSAGetLastError() << std::endl;
            }

            closesocket(TCPSocket);
            WSACleanup();
        }
        else
        {
            std::cout << "Failed to create a TCP socket. Error code: " << WSAGetLastError() << std::endl;
        }
    }
    else
    {
        std::cout << "Failed to initialize WinSockAPI." << std::endl;
    }

    return 0;
}