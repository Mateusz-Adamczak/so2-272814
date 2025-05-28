#include <iostream>
#include <thread>

void receiveMessages() {
    std::cout<<"(Message receiver placeholder)\n";
}

int main() {
    // Connect to server here
    std::cout<<"Connected to chat server."<<std::endl;
    
    std::string nickname;
    std::cout<<"Enter a nickname: \n";
    std::cin>>nickname;
    std::cout<<"You are now chatting as \""<<nickname<<"\".\n";
    
    std::thread receiver(receiveMessages);
    
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
    
    receiver.join();
    
    return 0;
}
