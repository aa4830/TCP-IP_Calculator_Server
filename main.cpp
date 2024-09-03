#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <cstring>
#pragma comment(lib, "ws2_32")
using namespace std;

int main()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData));

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in ServerSocketAddress;
    memset(&ServerSocketAddress, 0, sizeof(ServerSocketAddress));
    ServerSocketAddress.sin_family = AF_INET;
    ServerSocketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    ServerSocketAddress.sin_port = htons(10880);
    bind(ListenSocket, (struct sockaddr*)&ServerSocketAddress, sizeof(ServerSocketAddress));
 
    listen(ListenSocket, 5);

    fd_set ReadSockets;
    FD_ZERO(&ReadSockets);
    FD_SET(ListenSocket, &ReadSockets);

    fd_set CopyReadSockets; 

    struct timeval TimeOut;
    TimeOut.tv_sec = 0;
    TimeOut.tv_usec = 10;

    while (true)
    {
        CopyReadSockets = ReadSockets;
        int ReadSocketsCount = select(0, &CopyReadSockets, 0, 0, &TimeOut);
        if (ReadSocketsCount == 0)
        {
            continue;
        }
        else if (ReadSocketsCount < 0)
        {
            break;
        }
        else
        {
            for (int i = 0; i < (int)ReadSockets.fd_count; ++i)
            {
                if (FD_ISSET(ReadSockets.fd_array[i], &CopyReadSockets))
                {
                    if (ReadSockets.fd_array[i] == ListenSocket)
                    {
                        sockaddr_in ClientSocketAddress;
                        memset(&ClientSocketAddress, 0, sizeof(ClientSocketAddress));
                        int ClientSocketAddressSize = sizeof(ClientSocketAddress);
                        SOCKET ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientSocketAddress,&ClientSocketAddressSize);
                        FD_SET(ClientSocket, &ReadSockets);
                    }
                    else
                    {
                        char Buffer[1024] = { 0 };
                        int ReceiveLength = recv(ReadSockets.fd_array[i], Buffer, 1024, 0);

                        if (ReceiveLength <= 0)
                        {
                            closesocket(ReadSockets.fd_array[i]);
                        }
                        else
                        {
                            int SendLength = send(ReadSockets.fd_array[i], Buffer, ReceiveLength, 0); 
                        }
                    }
                }
            }
        }
    }
    return 0;
}