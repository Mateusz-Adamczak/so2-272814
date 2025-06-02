#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstring>
#include <algorithm>

#define PORT 12345

class Mutex {
private:
    volatile bool locked = false;

public:
    void lock() {
        while (__sync_lock_test_and_set(&locked, 1)) {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    void unlock() {
        __sync_lock_release(&locked);
    }
};

Mutex clients_mutex;
std::vector<int> clients;

void broadcastMessage(const std::string& msg, int sender_id) {
    clients_mutex.lock();
    
    for (int client_fd : clients) {
        if (client_fd != sender_id) {
            send(client_fd, msg.c_str(), msg.size(), 0);
        }
    }
    
    clients_mutex.unlock();
}

void handleClient(int client_fd) {
    char buffer[1024];
    
    while (true) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytesRead <= 0) {
            std::cout<<"A client disconnected.\n";
            break;
        }
        std::string msg(buffer);
        std::cout<<"Received message: "<<msg;
        broadcastMessage(msg, client_fd);
    }
    
    close(client_fd);
    clients_mutex.lock();
    clients.erase(std::remove(clients.begin(), clients.end(), client_fd), clients.end());
    clients_mutex.unlock();
}

int main() {
    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 5);
    std::cout<<"Chat server started on port "<<PORT<<".\n";
    
    while (true) {
        sockaddr_in client_address{};
        socklen_t client_len = sizeof(client_address);
        int client_fd = accept(server_fd, (struct sockaddr*)&client_address, &client_len);
        
        clients_mutex.lock();
        clients.push_back(client_fd);
        clients_mutex.unlock();
        
        std::thread(handleClient, client_fd).detach();
        std::cout<<"New client connected.\n";
    }

    close(server_fd);
    return 0;
}
