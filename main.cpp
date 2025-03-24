#include <iostream>
#include <thread>

void commandLoop(bool& running) {
    std::string command;
    std::cout<<"Enter command..."<<std::endl;
    while (running) {
        std::cin>>command;
        if (command=="stop") {
            running = false;
        } else {
            std::cout<<"Unknown command: "<<command<<std::endl;
        }
    }
}

int main() {
    bool running = true;
    
    std::thread commandThread(commandLoop, std::ref(running));
    commandThread.join();
    
    std::cout<<"Simulation ended"<<std::endl;
    return 0;
}
