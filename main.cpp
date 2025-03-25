#include <iostream>
#include <thread>
#include <vector>
#include <chrono>

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

class Waiter {
private:
    Mutex lock;
    std::vector<bool> forks;

public:
    Waiter(int numForks) : forks(numForks, true) {}
    
    void requestAccess(int id, int forkLeft, int forkRight) {
        while (true) {
            lock.lock();
            if (forks[forkLeft] && forks[forkRight]) {
                forks[forkLeft] = false;
                forks[forkRight] = false;
                std::cout<<"Philosopher "<<id<<" takes forks "
                    <<forkLeft<<" and "<<forkRight<<std::endl;
                lock.unlock();
                return;
            }
            lock.unlock();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
    
    void releaseAccess(int id, int forkLeft, int forkRight) {
        lock.lock();
        forks[forkLeft] = true;
        forks[forkRight] = true;
        std::cout<<"Philosopher "<<id<<" returns forks "
            <<forkLeft<<" and "<<forkRight<<std::endl;
        lock.unlock();
    }
};

class Philosopher {
private:
    int id;
    int forkLeft;
    int forkRight;
    Waiter& waiter;
    volatile bool& running;

public:
    Philosopher(int _id, int _forkLeft, int _forkRight, Waiter& _waiter, volatile bool& _running) : id(_id), forkLeft(_forkLeft), forkRight(_forkRight), waiter(_waiter), running(_running) {}
    
    void dine() {
        //std::cout<<"TEMP: philosopher "<<id<<" created."<<std::endl;
        //std::cout<<"id: "<<id<<", left fork: "<<forkLeft<<", right fork: "<<forkRight<<std::endl;
        while (running) {
            think();
            waiter.requestAccess(id, forkLeft, forkRight);
            eat();
            waiter.releaseAccess(id, forkLeft, forkRight);
        }
    }
    
    void think() {
        std::cout<<"Philosopher "<<id<<" is thinking"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000
            + rand() % 4000));
        
    }
    
    void eat() {
        std::cout<<"Philosopher "<<id<<" is eating"<<std::endl;
        std::this_thread::sleep_for(std::chrono::milliseconds(1000
            + rand() % 4000));
    }
};

void commandLoop(volatile bool& running) {
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

    Waiter waiter(philosophersCount);
    std::vector<std::thread> philosopherThreads;
    volatile bool running = true;
    for (int i = 0; i < philosophersCount; i++) {
        int forkLeft = i;
        int forkRight = (i + 1) % philosophersCount;
        
        philosopherThreads.emplace_back(&Philosopher::dine, Philosopher(i, forkLeft, forkRight, waiter, running));
    }
    
    std::thread commandThread(commandLoop, std::ref(running));
    commandThread.join();
    for (auto& t : philosopherThreads) { t.join(); }
    
    std::cout<<"Simulation ended"<<std::endl;
    return 0;
}
