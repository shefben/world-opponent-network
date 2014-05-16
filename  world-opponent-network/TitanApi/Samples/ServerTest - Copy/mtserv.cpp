#include <WinSock2.h>                //Socket Header
#include <iostream>        //I/O
#include <memory>        //shared_ptr
#include <list>        //List for our connections
#pragma comment(lib, "ws2_32.lib")    //Link the socket lib
using namespace std;                

void Start();
void SetupServerSocket();
DWORD WINAPI AcceptConnections(LPVOID lpParam);
DWORD WINAPI ProcessConnection(LPVOID lpParam);

sockaddr_in serverAddr;        //Server Address
SOCKET _serverSocket;        //Server Socket

int port = 1012;        //port to listen
struct ConnectionInfo        //Parameters to be cast
{
    SOCKET sock;
    HANDLE thread;
    sockaddr_in clientAddr;
};

HANDLE _acceptThread;        //our accept Thread
list<std::shared_ptr<ConnectionInfo>> _connections;        //our list of connections

DWORD WINAPI ProcessConnection(LPVOID lpParam)
{
    //Get parameters from lpParam
    ConnectionInfo connection = *(ConnectionInfo*)lpParam;

    while (true)
    {
        //Buffer for receiving and passing it back
        char buffer[256];

        //Receive the data
        int nRec = recv(connection.sock, buffer, sizeof(buffer) -1, NULL);

        //Exclude crap characters
        buffer[nRec] = '\0';

        if (nRec > 0)
        {
            cout << "Received: '" << buffer << "'" << endl;

            //Foreach client send the data through
            for each (shared_ptr<ConnectionInfo> conn in _connections)
            {
                send(conn->sock, buffer, strlen(buffer), NULL);
            }

            cout << "The data is sent to: " << _connections.size() << " connected clients." << endl;
        }
    }
}

DWORD WINAPI AcceptConnections(LPVOID lpParam)
{
    while (true)
    {
        //Create a shared_ptr for paramter
        shared_ptr<ConnectionInfo> connection(new ConnectionInfo);    

        //Accept the connection
        connection->sock = accept(_serverSocket, (sockaddr *)&connection->clientAddr, NULL);

        //Spawn a new thread for it
        connection->thread = CreateThread(NULL, NULL, ProcessConnection, (LPVOID)connection.get(), NULL, NULL);

        //Add it to the list
        _connections.push_back(connection);
    }
}

void SetupServerSocket()
{
    //initialize WSA
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2,2), &wsaData);

    //Setup the Socket
    _serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    //Setup the serverAddress
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    //Bind the socket
    bind(_serverSocket, (sockaddr *)&serverAddr, sizeof(serverAddr));

    //Listen the socket
    listen(_serverSocket, 0);

    cout << "_serverSocket is bound and listening.." << endl;
}

void Start()
{
    cout << "Example Threaded Server - iVision" << endl
         << "Starting up.." << endl;
    SetupServerSocket();
    _acceptThread = CreateThread(NULL, NULL, AcceptConnections, NULL, NULL, NULL);
}

int main()
{
    Start();

    while (true)
    {
        //buffer to hold our data
        char buffer[256];

        //get input
        cin.getline(buffer, 256);

        //send data to all clients:
        for each (shared_ptr<ConnectionInfo> conn in _connections)
        {
            send(conn->sock, buffer, strlen(buffer), NULL);
        }

        cout << "The data is sent to: " << _connections.size() << " connected clients." << endl;
    }

    return 0;
} 