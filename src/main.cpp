#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;

bool entry_open = false;
bool entry_car_pressent = false;


void entryGate(){
    while(true){
        std::unique_lock<std::mutex> lock(mtx);
        //venter på bil
        cv.wait(lock, [] {
            return entry_car_pressent;
        });
        std::cout << "Entry Gate: car requested gate" << std::endl;
        //åbner
        entry_open = true;
        std::cout << "Entry Gate: opening gate" << std::endl;
        //fortæl bil skal køre igennem
        cv.notify_one();
        //venter på bil kørt igennem
        cv.wait(lock, [] {
            return !entry_car_pressent;
        });
        std::cout << "Entry Gate: car passed" << std::endl;

        //lukker
        entry_open = false;
        std::cout << "Entry Gate: closing gate" << std::endl;

    }
}

void car(){
    {
        std::lock_guard<std::mutex> lock(mtx);
        entry_car_pressent = true;
        std::cout << "Bil registreret" << std::endl;
    }
    cv.notify_all();

    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] {
            return entry_open;
        });
    }
    std::cout << "Bilen koerer igennem\n";
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        entry_car_pressent = false;
    }
    cv.notify_all();
}

int main(){

}