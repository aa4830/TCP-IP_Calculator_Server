#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <cstring>
#include <string>
#pragma comment(lib, "ws2_32")
using namespace std;

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
        char Buffer[1024] = { 0, };
        short Result;
        int ReceiveLength = recv(ClientSocket, Buffer, (int)sizeof(Buffer), 0);
        Buffer[ReceiveLength] = '\0';
        if (ReceiveLength <= 0)
        {
            closesocket(ClientSocket);
        }
        else
        {
            string ReceivedMessage(Buffer);
            size_t OperatorPosition = ReceivedMessage.find_first_of("+-*/");
            char Operator = ReceivedMessage[OperatorPosition];
            string FirstNum = ReceivedMessage.substr(0, OperatorPosition);
            string  SecNum = ReceivedMessage.substr(OperatorPosition + 1);
            
            float num1 = std::stof(FirstNum);
            float num2 = std::stof(SecNum);

            switch (Operator)
            {
            case '+':
                Result = num1 + num2;
                break;
            case '-':
                Result = num1 - num2;
                break;
            case '*':
                Result = num1 * num2;
                break;
            case '/':
                if (num2 != 0)
                {
                    Result = num1 / num2;
                    break;
                }
            default:
                closesocket(ListenSocket);
                WSACleanup();
            }
            char Buffer[1024] = { 0, };
            sprintf(Buffer, "%d", Result);

            send(ClientSocket, Buffer, (int)sizeof(Buffer), 0);
        }
    }
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}