//Exercício 5: Receber CAN e enviar bytes recebidos por Ethernet
//Client

#include <iostream>
#include <unistd.h>
#include <WinSock2.h>

#pragma comment(lib, "ws2_32.lib")

typedef uint32_t canid_t;

struct can_frame {
    union {
        canid_t can_id;
        struct {
            uint32_t id : 29;
            uint32_t err : 1;
            uint32_t rtr : 1;
            uint32_t eff : 1;
        };
    };
    union
        {
        uint8_t can_dlc;
        uint8_t dlc;
        };
    uint8_t __pad;
    uint8_t __res0;
    uint8_t __res1;
    uint8_t data[8] __attribute__((aligned(8)));
};

int main() {

    int maxRetries = 1000;
    int retryCount = 0;

    do {
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "WSAStartup failed." << std::endl;
            return 1;
        }

        SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Socket creation failed." << std::endl;
            WSACleanup();
            return 1;
        }

        sockaddr_in serverAddress;
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_port = htons(1111);  // Change the port as needed
        serverAddress.sin_addr.s_addr = inet_addr("10.45.11.111");  // Replace with the server's IP address

        if (connect(clientSocket, reinterpret_cast<SOCKADDR*>(&serverAddress), sizeof(serverAddress)) == SOCKET_ERROR) {
            std::cerr << "Connection failed." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        bool responseReceived = false;
        uint32_t idcan;
        std::cout << "Enter IDCAN (in hexadecimal): ";
        std::cin >> std::hex >> idcan;

        if (send(clientSocket, reinterpret_cast<char*>(&idcan), sizeof(idcan), 0) == SOCKET_ERROR) {
            std::cerr << "Error sending IDCAN." << std::endl;
            closesocket(clientSocket);
            WSACleanup();
            return 1;
        }

        while (retryCount < maxRetries && responseReceived == false) {
            char buffer[sizeof(struct can_frame)];
            ssize_t bytes_received = recv(clientSocket, buffer, sizeof(buffer), 0);
            if (bytes_received < 0) {
                perror("Read error");
                break;
            }
            else {
                if (bytes_received == sizeof(struct can_frame)) {
                    struct can_frame frame;
                    memcpy(&frame, buffer, sizeof(struct can_frame));
                    memset(buffer, 0, sizeof(buffer));
                    if (frame.can_id == idcan) {
                        responseReceived = true;

                        std::cout << "Received IDCAN from server: " << std::hex << frame.can_id << std::dec << std::endl;
                        std::cout << "Received CAN message data: ";
                        for (int i = 0; i < frame.can_dlc; i++) {
                            std::cout << "0x" << std::hex << (unsigned int)frame.data[i] << " ";
                        }
                        std::cout << std::endl;
                    }
                }
                else {
                    std::cerr << "Ainda nao recebi esse dado!" << std::endl;
                    responseReceived = false;
                    retryCount++;
                }
            }
        }

        if (responseReceived == false) {
            std::cout << "Max retry attempts reached. No response received." << std::endl;
        }

        closesocket(clientSocket);

        std::cout << "Do you want to continue? Y=1 | N=0  -->  ";
        std::cin >> retryCount;
             
    } while (retryCount);

    WSACleanup();

    return 0;
}

