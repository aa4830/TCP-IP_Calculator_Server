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
    case '*' : case '/' : return 1;
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
    default:
        cout << "연산자가 없음" << endl;
    }
}
float FinalCalculator(string RecvNumAndOperator)
{
    vector<float> NumbersVector;
    vector<char> OperatorVector;
    size_t Index = 0;
    while (Index < RecvNumAndOperator.length())
    {
        if (isdigit(RecvNumAndOperator[Index])) // 숫자면 true를 반환
        {
            size_t Length;
            float value = stof(RecvNumAndOperator.substr(Index), &Length); //현재 Index부터 뒤 문자열을 실수로 바꾸기, Length에 문자열 길이 반환
            NumbersVector.push_back(value); // 변환한 실수를 Numbers vector에 저장
            Index += Length; //문자길이 만큼 문자열 Index 증가
        }
        else if(strchr("+-*/", RecvNumAndOperator[Index])) // 현재 문자가 연산자가 맞나 한 번 더 확인
        {
            while (!OperatorVector.empty() && OperatorSequence(OperatorVector.back() >= OperatorSequence(RecvNumAndOperator[Index]))) // OperatorVector가 비었고, 가장 뒤의 연산자의 값이 현재 문자열의 연산자의 우선순위를 비교
            {
                float Value2 = NumbersVector.back(); // 벡터의 마지막 요소 반환
                NumbersVector.pop_back(); // 제일 뒤 요소 삭제
                float Value1 = NumbersVector.back();
                NumbersVector.pop_back();
                char Operator = OperatorVector.back();
                OperatorVector.pop_back();
                NumbersVector.push_back(Calculator(Value1, Value2, Operator));
            }
        }
        OperatorVector.push_back(RecvNumAndOperator[Index]);
        Index++;
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
    if (NumbersVector.size() != 1)

    return NumbersVector.back();
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
        char RecvBuffer[1024] = { 0, };
        int ReceiveLength = recv(ClientSocket, RecvBuffer, sizeof(RecvBuffer), 0);
        if (ReceiveLength <= 0)
        {
            closesocket(ClientSocket);
            cout << "클라이언트 퇴장" << endl;
            continue;
        }
        else
        {
            cout <<"클라이언트가 보낸 값 : "  <<  RecvBuffer << endl;
            string ReceivedMessage(RecvBuffer);
            size_t OperatorPosition = ReceivedMessage.find_first_of("+-*/");
            char Operator = ReceivedMessage[OperatorPosition];
            string FirstNum = ReceivedMessage.substr(0, OperatorPosition);
            string  SecNum = ReceivedMessage.substr(OperatorPosition + 1);

            float num1 = std::stof(FirstNum);
            float num2 = std::stof(SecNum);
            float Result;

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
            char SendBuffer[1024] = { 0, };
            string StringResult = to_string(Result);
            strcpy(SendBuffer, StringResult.c_str());
            
            cout << " 계산결과 : " << SendBuffer << endl;
            send(ClientSocket, SendBuffer, sizeof(SendBuffer), 0);
        }
    }
    closesocket(ListenSocket);
    WSACleanup();
    return 0;
}