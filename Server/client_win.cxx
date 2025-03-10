#include <client_win.hxx>

#define DEFAULT_BUFLEN 512

using std::string;
using std::cout;

typedef struct ClientSocket
{
    WSADATA wsaData;
    int iResult;
    struct addrinfo *address;
    string hostname;
    string port = "8080";
    SOCKET ConnectSocket;

    ClientSocket()
        : address(nullptr),
          ConnectSocket(INVALID_SOCKET)
    {
        
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

        
        iResult = getaddrinfo("127.0.0.1", port.c_str(), &hints, &address);

        if (iResult != 0) {
            cout << "getaddrinfo failed: " << iResult << '\n';
            WSACleanup();
            return;
        }

        ConnectSocket = socket(address->ai_family, address->ai_socktype, 
            address->ai_protocol);
            
        if (ConnectSocket == INVALID_SOCKET) {
            cout << "Error at socket(): " << WSAGetLastError() << '\n';
            freeaddrinfo(address);
            WSACleanup();
            return;
        }
        
        iResult = connect(ConnectSocket, address->ai_addr, (int)address->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            cout << "Connection failed with error: " << WSAGetLastError() << '\n';
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
        }
    }

    int sendData(const char* sendbuf) {
        if (ConnectSocket == INVALID_SOCKET) {
            cout << "Socket is invalid, cannot send data.\n";
            return -1;
        }
        
        int iResult = send(ConnectSocket, sendbuf, (int)strlen(sendbuf), 0);
        if (iResult == SOCKET_ERROR) {
            cout << "send failed: " << WSAGetLastError() << '\n';
            return -1;
        }
        
        cout << "Bytes Sent: " << iResult << '\n';
        return iResult;
    }
    
    bool shutdownSend() {
        if (ConnectSocket == INVALID_SOCKET) {
            cout << "Socket is invalid, cannot shutdown.\n";
            return false;
        }
        
        int iResult = shutdown(ConnectSocket, SD_SEND);
        if (iResult == SOCKET_ERROR) {
            cout << "shutdown failed: " << WSAGetLastError() << '\n';
            return false;
        }
        return true;
    }
    
    int receiveData() {
        char recvbuf[DEFAULT_BUFLEN];
        int recvbuflen = DEFAULT_BUFLEN;
        int bytesReceived = 0;
        
        if (ConnectSocket == INVALID_SOCKET) {
            cout << "Socket is invalid, cannot receive data.\n";
            return -1;
        }
        
        int iResult = 0;
        do {
            iResult = recv(ConnectSocket, recvbuf, recvbuflen, 0);
            if (iResult > 0) {
                cout << "Bytes received: " << iResult << '\n';
                
                recvbuf[iResult] = '\0';
                cout << "Data: " << recvbuf << '\n';
                bytesReceived += iResult;
            }
            else if (iResult == 0)
                cout << "Connection closed\n";
            else
                cout << "recv failed: " << WSAGetLastError() << '\n';
        } while (iResult > 0);
        
        return bytesReceived;
    }
    
    ~ClientSocket() {
        if (address) {
           freeaddrinfo(address);
        }
        if (ConnectSocket != INVALID_SOCKET) {
            closesocket(ConnectSocket);
        }
        WSACleanup();
    }
} ClientSocket;

namespace client
{
    static ClientSocket *clientSocket = nullptr;

    void init() {
        if (clientSocket == nullptr) {
            clientSocket = new ClientSocket();
        }
    }
    
    void cleanup() {
        if (clientSocket != nullptr) {
            delete clientSocket;
            clientSocket = nullptr;
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
        if (clientSocket == nullptr) {
            return 1;
        }
        
        
        if (clientSocket->ConnectSocket == INVALID_SOCKET) {
            cout << "Connection failed, socket is invalid\n";
            return 1;
        }
        
        return 0; 
    }
    
    int sendMessage(const char* message) {
        if (clientSocket == nullptr) {
            cout << "Client socket not initialized\n";
            return -1;
        }
        return clientSocket->sendData(message);
    }
    
    int receiveMessages() {
        if (clientSocket == nullptr) {
            cout << "Client socket not initialized\n";
            return -1;
        }
        return clientSocket->receiveData();
    }
    
    bool closeConnection() {
        if (clientSocket == nullptr) {
            cout << "Client socket not initialized\n";
            return false;
        }
        return clientSocket->shutdownSend();
    }
}

int main()
{
    cout << "Client starting...\n";
  
    client::init();
    
    if (client::checkSocket() != 0) {
        cout << "Failed to initialize client socket\n";
        client::cleanup();
        return 1;
    }
    
    cout << "Connected to server successfully\n";
    
    const char* testMessage = "This is a test message";
    client::sendMessage(testMessage);
    
    client::closeConnection();
    
    client::receiveMessages();
    
    client::cleanup();
    
    cout << "Client terminated\n";
    
    return 0;
}