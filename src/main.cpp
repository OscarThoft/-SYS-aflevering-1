#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>

std::mutex mtx;

bool entry_open = false;


void entryGate(){
    while(true){
        std::unique_lock<std::mutex> lock(mtx);
    }
}

int main(){

}