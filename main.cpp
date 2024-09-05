#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <cstring>
#pragma comment(lib, "ws2_32")
using namespace std;

#pragma pack(push, 1) 
struct Data
{
    float Number1;
    char Operator;
    float Number2;
};
#pragma pack(pop) 

int main()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
    {
        return 1;
    };

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in ServerSocketAddress;
    memset(&ServerSocketAddress, 0, sizeof(ServerSocketAddress));
    ServerSocketAddress.sin_family = AF_INET;
    ServerSocketAddress.sin_addr.s_addr = inet_addr("192.168.3.164");
    ServerSocketAddress.sin_port = htons(10880);
    bind(ListenSocket, (struct sockaddr*)&ServerSocketAddress, sizeof(ServerSocketAddress));
 
    listen(ListenSocket, 5);
    sockaddr_in ClientSocketAddress; 
    memset(&ClientSocketAddress, 0, sizeof(ClientSocketAddress));
    int ClientSocketAddressSize = sizeof(ClientSocketAddress); 
    SOCKET ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientSocketAddress, &ClientSocketAddressSize);
    if (ClientSocket)
    {
        cout << "클라이언트 입장" << endl;
    }
    while (1)
    {
        Data RecvPacket;
        short Result;
        int ReceiveLength = recv(ClientSocket, (char*)&RecvPacket, sizeof(RecvPacket), 0);
        if (ReceiveLength <= 0)
        {
            closesocket(ClientSocket);
        }
        else
        {
            switch (RecvPacket.Operator)
            {
            case '+':
                Result = RecvPacket.Number1 + RecvPacket.Number2;
                break;
            case '-':
                Result = RecvPacket.Number1 - RecvPacket.Number2;
                break;
            case '*':
                Result = RecvPacket.Number1 * RecvPacket.Number2;
                break;
            case '/':
                if (RecvPacket.Number2 != 0)
                {
                    Result = RecvPacket.Number1 / RecvPacket.Number2;
                    break;
                }
            default:
                closesocket(ListenSocket);
                WSACleanup();
                return 1;
            }
            char Buffer[1024] = { 0, };
            sprintf(Buffer, "%d", Result);

            int SendLength = send(ClientSocket, Buffer, (int)sizeof(Buffer), 0);
        }
    }
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}