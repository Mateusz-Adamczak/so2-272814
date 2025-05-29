#include <iostream>
#include <thread>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 12345


void receiveMessages(int socket_fd) {
    std::cout<<"(Message receiver placeholder)\n";
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
        std::cout<<"[" + nickname + "] ";
        std::cin>>msg;
        
        if (msg=="/exit") {
            break;
        }
        
        msg = "[" + nickname + "] " + msg + "\n";
        std::cout<<msg; // temp
    }
    
    close(socket_fd);
    receiver.join();
    
    return 0;
}
