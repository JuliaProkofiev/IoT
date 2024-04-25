//Exercício 5: Receber CAN e enviar bytes recebidos por Ethernet
//Server

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <thread>
#include <mutex>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <random>

int s, can0, can1;
bool runThreads = true; // Flag to control thread execution
uint32_t clientSearchIDCAN; // Store the IDCAN received from the client
std::mutex mtx;
struct can_frame frameToSend;
bool enviou = false;

int initCAN(const char* can_interface) {
    struct sockaddr_can addr;
    struct ifreq ifr;

    if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("Socket creation failed");
        return -1;
    }

    strcpy(ifr.ifr_name, can_interface);
    ioctl(s, SIOCGIFINDEX, &ifr);
    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("Bind failed");
        return -1;
    }

    return s;
}

void receiveCANFrame() {
    while (runThreads) {
        struct can_frame frame;
        int nbytes = read(can0, &frame, sizeof(struct can_frame));
        if (nbytes < 0) {
            perror("Read error");
            return;
        }

        std::cout << "Received CAN frame ID: " << std::hex << frame.can_id << std::dec << " Data: ";
        for (int i = 0; i < frame.can_dlc; i++) {
            std::cout << "0x" << std::hex << static_cast<int>(frame.data[i]) << " ";
        }
        std::cout << std::endl;
        // Check if the received CAN frame has the IDCAN you are looking for
        if (frame.can_id == clientSearchIDCAN && enviou == false) {
            mtx.lock();
            frameToSend = frame;
            mtx.unlock();
            //std::cout << "clientSearchIDCAN: " << clientSearchIDCAN << std::endl;
        }
    }
}

void sendCANFrame(uint32_t can_id, uint8_t* data, uint8_t data_length) {
    while (runThreads) {
        struct can_frame frame;
        frame.can_id = can_id;
        frame.can_dlc = data_length;
        memcpy(frame.data, data, data_length);

        int nbytes = write(can1, &frame, sizeof(struct can_frame));
        if (nbytes < 0) {
            perror("Write error");
            return;
        }

        usleep(1000000); // Sleep for 1 second between transmissions
    }
}

void handleClient(int client_socket) {
   // while (runThreads) {
        if (enviou == false) {
            // Set a maximum number of retry attempts
            const int maxRetryAttempts = 5;
            int retryCount = 0;
            while (retryCount < maxRetryAttempts) {
                if (frameToSend.can_id == clientSearchIDCAN) {
                    send(client_socket, &frameToSend, sizeof(struct can_frame), 0);
                    enviou = true;
                    std::cout << "Sending CAN frame to Client - ID: " << std::hex << frameToSend.can_id << std::dec << " Data: ";
                    for (int i = 0; i < frameToSend.can_dlc; i++) {
                        std::cout << "0x" << std::hex << static_cast<int>(frameToSend.data[i]) << " ";
                    }
                    std::cout << std::endl;

                    break; // Response sent successfully, exit the loop
                }
                else {
                    // If the response isn't ready, wait for a short time and retry
                    std::cout << "retrycount: " << retryCount << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(1));
                    retryCount++;
                }
            }

            if (retryCount == maxRetryAttempts) {
                std::cout << "Max retry attempts reached. No response sent to the client." << std::endl;
            }
        }
        char buffer[4];
        ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == sizeof(uint32_t)) {
            uint32_t receivedIDCAN;
            memcpy(&receivedIDCAN, buffer, sizeof(receivedIDCAN));

            mtx.lock();
            clientSearchIDCAN = receivedIDCAN;
            mtx.unlock();
            enviou = false;
            //Inform the user about the received IDCAN
            std::cout << "Received IDCAN from client: " << std::hex << receivedIDCAN << std::dec << std::endl;
        }
        close(client_socket);
    //}
}

uint32_t randomHex(int size) {
    std::random_device rd;  // Obtain a random seed from the OS entropy device
    std::mt19937 gen(rd()); // Standard Mersenne Twister random number generator
    if (size == 2) {
        std::uniform_int_distribution<uint16_t> dis(0x00, 0xFF); // Define the range for your hexadecimal number
        return dis(gen);
    }
    else if (size == 6) {
        std::uniform_int_distribution<uint32_t> dis(0x000000, 0xFFFFFF); // Define the range for your hexadecimal number
        return dis(gen);
    }
    else {
        std::uniform_int_distribution<uint32_t> dis(0x00000000, 0xFFFFFFFF); // Define the range for your hexadecimal number
        return dis(gen);
    }
}

int main() {
    const char* can_0 = "can0"; // Replace with your CAN interface name
    const char* can_1 = "can1"; // Replace with your CAN interface name

    can0 = initCAN(can_0);
    can1 = initCAN(can_1);

    if (can0 == -1 && can1 == -1) {
        return -1;
    }

    // Create threads for receiving and sending CAN frames
    std::thread receiver(receiveCANFrame); // Pass an initial value of 0
    std::thread sender(sendCANFrame, randomHex(8), new uint8_t[8]{ 0x11, 0x22, 0xFF, 0xEF, 0xAB, 0x6C, 0xFF, 0x00 }, 8); // Example CAN ID and data

    // Set up a TCP server to listen for client connections
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Socket creation failed");
        return -1;
    }

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(1111); // Change the port as needed

    if (bind(server_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Bind failed");
        return -1;
    }

    listen(server_socket, 5); // Listen for up to 5 connections

    while (true) {
        int client_socket = accept(server_socket, NULL, NULL);
        std::thread client_handler(handleClient, client_socket);
        client_handler.detach(); // Allow the handler thread to run independently
    }

    receiver.join();
    sender.join();

    // Close the socket when done
    close(server_socket);
    runThreads = false; // Set the flag to stop the threads

    close(s);

    return 0;
}
