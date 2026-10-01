#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;

bool entry_open = false;

bool exit_open = false;

int entry_waiting = 0;
int exit_waiting = 0;


void entryGate(){
    while(true){
        std::unique_lock<std::mutex> lock(mtx);
        //venter på bil
        cv.wait(lock, [] {
            return entry_waiting > 0;
        });
        std::cout << "Entry Gate: car requested gate" << std::endl;
        //åbner
        entry_open = true;
        std::cout << "Entry Gate: opening gate" << std::endl;
        //fortæl bil skal køre igennem
        cv.notify_all();
        //venter på bil kørt igennem
        cv.wait(lock, [] {
            return entry_waiting == 0;
        });
        std::cout << "Entry Gate: car passed" << std::endl;

        //lukker
        entry_open = false;
        std::cout << "Entry Gate: closing gate" << std::endl;

    }
}

void exitGate(){
    while(true){
        std::unique_lock<std::mutex> lock(mtx);
        //venter på bil
        cv.wait(lock, [] {
            return exit_waiting >0;
        });
        std::cout << "Exit Gate: car requested gate" << std::endl;
        //åbner
        exit_open = true;
        std::cout << "Exit Gate: opening gate" << std::endl;
        //fortæl bil skal køre igennem
        cv.notify_all();
        //venter på bil kørt igennem
        cv.wait(lock, [] {
            return exit_waiting == 0;
        });
        std::cout << "Exit Gate: car passed" << std::endl;

        //lukker
        exit_open = false;
        std::cout << "Exit Gate: closing gate" << std::endl;
    }
}

void car(int carId, int pause){
    
    //detekter car ved gate
    {
        std::lock_guard<std::mutex> lock(mtx);
        entry_waiting++;
        std::cout << "Car " << carId << " waiting for entry gate" << std::endl;
    }
    cv.notify_all();
    //venter på entry gate er open
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] {
            return entry_open;
        });
    }
    //bilen kører igennem
    std::cout << "Car " << carId << " drives through entry gate" <<std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        entry_waiting--;
    }
    cv.notify_all();

    //mængde tid der ventes i parkinlot
     std::this_thread::sleep_for(std::chrono::seconds(pause));

    //venter i kø til exit gate
    {
        std::lock_guard<std::mutex> lock(mtx);
        exit_waiting++;
        std::cout << "Car " << carId << " waiting for exit gate" << std::endl;
    }
    cv.notify_all();
    //venter på exit gate er åben
    {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [] {
            return exit_open;
        });
    }
    //bilen kører igennem exit gate
    std::cout << "Car " << carId << " drives through exit gate" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        exit_waiting--;
    }
    cv.notify_all();

}

int main(){
    std::thread entry_gate_thread(entryGate);
    std::thread exit_gate_thread(exitGate);

    std::thread car1_thread(car,1,5);
    std::thread car2_thread(car,2,20);
    std::thread car3_thread(car,3,10);


    entry_gate_thread.join();
    exit_gate_thread.join();
    car1_thread.join();
    car2_thread.join();
    car3_thread.join();
}