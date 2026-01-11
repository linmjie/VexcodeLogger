#pragma once
#include "vex.h"

#include <vector>
#include <string>
#include <cstdarg>

using namespace vex;

/**
 * @brief Container for a string to print and whether to print it on it's own line
 */
struct outputLine {
    std::string contents;
    bool onNewLine;
};

class Logger {
    static const int WIDTH = 480;
    static const int HEIGHT = 240;
    std::vector<outputLine> buffer;

    //Properties
    brain::lcd* screen;
    uint32_t maxLineSize;
    bool logExternally;
    std::string logFile;

    void printBuffer();
    void addToBuffer(bool makeNewLine, const char* format, std::va_list args);

    public:
        //Prefer construction via builder
        Logger(brain::lcd* screen, uint32_t maxLineSize,
            bool logExternally, std::string logFile);

        //Function declarations copied from vex_brain.h
        //FUNCTION DEC. COPY START

        /** 
         * @brief Prints a number, string, or Boolean.
         * Provides formatting from printf
         * @param value Information to display on the screen
         * @note Some characters like \t and \\n don't work as they would normally
        */  
        template <class T>
        void     print( T value ) {
          if( std::is_same< T, int >::value )
            print( "%d", (int)value );
          else
          if( std::is_same< T, double >::value )
            print( "%.2f", (double)value );
          else {
            // primarily to handle modkit number
            if( (int)value == value )
              print( "%d", (int)value );
            else
              print( "%.2f", (double)value );
          }
        }

        /** 
         * @brief Prints a number, string, or Boolean.
         * Provides formatting from printf
         * @param format This is a reference to a char format that prints the value of variables
         * @param ... A variable list of parameters to insert into format string
         * @note Some characters like \t and \\n don't work as they would normally
        */          
        void     print( const char *format, ... );
        //FUNCTION DEC. COPY END

        /** 
         * @brief Prints a number, string, or Boolean.
         * Provides formatting from printf
         * @param value Information to display on the screen
         * @note Some characters like \t and \\n don't work as they would normally
        */  
        template <class T>
        void println( T value ) {
          if( std::is_same< T, int >::value )
            println( "%d", (int)value );
          else
          if( std::is_same< T, double >::value )
            println( "%.2f", (double)value );
          else {
            // primarily to handle modkit number
            if( (int)value == value )
              println( "%d", (int)value );
            else
              println( "%.2f", (double)value );
          }
        }

        /** 
         * @brief Prints a number, string, or Boolean with a new line.
         * Provides formatting from printf
         * @param format This is a reference to a char format that prints the value of variables
         * @param ... A variable list of parameters to insert into format string
         * @note Some characters like \t and \\n don't work as they would normally
        */          
        void println( const char *format, ... );

        //CONSTRUCTION
    private: 
        class Builder {
            brain::lcd* screen;
            uint32_t maxLineSize = 200;
            bool logExternally = false;
            std::string logFile = "";

            public:
                Builder(brain::lcd *screen);

                /**
                 * @brief Any string printed will be cut off at this character limit
                 */
                Builder& setMaxLineSize(uint32_t bytes);

                Builder& logToStdOut(bool);

                /**
                 * @brief Any output from the logger can also be logged into a file.
                 * @param fileName The file path as a string.
                 */
                Builder& logToFile(std::string fileName);

                /**
                 * @brief Finishes the building process
                 * @return The completed customized logger
                 */
                Logger build();
        };

    public:
        /**
         * @brief Creates a new builder with some default values with the ability
         *  tack on methods from the builder class to customize
         * @param screen This is a pointer to the brain's screen the logger will output to.
         * Usually you would pass in @code &Brain.Screen @endcode
         */
        static Builder create(brain::lcd* screen);
};