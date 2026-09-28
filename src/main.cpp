#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;

bool entry_open = false;
bool entry_requests = false;
bool entry_passed = false;


void entryGate(){
    while(true){
        std::unique_lock<std::mutex> lock(mtx);
        //venter på bil
        cv.wait(lock, [] {
            return entry_requests;
        });
        std::cout << "Entry Gate: car requested gate" << std::endl;
        //åbner
        entry_open = true;
        std::cout << "Entry Gate: opening gate" << std::endl;
        //fortæl bil skal køre igennem
        cv.notify_one();
        //venter på bil kørt igennem
        cv.wait(lock, [] {
            return entry_passed;
        });
        std::cout << "Entry Gate: car passed" << std::endl;

        //lukker
        entry_open = false;
        std::cout << "Entry Gate: closing gate" << std::endl;

        
        bool entry_requests = false;
        bool entry_passed = false;
    }
}

int main(){

}