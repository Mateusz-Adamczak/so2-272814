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

Mutex clients_mutex;
std::vector<int> clients;

int main() {
    return 0;
}
