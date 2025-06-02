#include <iostream>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>

#define PORT 12345


void receiveMessages(int socket_fd) {
    char buffer[1024];
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(socket_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            std::cout<<"Disconnected from chat server.\n";
            break;
        }
        std::cout<<buffer;
    }
}

int main() {
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in server_address{};
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);
    
    if (connect(socket_fd, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        std::cout<<"Connection to chat server failed.\n";
        return 1;
    }
    
    std::cout<<"Connected to chat server."<<std::endl;
    
    std::string nickname;
    std::cout<<"Enter a nickname: \n";
    std::cin>>nickname;
    std::cout<<"You are now chatting as \""<<nickname<<"\".\n";
    
    std::thread receiver(receiveMessages, socket_fd);
    
    std::string msg;
    while (true) {
        std::getline(std::cin, msg);
        
        if (msg=="/exit") {
            std::cout<<"test";
            break;
        }
        
        msg = "[" + nickname + "] " + msg + "\n";
        send(socket_fd, msg.c_str(), msg.size(), 0);
    }
    
    close(socket_fd);
    receiver.join();
    
    return 0;
}
