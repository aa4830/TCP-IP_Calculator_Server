#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

int main()
{
	WSAData wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData); //구조체는 객체의 크기가 크므로 &를 써서 객체를 직접 참조하자
	
	SOCKET ServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	sockaddr_in ServerSocketAddress;
	memset(&ServerSocketAddress, 0, sizeof(ServerSocketAddress));
	ServerSocketAddress.sin_family = AF_INET;
	ServerSocketAddress.sin_addr.s_addr = inet_addr("127.0.0.1");
	ServerSocketAddress.sin_port = htons(10880);
	bind(ServerSocket, (struct sockaddr*)&ServerSocketAddress, sizeof(ServerSocketAddress));
	listen(ServerSocket, 5);

	sockaddr_in ClientSocketAddress;
	memset(&ClientSocketAddress, 0, sizeof(ClientSocketAddress));
	int ClientSocketAddressSize = sizeof(ClientSocketAddress);
	SOCKET ClientSocket = accept(ServerSocket, (struct sockaddr*)&ClientSocketAddress, &ClientSocketAddressSize);

	char Buffer[1024] = { 0, };
	int ReceiveLength = recv(ClientSocket, Buffer, 1024, 0);
	if (ReceiveLength == 0)
	{

	}
	else if (ReceiveLength < 0)
	{
		
	}
	else
	{
		int SendLength = send(ClientSocket, Buffer, ReceiveLength, 0); 
	}
	closesocket(ClientSocket);
	closesocket(ServerSocket);
	WSACleanup(); 

	return 0;
}