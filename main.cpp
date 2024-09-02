#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <cstring>  // For memset
#pragma comment(lib, "ws2_32")
using namespace std;

int main() {
    WSAData wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) 
    {
        cout << "WSAStartup failed with error: " << WSAGetLastError() << endl;
        return 1;
    }

    SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ServerSocket == INVALID_SOCKET)
    {
        cout << "Socket creation failed with error: " << WSAGetLastError() << endl;
        WSACleanup();
        return 1;
    }

    sockaddr_in ServerSocketAddress;
    memset(&ServerSocketAddress, 0, sizeof(ServerSocketAddress));
    ServerSocketAddress.sin_family = AF_INET;
    ServerSocketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
    ServerSocketAddress.sin_port = htons(10880);

    if (bind(ServerSocket, (struct sockaddr*)&ServerSocketAddress, sizeof(ServerSocketAddress)) == SOCKET_ERROR) {
        cout << "Bind failed with error: " << WSAGetLastError() << endl;
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    if (listen(ServerSocket, 5) == SOCKET_ERROR) {
        cout << "Listen failed with error: " << WSAGetLastError() << endl;
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    cout << "Waiting for client connection..." << endl;

    sockaddr_in ClientSocketAddress;
    int ClientSocketAddressSize = sizeof(ClientSocketAddress);
    SOCKET ClientSocket = accept(ServerSocket, (struct sockaddr*)&ClientSocketAddress, &ClientSocketAddressSize);
    if (ClientSocket == INVALID_SOCKET) {
        cout << "Accept failed with error: " << WSAGetLastError() << endl;
        closesocket(ServerSocket);
        WSACleanup();
        return 1;
    }

    // 클라이언트에게 "10 + 20" 문자열 전송
    const char* problem = "10 + 20";
    send(ClientSocket, problem, (int)strlen(problem), 0);

    // 클라이언트로부터 결과 수신
    char buffer[1024] = { 0 };
    int recvBytes = recv(ClientSocket, buffer, sizeof(buffer) - 1, 0); // -1 for null terminator
    if (recvBytes > 0) {
        buffer[recvBytes] = '\0'; // Null-terminate the received data
        printf("Server received: %s\n", buffer);

        // 클라이언트로부터 받은 문자열이 "30"인지 확인
        if (strcmp(buffer, "30") == 0) {
            const char* message = "메롱";
            send(ClientSocket, message, (int)strlen(message), 0);
        }
    }
    else if (recvBytes == 0) {
        cout << "Connection closed by client." << endl;
    }
    else {
        cout << "recv failed with error: " << WSAGetLastError() << endl;
    }

    closesocket(ClientSocket);
    closesocket(ServerSocket);
    WSACleanup();

    return 0;
}