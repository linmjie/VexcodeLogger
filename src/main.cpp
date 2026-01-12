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

    Brain.Screen.printAt( 10, 50, "Hello V5" );

    auto LOGGER = Logger::create(&Brain.Screen)
        .setMaxLineSize(250)
        .printToStdout()
        .build();

    for (int i = 0; i < 10; i++) {
        LOGGER.println("%d^2 = %d", i, i*i);
    }
   
    while(1) {
        
        // Allow other tasks to run
        this_thread::sleep_for(10);
    }
}