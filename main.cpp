#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

class Philosopher {
private:
    int id;
    int forkLeft;
    int forkRight;
    bool& running;

public:
    Philosopher(int _id, int _forkLeft, int _forkRight, bool& _running) : id(_id), forkLeft(_forkLeft), forkRight(_forkRight), running(_running) {}
    
    void dine() {
        //std::cout<<"TEMP: philosopher "<<id<<" created."<<std::endl;
        //std::cout<<"id: "<<id<<", left fork: "<<forkLeft<<", right fork: "<<forkRight<<std::endl;
        while (running) {
            think();
            // PLACEHOLDER - request access to resources
            eat();
            // PLACEHOLDER - release access to resources
        }
    }
    
    void think() {
        std::cout<<"Philosopher "<<id<<" is thinking"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 + rand() % 2000));
        
    }
    
    void eat() {
        std::cout<<"Philosopher "<<id<<" is eating"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000 + rand() % 2000));
    }
};

void commandLoop(bool& running) {
    std::string command;
    std::cout<<"Enter command..."<<std::endl;
    while (running) {
        std::cin>>command;
        if (command=="stop") {
            std::cout<<"Stopping simulation..."<<std::endl;
            running = false;
        } else {
            std::cout<<"Unknown command: "<<command<<std::endl;
        }
    }
}

int main() {
    int philosophersCount;
    std::cout<<"Number of philosophers: ";
    std::cin>>philosophersCount;
    
    if (philosophersCount < 2) {
        std::cout<<"At least 2 philosophers are required for the problem"<<std::endl;
        return 1;
    }

    std::vector<std::thread> philosopherThreads;
    bool running = true;
    for (int i = 0; i < philosophersCount; i++) {
        int forkLeft = i;
        int forkRight = (i + 1) % philosophersCount;
        
        philosopherThreads.emplace_back(&Philosopher::dine, Philosopher(i, forkLeft, forkRight, running));
    }
    
    std::thread commandThread(commandLoop, std::ref(running));
    commandThread.join();
    for (auto& t : philosopherThreads) { t.join(); }
    
    std::cout<<"Simulation ended"<<std::endl;
    return 0;
}
