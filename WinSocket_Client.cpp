#include <Winsock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <string>
#include <limits>

#pragma comment(lib, "ws2_32.lib")

#define MAX_PACKET_SIZE 4096

int main() 
{
    WSADATA ws;
    if (WSAStartup(MAKEWORD(2, 2), &ws) == 0) 
    {
        SOCKET TCPSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        if (TCPSocket != INVALID_SOCKET) {
            std::cout << "TCP socket is created" << std::endl;

            sockaddr_in client;
            ZeroMemory(&client, sizeof(client));
            client.sin_family = AF_INET;

            // Use inet_pton to convert IP address string to binary form
            if (inet_pton(AF_INET, "127.0.0.1", &(client.sin_addr)) != 1) 
            {
                std::cout << "Invalid address. Error code: " << WSAGetLastError() << std::endl;
                closesocket(TCPSocket);
                WSACleanup();
                return 1;
            }

            client.sin_port = htons(2222);
            std::cout << "Press any key to connect to the server" << std::endl;
            system("pause");

            if (connect(TCPSocket, reinterpret_cast<sockaddr*>(&client), sizeof(client)) == 0)
            {
                std::cout << "Connected successfully. Type your message or press 'q' to close the connection" << std::endl;

                while(true)
                {
                    std::cout << ">> ";
                    std::string userInput;
                    std::getline(std::cin, userInput);

                    if (userInput.empty()) {
                        std::cout << "Please enter a message." << std::endl;
                        continue;
                    }

                    const char* buffer = userInput.c_str();
                    if (!strcmp(buffer, "q")) {
                        closesocket(TCPSocket);
                        WSACleanup();
                        std::cout << "Connection closed." << std::endl;
                        return 0;
                    }

                    send(TCPSocket, buffer, userInput.length(), 0);

                    timeval tv; // timeout 
                    tv.tv_sec = 0;
                    tv.tv_usec = 100; // timeout in microseconds

                    fd_set readSet;
                    FD_ZERO(&readSet);
                    FD_SET(TCPSocket, &readSet);

                    int selectResult = select(0, &readSet, nullptr, nullptr, &tv);
                    if (selectResult == SOCKET_ERROR) 
                    {
                        std::cout << "Error in select. Error code: " << WSAGetLastError() << std::endl;
                        break;
                    }

                    if (selectResult > 0) 
                    {
                        char responseBuffer[MAX_PACKET_SIZE];
                        int bytesReceived = recv(TCPSocket, responseBuffer, MAX_PACKET_SIZE, 0);
                        if (bytesReceived > 0) {
                            responseBuffer[bytesReceived] = '\0';
                            std::cout << "Server response: " << responseBuffer << std::endl;
                        }
                    }
                    else
                    {
                        // if timeout has been exceeded
                    }
                }
            }
            else 
            {
                std::cout << "Failed to establish connection with the server. Error code: " << WSAGetLastError() << std::endl;
            }
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