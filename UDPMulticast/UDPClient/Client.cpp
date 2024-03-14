
#define WIN32_LEAN_AND_MEAN

#include <ws2tcpip.h>
#include <windows.h>
#include <iostream>
#include <string>
using namespace std;

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 4096

#define SERVER_IP "127.0.0.1"
#define DEFAULT_PORT "8888"

SOCKET client_socket;

DWORD WINAPI Sender(void* param)
{
    while (true) {
        // cout << "Please insert your query for server: ";
        char query[DEFAULT_BUFLEN];
        cin.getline(query, DEFAULT_BUFLEN);
        send(client_socket, query, strlen(query), 0);

        // �������������� ������� ����� ������ ��������
        // string query;
        // getline(cin, query);
        // send(client_socket, query.c_str(), query.size(), 0);
    }
}

DWORD WINAPI Receiver(void* param)
{
    COORD point = { 0,1 };
    char Nickname[64];
    char color[3];
    char Message[DEFAULT_BUFLEN];
    int turn = 0;
    int nicksize = 0;
    int j = 0;
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    while (true) {
        char response[DEFAULT_BUFLEN];
        int result = recv(client_socket, response, DEFAULT_BUFLEN, 0);
        response[result] = '\0';
        nicksize = 0;
        turn = 0;
        j = 0;

        for (int i = 0; i < strlen(response); i++)
        {
            if (response[i] != ' ' && turn == 0)
            {
                Nickname[i] = response[i];
                nicksize++;
            }
            if (response[i] == ' ' && turn == 0 || turn == 1)
            {
                turn++;
                i++;
            }
            if (response[i] != ' ' && turn == 1)
            {
                color[turn - 1] = response[i];
            }
            if (turn == 2)
            {
                Message[j] = response[i];
                j++;
            }

            // cout << "Please insert your query for server: ";
        }
        Message[j] = '\0';
        color[2] = '\0';
        Nickname[nicksize] = '\0';
        std::string temp = color;
        int IColor = std::stoi(temp);
        SetConsoleCursorPosition(h, point);
        SetConsoleTextAttribute(h, IColor);
        // cout << "...\nYou have new response from server: " << response << "\n";
        cout << Nickname << ' ' << Message << "\n";
        point.Y++;
        SetConsoleTextAttribute(h, 7);
        //cout << response;
    }
}

BOOL ExitHandler(DWORD whatHappening)
{
    switch (whatHappening)
    {
    case CTRL_C_EVENT: // closing console by ctrl + c
    case CTRL_BREAK_EVENT: // ctrl + break
    case CTRL_CLOSE_EVENT:
        WSACleanup();
      return(TRUE);
        break;
    default:
        return FALSE;
    }
}

int main()
{
    // ���������� �������� ���� �������
    //SetConsoleCtrlHandler((PHANDLER_ROUTINE)ExitHandler, true);

    system("title Client");

    // initialize Winsock
    WSADATA wsaData;
    int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }
    addrinfo hints = {};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // ��������� ����� ������� � ����
    addrinfo* result = nullptr;
    iResult = getaddrinfo(SERVER_IP, DEFAULT_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 2;
    }

    addrinfo* ptr = nullptr;
    // �������� ������������ � ������, ���� �� �������
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next) {
        // ������� ����� �� ������� ������� ��� ����������� � �������
        client_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

        if (client_socket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 3;
        }

        // connect to server
        iResult = connect(client_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(client_socket);
            client_socket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (client_socket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 5;
    }

    CreateThread(0, 0, Sender, 0, 0, 0);
    CreateThread(0, 0, Receiver, 0, 0, 0);

    Sleep(INFINITE);
}