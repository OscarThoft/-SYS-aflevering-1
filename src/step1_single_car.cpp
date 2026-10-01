#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>
#include <condition_variable>

std::mutex mtx;
std::condition_variable cv;

bool entry_open = false;
bool entry_car_present = false;
bool exit_open = false;
bool exit_car_present = false;


void entryGate(){
    while(true){
        std::unique_lock<std::mutex> lock(mtx);
        //venter på bil
        cv.wait(lock, [] {
            return entry_car_present;
        });
        std::cout << "Entry Gate: car requested gate" << std::endl;
        //åbner
        entry_open = true;
        std::cout << "Entry Gate: opening gate" << std::endl;
        //fortæl bil skal køre igennem
        cv.notify_all();
        //venter på bil kørt igennem
        cv.wait(lock, [] {
            return !entry_car_present;
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
            return exit_car_present;
        });
        std::cout << "Exit Gate: car requested gate" << std::endl;
        //åbner
        exit_open = true;
        std::cout << "Exit Gate: opening gate" << std::endl;
        //fortæl bil skal køre igennem
        cv.notify_all();
        //venter på bil kørt igennem
        cv.wait(lock, [] {
            return !exit_car_present;
        });
        std::cout << "Exit Gate: car passed" << std::endl;

        //lukker
        exit_open = false;
        std::cout << "Exit Gate: closing gate" << std::endl;
    }
}

void car(){
    //detekter car ved gate
    while(true) 
    {
    {
        std::lock_guard<std::mutex> lock(mtx);
        entry_car_present = true;
        std::cout << "Car registered" << std::endl;
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
    std::cout << "Car drives through gate" <<std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        entry_car_present = false;
    }
    cv.notify_all();


    {
        std::lock_guard<std::mutex> lock(mtx);
        exit_car_present = true;
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
    std::cout << "Car drives through exit gate" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        exit_car_present = false;
    }
    cv.notify_all();
}
}

int main(){

    std::thread entry_gate_thread(entryGate);
    std::thread exit_gate_thread(exitGate);
    std::thread car_thread(car);

    entry_gate_thread.join();
    exit_gate_thread.join();
    car_thread.join();
return 0;
}