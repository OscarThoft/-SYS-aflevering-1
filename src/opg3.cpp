#include <iostream>
#include <mutex>
#include <chrono>
#include <thread>
#include <functional>
#include <condition_variable>

std::mutex mtx;
std::mutex capacity_mtx;
std::condition_variable cv;
std::condition_variable capacity_cv;

bool entry_open = false;
int entry_car_present = 0;
bool exit_open = false;
int exit_car_present = 0;

int capacity = 3;


void entryGate(){
    while(true){
        std::unique_lock<std::mutex> lock(mtx);
        //venter på bil
        cv.wait(lock, [] {
            return entry_car_present > 0;
        });
        std::cout << "Entry Gate: car requested gate" << std::endl;
        //åbner
        entry_open = true;
        std::cout << "Entry Gate: opening gate" << std::endl;
        //fortæl bil skal køre igennem
        cv.notify_all();
        //venter på bil kørt igennem
        cv.wait(lock, [] {
            return entry_car_present== 0;
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
            return exit_car_present > 0;
        });
        std::cout << "Exit Gate: car requested gate" << std::endl;
        //åbner
        exit_open = true;
        std::cout << "Exit Gate: opening gate" << std::endl;
        //fortæl bil skal køre igennem
        cv.notify_all();
        //venter på bil kørt igennem
        cv.wait(lock, [] {
            return exit_car_present== 0;
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
        entry_car_present++;
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

{
    std::unique_lock<std::mutex> lock(capacity_mtx);

    if (capacity == 0) {
        std::cout << "PARKING LOT FULL - Car " << carId << " is waiting" << std::endl;
    }

    capacity_cv.wait(lock, [] {
        return capacity > 0;
    });

    capacity--;

    std::cout << "Car " << carId << " entered parking lot. Free spaces: " << capacity << std::endl;
}

    //bilen kører igennem
    std::cout << "Car " << carId << " drives through entry gate" <<std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::lock_guard<std::mutex> lock(mtx);
        entry_car_present--;
    }
    cv.notify_all();

    //mængde tid der ventes i parkinlot
     std::this_thread::sleep_for(std::chrono::seconds(pause));

    //venter i kø til exit gate
    {
        std::lock_guard<std::mutex> lock(mtx);
        exit_car_present++;
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
        exit_car_present--;
    }

    cv.notify_all();

    {
    std::lock_guard<std::mutex> lock(capacity_mtx);

    capacity++;

    std::cout << "Car " << carId
              << " left parking lot. Free spaces: "
              << capacity << std::endl;
}

capacity_cv.notify_one();

}

int main(){
    std::thread entry_gate_thread(entryGate);
    std::thread exit_gate_thread(exitGate);

    std::thread car1_thread(car,1,5);
    std::thread car2_thread(car,2,20);
    std::thread car3_thread(car,3,10);
    std::thread car4_thread(car,4,2);
    std::thread car5_thread(car,5,12);
    


    entry_gate_thread.join();
    exit_gate_thread.join();
    car1_thread.join();
    car2_thread.join();
    car3_thread.join();
    car4_thread.join();
    car5_thread.join();
}