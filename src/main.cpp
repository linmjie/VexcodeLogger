/*----------------------------------------------------------------------------*/
/*                                                                            */
/*    Module:       main.cpp                                                  */
/*    Author:       deolin                                                    */
/*    Created:      1/9/2026, 10:59:48 PM                                     */
/*    Description:  V5 project                                                */
/*                                                                            */
/*----------------------------------------------------------------------------*/
#include "vex.h"
#include "logger.h"

using namespace vex;

// A global instance of vex::brain used for printing to the V5 brain screen
brain Brain;

// define your global instances of motors and other devices here


int main() {

    auto LOGGER = Logger::create(&Brain.Screen)
        .disableWordWrap()
        .build();
    
    for (int i = 0; i < 100; i++) {
        LOGGER.println("%d^2: %d", i, i*i);
        LOGGER.print(" [NOT ON NEW LINE]");
        this_thread::sleep_for(100);
    }
    

    while(1) {
        
        // Allow other tasks to run
        this_thread::sleep_for(10);
    }
}