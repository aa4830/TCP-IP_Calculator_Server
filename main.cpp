#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#include <WinSock2.h>
#include <iostream>
#include <vector>
#include <cstring>
#include <string>
#pragma comment(lib, "ws2_32")
using namespace std;

int OperatorSequence(char Operator)
{
    switch (Operator)
    {
    case '+': case '-' : return 1;
    case '*' : case '/' : return 2;
    default : return 0;
    }
}
float Calculator(float a, float b, char Operator)
{
    switch (Operator)
    {
    case '+': return a + b;
    case '-': return a - b;
    case '*': return a * b;
    case '/':
        if (b != 0) 
        {
            return a / b;
        }
        else
        {
            cout << "0���� ���� �� ����" << endl;
            return 0;
        }
    default:
        cout << "�����ڰ� ����" << Operator << endl;
        return 0;
    }
}
float FinalCalculator(string RecvNumAndOperator)
{
    cout << RecvNumAndOperator << endl;
    vector<float> NumbersVector;
    vector<char> OperatorVector;
    size_t Index = 0;
    while (Index < RecvNumAndOperator.length())
    {
        if (isdigit(RecvNumAndOperator[Index])) // ���� �ε����� ����?
        {
            size_t Length;
            float value = stof(RecvNumAndOperator.substr(Index), &Length); //���� Index���� �� ���ڿ��� �Ǽ��� �ٲٱ�, Length�� ���ڿ� ���� ��ȯ
            NumbersVector.push_back(value); // ��ȯ�� �Ǽ��� Numbers vector �ǵڿ� �߰�
            Index += Length; //���ڱ��� ��ŭ ���ڿ� Index ����
        }
        else if(strchr("+-*/", RecvNumAndOperator[Index])) // ���� �ε����� ������?
        {
            char currentOperator = RecvNumAndOperator[Index];
            
            while (!OperatorVector.empty() && OperatorSequence(OperatorVector.back()) >= OperatorSequence(RecvNumAndOperator[Index]))  // OperatorVector�� �����, ���� ���� �������� ���� ���� ���ڿ��� �������� �켱������ ���ؼ� ũ�� ����ϰ� ���� ����
            {
                float Value2 = NumbersVector.back(); // ������ ������ ��� ��ȯ
                NumbersVector.pop_back(); // ���� �� ��� ����
                float Value1 = NumbersVector.back();
                NumbersVector.pop_back();
                char Operator = OperatorVector.back();
                OperatorVector.pop_back();
                NumbersVector.push_back(Calculator(Value1, Value2, Operator));
            }
            OperatorVector.push_back(currentOperator);
            Index++;
        }
        else
        {
            Index++;
        }
    }
    while (!OperatorVector.empty())
    {
        float Value2 = NumbersVector.back();
        NumbersVector.pop_back();
        float Value1 = NumbersVector.back();
        NumbersVector.pop_back();
        char  Operator = OperatorVector.back();
        OperatorVector.pop_back();
        NumbersVector.push_back(Calculator(Value1, Value2, Operator));
    }
    if (NumbersVector.size() == 1)
    {
        return NumbersVector.back();
    }
    else
    {
        cout << "����� �ټ�" << endl;
        return 0;
    }
}

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
    ServerSocketAddress.sin_addr.s_addr = inet_addr("192.168.0.108");
    ServerSocketAddress.sin_port = htons(10880);
    bind(ListenSocket, (struct sockaddr*)&ServerSocketAddress, sizeof(ServerSocketAddress));
 
    listen(ListenSocket, 5);
    sockaddr_in ClientSocketAddress; 
    memset(&ClientSocketAddress, 0, sizeof(ClientSocketAddress));
    int ClientSocketAddressSize = sizeof(ClientSocketAddress); 
    SOCKET ClientSocket = accept(ListenSocket, (struct sockaddr*)&ClientSocketAddress, &ClientSocketAddressSize);
    if (ClientSocket)
    {
        cout << "Ŭ���̾�Ʈ ����" << endl;
    }
    while (1)
    {
        char RecvBuffer[1024] = { 0, };
        int ReceiveLength = recv(ClientSocket, RecvBuffer, sizeof(RecvBuffer), 0);
        if (ReceiveLength <= 0)
        {
            closesocket(ClientSocket);
            cout << "Ŭ���̾�Ʈ ����" << endl;
            break;
        }
        else
        {
            cout <<"Ŭ���̾�Ʈ�� ���� �� : "  <<  RecvBuffer << endl;
            string ReceivedMessage(RecvBuffer);
            float FinalResult  = FinalCalculator(ReceivedMessage);
            string finalResult = to_string(FinalResult);
            char SendBuffer[1024] = { 0, };
            strncpy(SendBuffer, finalResult.c_str(), sizeof(SendBuffer) - 1);
            
            cout << " ����� : " << SendBuffer << endl;
            send(ClientSocket, SendBuffer, finalResult.size(), 0);
        }
    }
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}