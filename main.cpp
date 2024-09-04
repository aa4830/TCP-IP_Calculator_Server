#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <cstring>
#pragma comment(lib, "ws2_32")
using namespace std;

#pragma pack(push, 1) // 구조체의 멤버들이 1바이트 경계로 정렬되도록 설정
struct Data
{
    char Message;
};
#pragma pack(pop) // 이전 메모리 정렬 설정을 복원. 뒤로 사용하는 구조체들은 정렬안되게 하기.

int main()
{
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData));

    SOCKET ListenSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    sockaddr_in ServerSocketAddress;
    memset(&ServerSocketAddress, 0, sizeof(ServerSocketAddress));
    ServerSocketAddress.sin_family = AF_INET;
    ServerSocketAddress.sin_addr.s_addr = inet_addr("192.168.3.164");
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
                        cout << "새" << inet_ntoa(ServerSocketAddress.sin_addr) << "가 접속했당" << endl; // inet_ntoa : IP 주소를 이진 형태로 변환
                    }
                    else
                    {
                        Data RecvPacket;
                        int ReceiveLength = recv(ReadSockets.fd_array[i], (char*)&RecvPacket, sizeof(RecvPacket), 0);

                        if (ReceiveLength <= 0)
                        {
                            cout << "클라이언트가 나갔당" << endl;
                            continue;
                        }
                        else
                        {
                            for (int j = 0; j < (int)ReadSockets.fd_count; ++j)
                            {
                                if (ReadSockets.fd_array[j] != ListenSocket)
                                {
                                    Data Packet;
                                    Packet.Message = RecvPacket.Message;
                                    send(ReadSockets.fd_array[j], (char*)&Packet, sizeof(Packet), 0);
                                    cout << "클라이언트" << inet_ntoa(ServerSocketAddress.sin_addr)<<"가 메세지를 보냈당" << endl;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}