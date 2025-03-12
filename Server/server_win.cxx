#include <server_win.hxx>

#define DEFAULT_BUFLEN 512

using std::string;
using std::cout;

// Flag for controlling server shutdown
std::atomic<bool> g_serverRunning(true);

typedef struct ServerSocket
{
    WSADATA wsaData;
    int iResult;
    struct addrinfo *address;
    string port = "8080";
    SOCKET ListenSocket;
    SOCKET ClientSocket;

    ServerSocket()
        : address(nullptr),
          ListenSocket(INVALID_SOCKET),
          ClientSocket(INVALID_SOCKET)
    {
        // Initialize Winsock
        ZeroMemory(&wsaData, sizeof(wsaData));
        iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
        
        if (iResult != 0) {
            cout << "WSAStartup failed with error: " << iResult << '\n';
            return;
        }

        struct addrinfo hints;
        ZeroMemory(&hints, sizeof(hints));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        hints.ai_protocol = IPPROTO_TCP;
        hints.ai_flags = AI_PASSIVE;

        iResult = getaddrinfo(NULL, port.c_str(), &hints, &address);

        if (iResult != 0) {
            cout << "getaddrinfo failed: " << iResult << '\n';
            WSACleanup();
            return;
        }

        ListenSocket = socket(address->ai_family, address->ai_socktype, 
        address->ai_protocol);
            
        if (ListenSocket == INVALID_SOCKET) {
            cout << "Error at socket(): " << WSAGetLastError() << '\n';
            freeaddrinfo(address);
            WSACleanup();
            return;
        }
        
      
        int optval = 1;
        if (setsockopt(ListenSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optval, sizeof(optval)) == SOCKET_ERROR) {
            cout << "setsockopt failed with error: " << WSAGetLastError() << '\n';
            freeaddrinfo(address);
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }
        
        iResult = bind(ListenSocket, address->ai_addr, (int)address->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            cout << "Bind failed with error: " << WSAGetLastError() << '\n';
            freeaddrinfo(address);
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }

        freeaddrinfo(address);
        address = nullptr;
        
        iResult = listen(ListenSocket, SOMAXCONN);
        if (iResult == SOCKET_ERROR) {
            cout << "Listen failed with error: " << WSAGetLastError() << '\n';
            closesocket(ListenSocket);
            WSACleanup();
            return;
        }
        
        cout << "Server listening on port " << port << '\n';
    }

    bool acceptClient() {
        cout << "Waiting for client connection...\n";
        
        
        if (ClientSocket != INVALID_SOCKET) {
            closesocket(ClientSocket);
            ClientSocket = INVALID_SOCKET;
        }
        
        ClientSocket = accept(ListenSocket, NULL, NULL);
        if (ClientSocket == INVALID_SOCKET) {
            cout << "Accept failed with error: " << WSAGetLastError() << '\n';
            return false;
        }
        cout << "Client connected\n";
        return true;
    }
    
    // server takes int now (dead code)
    int receiveAndEcho() {
        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;
        int totalBytesProcessed = 0;
        
        if (ClientSocket == INVALID_SOCKET) {
            cout << "Client socket is invalid, cannot receive data.\n";
            return -1;
        }
        
        do {
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0) {
                cout << "Bytes received: " << iResult << '\n';
                recvbuf[iResult] = '\0';
                cout << "Data received: " << int(recvbuf) << '\n';
                
                int receivedValue = *reinterpret_cast<int*>(recvbuf);

                Prime prime(receivedValue);

                std::string response = prime.prime_or_composite();
                cout << "Processing: " << response << '\n';

               
                int iSendResult = send(ClientSocket, response.c_str(), response.length(), 0);
                if (iSendResult == SOCKET_ERROR) {
                    cout << "Send failed with error: " << WSAGetLastError() << '\n';
                    return -1;
                }

                cout << "Bytes sent: " << iSendResult << '\n';
                totalBytesProcessed += iSendResult;
            }
            else if (iResult == 0)
                cout << "Connection closing...\n";
            else {
                cout << "Recv failed with error: " << WSAGetLastError() << '\n';
                return -1;
            }
        } while (iResult > 0);
        
        return totalBytesProcessed;
    }

    bool shutdownClient() {
        if (ClientSocket == INVALID_SOCKET) {
            return false;
        }
        
        iResult = shutdown(ClientSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            cout << "Shutdown failed with error: " << WSAGetLastError() << '\n';
            return false;
        }
        
    
        closesocket(ClientSocket);
        ClientSocket = INVALID_SOCKET;
        cout << "Client connection closed\n";
        
        return true;
    }
    
    ~ServerSocket() {
        if (address) {
            freeaddrinfo(address);
        }
        if (ClientSocket != INVALID_SOCKET) {
            closesocket(ClientSocket);
        }
        if (ListenSocket != INVALID_SOCKET) {
            closesocket(ListenSocket);
        }
        WSACleanup();
    }
} ServerSocket;

namespace server
{
    static ServerSocket *serverSocket = nullptr;

    void init() {
        if (serverSocket == nullptr) {
            serverSocket = new ServerSocket();
        }
    }
    
    void cleanup() {
        if (serverSocket != nullptr) {
            delete serverSocket;
            serverSocket = nullptr;
        }
    }

    int checkSocketResult(int iResult) {
        if (iResult != 0) {
            cout << "Socket error: " << iResult << '\n';
            return 1;
        }
        return 0;
    }

    int checkSocket() {
        if (serverSocket == nullptr) {
            return 1;
        }
        return checkSocketResult(serverSocket->iResult);
    }
    
    bool waitForClient() {
        if (serverSocket == nullptr) {
            cout << "Server socket not initialized\n";
            return false;
        }
        return serverSocket->acceptClient();
    }
    
    int processClientMessages() {
        if (serverSocket == nullptr) {
            cout << "Server socket not initialized\n";
            return -1;
        }
        return serverSocket->receiveAndEcho();
    }
    
    bool closeClientConnection() {
        if (serverSocket == nullptr) {
            cout << "Server socket not initialized\n";
            return false;
        }
        return serverSocket->shutdownClient();
    }
    
    
    void signalShutdown() {
        g_serverRunning = false;
    }
}

int main()
{
    cout << "Server starting...\n";
  
    server::init();
    
    if (server::checkSocket() != 0) {
        cout << "Failed to initialize server socket\n";
        server::cleanup();
        return 1;
    }
    
    cout << "Server initialized successfully\n";
    cout << "Press Ctrl+C to stop the server\n";
    
   
    while (g_serverRunning) {
        if (!server::waitForClient()) {
            cout << "Failed to accept client connection, retrying...\n";
           
            Sleep(1000);
            continue;
        }
        
      
        server::processClientMessages();
        
       
        server::closeClientConnection();
        
        cout << "Ready for next client connection\n";
    }
    
    server::cleanup();
    
    cout << "Server terminated\n";
    
    return 0;
}